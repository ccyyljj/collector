#ifndef BALISEDEMOD_H
#define BALISEDEMOD_H

#include <QObject>
#include <qvector.h>
#include <balisestructdefine.h>
#include "balisemanager.h"

class BaliseDemod : public QObject
{
    Q_OBJECT
public:
    explicit BaliseDemod(QObject *parent = nullptr);
    ~BaliseDemod();
	
	static void downSampling(const int downSamplingRate, const QVector<float>& originalSampleData, QVector<float>& afterDownSampling);
	static void hilbertPhaseTrans(const int downSamplingRate, const QVector<float> &sampleDataVec, QVector<float> &hilbertPhaseDataVec);
	
	static BYTE nonCoherentDemod(const QVector<double>::iterator InPutDataIter);
	static BYTE nonCoherentDemod_3(const QVector<float>::iterator InPutDataIter);
	static void LinearFittingTrans(QVector<double> &SamplePointInDouble, const QVector<double> &HilbertPhaseData, QVector<BYTE> &demodedBitVec);
	
private:
	static const int hibertFilterOrder = 11;  //ϣ�������˲�������
	constexpr static const double sampleRate = 63.221;      //����Ƶ��(M)
	constexpr static const double baudRate = 564.48;        //��׼���ݴ�������(K)
	static const int pointsOf1bit = 112;       //һ��bit����Ӧ�Ĳ�������
	static const int deviationPointOf8bit = 28;     //8������0����1������ݵ���ƫ��
	static const int deviationPointOf8bit_3 = 14;
	
signals:
	void sig_demodFinished(const Balise &balise);
    void sig_hasPhaseData(const int &recvNum, const QVector<float> &phaseDataVec);
public slots :
	void slo_demod(Balise balise);
	void slo_caculatePhaseData(int recvNum, QVector<float> sampleDataVec);
};

#endif // BALISEDEMOD_H
