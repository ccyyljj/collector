#include "balisedemod.h"
#include <QTextStream>
#include <QDebug>
#include <windows.h>
#include "balisemsganalyse.h"
/*******************************************************
 * 此类是应答器对应答器采样数据的解调类
 *
 *
 * *****************************************************/



double const   COS_Fd[37] = {
		1,
		0.382769511,
		- 0.706975002,
		- 0.923986464,
		- 0.000372692,
		0.923701154,
		0.70750197,
		- 0.382080787,
		- 0.999999722,
		- 0.383458023,
		0.706447642,
		0.92427126,
		0.001118075,
		- 0.92341533,
		- 0.708028544,
		0.381391851,
		0.999998889,
		0.384146321,
		- 0.705919889,
		- 0.924555544,
		- 0.001863458,
		0.923128994,
		0.708554725,
		- 0.380702702,
		- 0.9999975,
		- 0.384834406,
		0.705391744,
		0.924839313,
		0.002608839,
		- 0.922842145,
		- 0.709080513,
		0.380013342,
		0.999995555,
		0.385522278,
		- 0.704863208,
		- 0.925122569
};
double const  SIN_Fd[37] = { 
0,
0.923843873,
0.707238535,
- 0.382425176,
- 0.999999931,
- 0.383113794,
0.706711371,
0.924128926,
0.000745383,
- 0.923558306,
- 0.707765306,
0.381736345,
0.999999375,
0.383802199,
- 0.706183815,
- 0.924413466,
- 0.001490766,
0.923272226,
0.708291684,
- 0.381047303,
- 0.999998264,
- 0.384490391,
0.705655866,
0.924697492,
0.002236149,
- 0.922985633,
- 0.708817668,
0.380358049,
0.999996597,
0.385178369,
- 0.705127525,
- 0.924981005,
- 0.00298153,
0.922698528,
0.709343259,
- 0.379668583 
};
double const    COS_Fu[37] = { 
1,
0.222458645,
- 0.901024302,
- 0.623339937,
0.623689586,
0.900830218,
- 0.222894647,
- 0.9999999,
- 0.2220226,
0.901218206,
0.622990162,
- 0.624039111,
- 0.900635953,
0.223330603,
0.9999996,
0.22158651,
- 0.90141193,
- 0.622640263,
0.624388511,
0.900441508,
- 0.223766515,
- 0.9999991,
- 0.221150375,
0.901605474,
0.62229024,
- 0.624737787,
- 0.900246883,
0.224202382,
0.9999984,
0.220714196,
- 0.901798838,
- 0.621940092,
0.625086937,
0.900052078,
- 0.224638205,
- 0.9999975
};
double const    SIN_Fu[37] = { 
0,
0.974942127,
0.43376861,
- 0.781950972,
- 0.781672118,
0.434171532,
0.974842539,
- 0.00044723,
- 0.97504152,
- 0.433365601,
0.782229671,
0.781393107,
- 0.434574366,
- 0.974742757,
0.00089446,
0.975140717,
0.432962506,
- 0.782508212,
- 0.78111394,
0.434977115,
0.974642779,
- 0.00134169,
- 0.97523972,
- 0.432559324,
0.782786598,
0.780834616,
- 0.435379776,
- 0.974542606,
0.001788919,
0.975338528,
0.432156056,
- 0.783064826,
- 0.780555137,
0.435782349,
0.974442239,
- 0.002236149 
};

BaliseDemod::BaliseDemod(QObject *parent) : QObject(parent)
{
}

BaliseDemod::~BaliseDemod()
{

}

void BaliseDemod::downSampling(const int downSamplingRate, const QVector<float>& originalSampleData, QVector<float>& afterDownSampling)
{
	afterDownSampling.resize(originalSampleData.size() / downSamplingRate);
	for (int i = 0; i < afterDownSampling.size(); i++)
	{
		afterDownSampling[i] = originalSampleData[i * downSamplingRate];
	}

}

