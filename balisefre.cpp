#include "balisefre.h"
#include <QReadWriteLock>
#include <QDataStream>
#include <QVector>
//#include <random.h>
//#include <vectormath.h>
//#include <parametricpse.h>
//#include <classicalpse.h>
//#include <vector.h>
#include <QMutex>
#include "balisemanager.h"
#include "fftw3.h"


//using namespace splab;
using namespace std;

FreCaculator::FreCaculator(QObject *parent) : QObject(parent)
{
    sampleRate = 63.221;
    cutOffFre = 6;
    resolution = 0.005;
    fillterRank = 25;
    double t = cutOffFre/resolution + 1;
    specDataSize = (size_t)(t);
}

void FreCaculator::slo_caculateFre(Balise balise)
{
	//备注 实际测试 100 0000 个点的fft仅需28ms
	fftwf_plan p;
	int len = BaliseManager::sampleDataVec.size();
	fftwf_plan pifft;
	float *in = BaliseManager::sampleDataVec.data();
	fftwf_complex *out = (fftwf_complex *)fftwf_malloc(sizeof(fftwf_complex) * len);
	p = fftwf_plan_dft_r2c_1d(len, in, out, FFTW_ESTIMATE);
	fftwf_execute(p);
	// 输出幅度谱
	QVector<float> freVec;
	freVec.resize(len/6);
	for (int i = 0; i < len/6; i++)
	{
		freVec[i] = sqrt(out[i][0] * out[i][0] + out[i][1] * out[i][1]);  
	}
	emit sig_caculateFreFinished(balise, freVec);
}

//void FreCaculator::slo_caculateAverFre(Balise balise, QVector<double> sampleDataVec_out)
//{
//	sampleDataVec = sampleDataVec_out;
//	QVector<double>	spectrum; //每段的频谱结果
//	QVector<double> averSpec; //平均后的频谱结果
//
//	if (sampleDataVec.size() == 0)
//	{
//		balise.isFreFinished = true;
//		emit sig_caculateFreFinished(balise, averSpec);
//		return;
//	}
//    balise.resolution = resolution;
//    const size_t		calcNum = 50;//平均频谱迭代累加计算次数
//    const size_t		calcLen = sampleDataVec.size()/calcNum;	// 计算局部频谱时计算的数据长度
//
//	ejia.resize(2);
//	ejia[0].resize(calcLen);
//	ejia[1].resize(calcLen);
//	
//	ejian.resize(2);
//	ejian[0].resize(calcLen);
//	ejian[1].resize(calcLen);
//	 
//    averSpec.resize(specDataSize);
//    spectrum.resize(specDataSize);
//    memset(&spectrum[0], 0, spectrum.size() * sizeof(double));
//    for (size_t i=0, j=0; i<calcNum; i++, j += calcLen)
//    {
//        slo_caculateFre(j, calcLen, spectrum);
//        // 找出FH和FL的瞬时最大值和最小值
//        double	FH = 0;
//        double	FC = 0;
//        double	FL = 0;
//        getBaliseCF(averSpec, resolution, FH, FL, FC);
//
//        balise.FC[0]	=	balise.FC[0] < FC ? FC : balise.FC[0];
//        balise.FC[1]	=	balise.FC[1] > FC ? FC : balise.FC[1];
//
//        balise.FL[0]	=	balise.FL[0] < FL ? FL : balise.FL[0];
//        balise.FL[1]	=	balise.FL[1] > FL ? FL : balise.FL[1];
//
//        balise.FH[0]	=	balise.FH[0] < FH ? FH : balise.FH[0];
//        balise.FH[1]	=	balise.FH[1] > FH ? FH : balise.FH[1];
//
//        for (size_t j = 0; j < specDataSize; ++j)
//        {
//            averSpec[j]		+=		spectrum[j];
//        }
//    }
//    // 统计每个频点的平均值
//    for (size_t j = 0; j < specDataSize; ++j)
//    {
//        averSpec[j]		/=		calcNum;
//    }
//
//    // 获取FH，FL和FC的平均值
//    getBaliseCF(averSpec, resolution, balise.FH[2], balise.FL[2],
//        balise.FC[2]);
//
//    //向baliseManager发送频谱数据计算完成信号
//	balise.isFreFinished = true;
//    emit sig_caculateFreFinished(balise, averSpec);
//}
//
//void FreCaculator::slo_caculateFre(int s, size_t sampleDataLen, QVector<double> &spectrum)
//{
//    double	DELT_POWER = 0;
//    for (size_t i = 0; i < sampleDataLen; i++)
//    {
//        DELT_POWER += (sampleDataVec.at(s+i) * sampleDataVec.at(s+i));
//    }
//    DELT_POWER /= sampleDataLen;
//	//QVector<QVector<double>> ejia(2, QVector<double>(sampleDataLen));
//	//QVector<QVector<double>> ejian(2, QVector<double>(sampleDataLen));
//
//	for (int i = 0; i < sampleDataLen; ++i)
//	{
//		ejia[0][i] = sampleDataVec.at(s + i);
//		ejia[1][i] = 0;
//	}
//	ejian = ejia;
//    
//    // 计算改进型伯格算法的修正参数以及修正算法下发射系数的初始值
//    double improve_b[2]		={0};
//    double improve_c[2]		={0};
//    double improve_a[2][2]	={0};
//
//    for(size_t i=0; i<2; i++)
//    for(size_t j=2; j<sampleDataLen; j++)
//    {
//        improve_b[i] +=  sampleDataVec.at(s + j+(i+1)-2) * sampleDataVec.at(s + j+(i+1)-2)
//            + sampleDataVec.at(s + j-(i+1)) * sampleDataVec.at(s + j-(i+1));
//        improve_c[i] += sampleDataVec.at(s + j+(i+1)-2) * sampleDataVec.at(s + j-2)
//            + sampleDataVec.at(s + j-(i+1))* sampleDataVec.at(s + j);
//    }
//
//    improve_a[0][0] = -improve_c[0] / improve_b[0];
//    improve_a[1][0] = -improve_c[0]*(improve_b[1]-improve_c[1])/(improve_b[0]*improve_b[1]-improve_c[0]*improve_c[0]);
//    improve_a[1][1] = -(improve_b[0]*improve_c[1]-improve_c[0]*improve_c[0])/(improve_b[0]*improve_b[1]-improve_c[0]*improve_c[0]);
//
//
//    // 计算伯格算法的参数
//    double		refCofficient = improve_a[0][0];			//反射系数
//
//	QVector<QVector<double> > tempArray(2, QVector<double>(fillterRank));
//
//    for(size_t m=0, n=0; m<fillterRank; m++, n=m&1)
//    {
//        //计算前向预测滤波器系数
//        for(size_t i=0, j=(m-1)&1; i<m; i++)
//        {
//            tempArray[n][i] = tempArray[j][i] + refCofficient * tempArray[j][m-i-1];
//        }
//        tempArray[n][m] = refCofficient;
//        //计算预测误差功率 (迭代运算，重复利用同一个内存)
//        DELT_POWER = (1.0-refCofficient*refCofficient) * DELT_POWER;
//
//        //计算滤波器输出
//        for(size_t j=1,k=(m+1)&1; j<sampleDataLen; j++)
//        {
//            ejia[k][j] = ejia[n][j] + refCofficient * ejian[n][j-1];
//            ejian[k][j] = refCofficient * ejia[n][j] + ejian[n][j-1];
//        }
//
//        //计算反射系数
//        double temp1	=	0;
//        double temp2	=	0;
//        for(size_t k= m+2;k<sampleDataLen;k++)
//        {
//            temp1 += -2*(ejia[(m+1)&1][k] * ejian[(m+1)&1][k-1]);
//            temp2 += ejia[(m+1)&1][k]*ejia[(m+1)&1][k]+ejian[(m+1)&1][k-1]*ejian[(m+1)&1][k-1];
//        }
//
//        // 当分母为零时说明数据为错误数据，不再计算，直接返回
//        if ( 0 == temp2 )
//        {
//            return;
//        }
//        refCofficient		=		temp1 / temp2;
//    }
//
//
//    // 计算功率谱
//    double			step = 0;
//    const double	calcStep(2 * PI * resolution / sampleRate);
//    double			real, image, temp;
//
//    for (size_t i=0, j=(fillterRank-1)&1; i<specDataSize; i++, step+=calcStep)
//    {
//        real	=	1;
//        image	=	0;
//        temp	=	step;
//
//        for (size_t m=0; m<fillterRank; m++, temp+=step)
//        {
//            real	+=	tempArray[j][m] * cos(temp);		// 实部
//            image	+=	tempArray[j][m] * sin(-temp);		// 虚部
//        }
//
//        spectrum[i]		=		20 * log10(DELT_POWER / (real * real + image * image));
//    }
//
//}

