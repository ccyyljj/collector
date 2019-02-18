#ifndef BALISEFRE_H
#define BALISEFRE_H

#include <QObject>
#include <balisestructdefine.h>
#include <windows.h>
#pragma comment(lib, "C:\\fftw\\lib\\libfftw3-3.lib")
#pragma comment(lib, "C:\\fftw\\lib\\libfftw3f-3.lib")
#pragma comment(lib, "C:\\fftw\\lib\\libfftw3l-3.lib")


#define R_NUMBER 335544//8389*500   // 频谱计算的缓冲数组长度
class FreCaculator : public QObject
{
    Q_OBJECT
public:
    explicit FreCaculator(QObject *parent = nullptr);
private:
    void getBaliseCF(const QVector<double>&spectrumData, double DELT, double& FH, double& FL, double& FC);
private:
    
    //QVector<double> sampleDataVec;
    double sampleRate;          //fsk信号的采样率
    size_t cutOffFre;
    double resolution;
    size_t fillterRank;         //burg算法滤波器阶数
    size_t specDataSize;        //burg算法得出的频谱数据长度
	QVector<QVector<double>> ejia;						//double	ejia[2][R_NUMBER];	// 频谱计算的中间数组
	QVector<QVector<double>> ejian;						//double	ejian[2][R_NUMBER];	// 频谱计算的中间数组
	QVector<double> sampleDataVec;
    
signals:
    void sig_caculateFreFinished(const Balise &balise, const QVector<float> &averSpec);

public slots:
	void slo_caculateFre(Balise balise);
   /* void slo_caculateAverFre(Balise balise);

    void slo_caculateFre(int s, size_t sampleDataLen, QVector<double>& spectrum);*/

    void slo_changeFreInfo(FreInfo freInfo);
};

#endif // BALISEFRE_H