/*************************************************************************************************
* 此函数对采样数据进行希尔伯特变换
* 使用IIR设计的滤波器进行希尔伯特变换，并对得到的相位进行了关于中心频率的修正
* *************************************************************************************************/
 void BaliseDemod::hilbertPhaseTrans(const int downSamplingRate, const QVector<float> &sampleDataVec, QVector<float> &hilbertPhaseDataVec)
 {
	 int multiplication_Zero = 	(hibertFilterOrder - 1)/2;
	 int size_Hilbert_Phase = sampleDataVec.size() - multiplication_Zero;
	 hilbertPhaseDataVec.resize(size_Hilbert_Phase);
	 double	imag = 0;  //希尔伯特变换后得到的解析信号虚部
	 const double f1 = 0.6315603173673;		//希尔伯特变换滤波器的系数
	 const double f3 = 0.1972499708834;
	 const double f5 = 0.1031216463074;
	 for (size_t i = multiplication_Zero; i < size_Hilbert_Phase; i++)
	 {
		 imag = 0.6315603173673 * (sampleDataVec[i - 1] - sampleDataVec[i + 1])
			 + 0.1972499708834 * (sampleDataVec[i - 3] - sampleDataVec[i + 3])
			 + 0.1031216463074 * (sampleDataVec[i - 5] - sampleDataVec[i + 5]);

		 hilbertPhaseDataVec[i - multiplication_Zero] = atan2(imag, sampleDataVec[i]);  //atan2(y,x)
	 }
	 double accumulate = 0;
	 double temp = 0;
	 double testF = TWICE_PI * 4.234 / (sampleRate / downSamplingRate);
	 for (size_t j = 1; j < size_Hilbert_Phase - hibertFilterOrder*2; j++)//Size_Hilbert_Phase-Filter_Order_double
	 {
		 hilbertPhaseDataVec[j] += accumulate;
		 temp = hilbertPhaseDataVec[j] < hilbertPhaseDataVec[j - 1] ? TWICE_PI - testF : -testF;
		 hilbertPhaseDataVec[j] += temp;
		 accumulate += temp;
	 }
 }


 /*************************************************************************************
 * 对收到的采样数据进行解调。
 * ************************************************************************************/
 void BaliseDemod::slo_demod(Balise balise)
 {
	 LARGE_INTEGER freq_;
	 QueryPerformanceFrequency(&freq_);

	 LARGE_INTEGER begin_time;
	 LARGE_INTEGER end_time;
	

	 QueryPerformanceCounter(&begin_time);

	 //首先对采样数据进行三倍将采样
	 QVector<float> sampleDataVec_down3;
	 downSampling(3, BaliseManager::sampleDataVec, sampleDataVec_down3);
	 //对采样数据进行希尔伯特变换得到相位折线
	 QVector<float> phaseDataVec_down3;  //三倍降采样后的相位折线
	 hilbertPhaseTrans(3, sampleDataVec_down3, phaseDataVec_down3);

	 QVector<BYTE> demodedBitVec;   //存储解调后bit
	 demodedBitVec.resize(150000);  //最大有150000个bit

	 size_t itemSize = phaseDataVec_down3.size();

	 int N = 10;      //前后拟合使用的点数
	 double a_r, b_r, a_l, b_l, x0;
	 int pre_position = 0, start_idx = 0, demodedBitNum = 0;
	 double tempSuma = 0, tempSumb = 0, yinzi = 0;

	 for (int i = N; i < (int)itemSize - N; i += 2) //从相位的第20点开始找拐点
	 {
		 //计算右斜率
		 tempSuma = 0, tempSumb = 0, yinzi = 0;
		 for (int s = 1; s <= 10; s++)
		 {
			 tempSuma += (2 * s - (N + 1))*phaseDataVec_down3[i + s];
			 tempSumb += ((2 * N + 1) - 3 * s)*phaseDataVec_down3[i + s];
		 }
		 a_r = tempSuma * 6 / (N*(N*N - 1));
		 b_r = tempSumb * 2 / (N*(N - 1));
		 if (a_r > 0.025940 && a_r < 0.142223 || a_r > -0.1302971 && a_r < -0.0378666) //右斜率满足
		 {
			 //计算左斜率
			 tempSuma = 0; tempSumb = 0;
			 for (int s = -(N -1); s <= 0; s++)
			 {
				 tempSuma += (2 * (s + N) - (N + 1))*phaseDataVec_down3[i + s];
				 tempSumb += ((2 * N + 1) - 3 * (1 - s))*phaseDataVec_down3[i + s];
			 }
			 a_l = tempSuma * 6 / 990;
			 b_l = tempSumb * 2 / 90;
			 if ((a_l > 0.025940 && a_l < 0.142223 || a_l > -0.1302971 && a_l < -0.0378666) && a_r*a_l < 0)//左斜率满足
			 {
				 //计算横轴截距
				 x0 = (b_l - b_r) / (a_r - a_l);
				 if (x0 > -1 && x0 < 1) //截距满足
				 {
					 if (pre_position == 0)
					 {
						 pre_position = i;
						 continue;
					 }
					 if (i - pre_position >= POINTS1BIT_3 - deviationPointOf8bit_3)
					 {
						 int deviationPoint = (i - pre_position) % POINTS1BIT_3;
						 if (deviationPoint >= POINTS1BIT_3 - deviationPointOf8bit_3)
							 deviationPoint = POINTS1BIT_3 - deviationPoint;
						 if (deviationPoint <= deviationPointOf8bit_3) //点数满足要求
						 {						 
							 for (int j = start_idx; j < i - N; j += POINTS1BIT_3)
							 {
								 demodedBitVec[demodedBitNum++] = nonCoherentDemod_3(sampleDataVec_down3.begin() + j + 5);
							 }
							 start_idx = i;
						 }
					 } 
					 pre_position = i;
				 }
			 }
		 }
	 }
			
	 demodedBitVec.resize(demodedBitNum);

	 QueryPerformanceCounter(&end_time);
	 double ns_time = (end_time.QuadPart - begin_time.QuadPart) * 1000000.0 / freq_.QuadPart;
	 qDebug() << "time" << ns_time;

	 //检测解调后数据是否有正确报文
	 if (!BaliseMsgAnalyse::analyse(balise, demodedBitVec)) //没有正确报文
	 {
		 
	 }
	 else
	 {
		 //开始计算MTIE和数据速率
		 BaliseMsgAnalyse::caculateMTIEAndBPS(balise, BaliseManager::sampleDataVec);
	 }
	 balise.isDemodFinished = true;
	 emit sig_demodFinished(balise);

	/* QFile bittxt("bittxt.txt");
	 if (bittxt.open(QIODevice::WriteOnly))
	 {
		 QTextStream out(&bittxt);
		 for (int i = 0; i < demodedBitVec.size(); i++)
		 {
			 if (demodedBitVec[i] == 0)
			 {
				 out << "0 ";
			 }
			 else
			 {
				 out << "1 ";
			 }
		 }
		 bittxt.close();
	 }*/

 }