void FreCaculator::slo_changeFreInfo(FreInfo freInfo)
{
    sampleRate = freInfo.sampleRate;
    cutOffFre = freInfo.cutOffFre;
    resolution = freInfo.resolution;
    fillterRank = freInfo.fillterRank;
    specDataSize = (size_t)(cutOffFre/resolution+1);
}

/*****************************************************************************************************************
 * GetBaliseCF函数收到频谱数据，计算出3.951附近和4.516附近两个频点值并得出中频值
 *
 * ***************************************************************************************************************/
void FreCaculator::getBaliseCF(const QVector<double>&spectrumData, double DELT, double& FH, double& FL, double& FC)
{
    // 搜寻下边频
    double	FLMaxValue(-1e100);
    size_t	FLSearchRange[]		=	{(size_t)(3.5/DELT), (size_t)(4.2/DELT)};
    for ( size_t i=FLSearchRange[0]; i<=FLSearchRange[1]; ++i )
    {
        if ( FLMaxValue < spectrumData[i] )
        {
            FLMaxValue		=		spectrumData[i];
            FL				=		i * DELT;
        }
    }

    // 搜寻上边频
    double	FHMaxValue(-1e100);
    size_t	FHSearchRange[]		=	{(size_t)(4.2/DELT), (size_t)(5/DELT)};
    for ( size_t i=FHSearchRange[0]; i<=FHSearchRange[1]; ++i )
    {
        if ( FHMaxValue < spectrumData[i] )
        {
            FHMaxValue		=		spectrumData[i];
            FH				=		i * DELT;
        }
    }

    FC		=		(FH + FL) / 2;
}