void BaliseDemod::LinearFittingTrans(QVector<double> &SamplePointInDouble, const QVector<double> &HilbertPhaseData, QVector<BYTE> &demodedBitVec)
 {
	 int FskDemodulateBitNum = 0;
	 size_t	m = 0;
	 size_t	start_idx = 0;   //折线的起点
	 size_t	end_idx = 0;     //折线的终点

	 int		pre_position = 0;
	 int      data_length_1;
	 BYTE	StateOfSync = 0;

	 int		i, j, k, coe;//循环变量 
	 bool	UpDown = 0;  //1:up; 0:down
	 BOOL point_is_right;

	 double	length_line = 0;
	 double a_right, a_left, b_right, b_left;
	 double x0 = 0; //两相位折线交点
	 size_t	num_bit = 0;     //当前折线对应的比特总数
	 size_t	PositionOfSync = 0;
	 int 	RecordInflectionPoint[10] = { 0 };
	 int		TimeOfnonSync = -1;//未同步
								   //定第一条线的折线的方向

	/* QVector<BYTE> demodedBitVec;*/
	 demodedBitVec.resize(150000);

	 size_t Itemr = HilbertPhaseData.size();
	 //先使用前8后8定出大致拐点位置。

	 int N = 36;

	 for (j = 50; j<(int)Itemr - 50; j += 2) //从相位的第20点开始找拐点
	 {
		 /*double tempSuma = 0, tempSumb = 0, yinzi = 0;
		 for (int s = 1; s <= 22; s++)
		 {
		 tempSuma += (2 * s - 23)*HilbertPhaseData[j + s];
		 tempSumb += (45 - 3 * s)*HilbertPhaseData[j + s];
		 }
		 a_right = tempSuma / 1771;
		 b_right = tempSumb / 231;

		 tempSuma = 0; tempSumb = 0;
		 for (int s = -21; s <= 0; s++)
		 {

		 tempSuma += (2 *s + 21)*HilbertPhaseData[j + s];
		 tempSumb += (42 + 3*s)*HilbertPhaseData[j + s];
		 }
		 a_left = tempSuma / 1771;
		 b_left = tempSumb / 231;

		 x0 = (b_left - b_right) / (a_right - a_left);*/
		 //找折线起始点
		 double tempSuma = 0, tempSumb = 0, yinzi = 0;
		 for (int s = 1; s <= N; s++)
		 {
			 tempSuma += (2 * s - (N + 1))*HilbertPhaseData[j + s];
			 tempSumb += ((2 * N + 1) - 3 * s)*HilbertPhaseData[j + s];
		 }
		 a_right = tempSuma * 6 / (N*(N*N - 1));
		 b_right = tempSumb * 2 / (N*(N - 1));

		 tempSuma = 0; tempSumb = 0;
		 for (int s = -(N - 1); s <= 0; s++)
		 {

			 tempSuma += (2 * (s + N) - (N + 1))*HilbertPhaseData[j + s];
			 tempSumb += ((2 * N + 1) - 3 * (1 - s))*HilbertPhaseData[j + s];
		 }
		 a_left = tempSuma * 6 / (N * (N*N - 1));
		 b_left = tempSumb * 2 / (N * (N - 1));

		 x0 = (b_left - b_right) / (a_right - a_left);

		 //判断是否属于正确的拐点
		 //------------------------------------------
		 if ((((x0 > -1) && x0<1 && a_right>0.005764299 && a_right<0.071109585 && a_left>-0.065146517 && a_left < -0.008414551)   //折线拐点
			 || (x0 > -1 && x0<1 && a_left>0.005764299 && a_left<0.071109585 && a_right>-0.065146517 && a_right < -0.008414551))
			 && (pre_position < j - 35))
			 //if((  ((x0>-1) && x0<1 && a_right>0.021484375 && a_right<0.1201171875 && a_left>-0.1201171875  && a_left<-0.021484375  )   //折线拐点
			 //   || (x0>-1  && x0<1 && a_left>0.021484375  && a_left<0.1201171875  && a_right>-0.1201171875 && a_right<-0.021484375 ))
			 //   && (pre_position<j-35))
		 {
			 /*  if (j >= 245600 && j <= 245750)
			 {
			 j = j;
			 }*/

			 data_length_1 = (j - pre_position) % 112;
			 if (data_length_1 >= (int)112 - 28)  data_length_1 -= 112;
			 point_is_right = (data_length_1 >= -14) && (data_length_1 <= 14);

			 pre_position = j;
			 if (!point_is_right) /*||(StateOfSync   == 0)*/  //第一次同步时也可以进入
			 {
				 continue;  //如果不是合适的拐点，舍弃
			 }
		 }
		 else   //无拐点,不执行下面的代码
			 continue;

#pragma region//拟合直线
		 //1==================================================================================================
		 for (i = start_idx; i< j - 50; i += 112)
		 {
			 demodedBitVec[FskDemodulateBitNum++] = nonCoherentDemod(SamplePointInDouble.begin() + i + 5);
		 }
		 start_idx = j;
		 //1==================================================================================================
#pragma endregion
	 }
	 QFile bittxt("bittxt_t.txt");
	 if (bittxt.open(QIODevice::WriteOnly))
	 {
		 QTextStream out(&bittxt);
		 for (int i = 0; i < demodedBitVec.size(); i++)
		 {
			 if (demodedBitVec[i] == 0)
			 {
				 out << "0 ";
			 }
			 else
			 {
				 out << "1 ";
			 }
		 }
		 bittxt.close();
	 }
 }

 BYTE BaliseDemod::nonCoherentDemod(const QVector<double>::iterator InPutDataIter)
 {
	 double SumCosFd = 0;
	 double SumSinFd = 0;
	 double SumCosFu = 0;
	 double SumSinFu = 0;

	 for (size_t i = 0; i < 112; i++, InPutDataIter)
	 {
		 SumCosFd += *(i + InPutDataIter) * COS_Fd[i];
		 SumSinFd += *(i + InPutDataIter) * SIN_Fd[i];
		 SumCosFu += *(i + InPutDataIter) * COS_Fu[i];
		 SumSinFu += *(i + InPutDataIter) * SIN_Fu[i];
	 }
	 return ((BYTE)((SumCosFd * SumCosFd + SumSinFd * SumSinFd) <(SumCosFu * SumCosFu + SumSinFu * SumSinFu)));
 }

 BYTE BaliseDemod::nonCoherentDemod_3(const QVector<float>::iterator InPutDataIter)
 {
	 double SumCosFd = 0;
	 double SumSinFd = 0;
	 double SumCosFu = 0;
	 double SumSinFu = 0;

	 for (size_t i = 0; i < POINTS1BIT_3; i++, InPutDataIter)
	 {
		 SumCosFd += *(i + InPutDataIter) * COS_Fd[i];
		 SumSinFd += *(i + InPutDataIter) * SIN_Fd[i];
		 SumCosFu += *(i + InPutDataIter) * COS_Fu[i];
		 SumSinFu += *(i + InPutDataIter) * SIN_Fu[i];
	 }
	 return ((BYTE)((SumCosFd * SumCosFd + SumSinFd * SumSinFd) <(SumCosFu * SumCosFu + SumSinFu * SumSinFu)));
 }

 void BaliseDemod::slo_caculatePhaseData(int recvNum, QVector<float> sampleDataVec)
 {
     QVector<float> hibertPhaseDataVec;
	 QVector<float> downSamplingVec;
	 downSampling(3, sampleDataVec, downSamplingVec);
     hilbertPhaseTrans(3, downSamplingVec, hibertPhaseDataVec);
     emit sig_hasPhaseData(recvNum, hibertPhaseDataVec);
 }
