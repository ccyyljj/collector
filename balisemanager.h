#ifndef BALISEMANAGER_H
#define BALISEMANAGER_H

#include <QObject>
#include <QQueue>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>
#include <QThread>
#include <QDateTime>
#include <QTemporaryFile>

#include "balisestructdefine.h"
#include "getfpgadata.h"
#include "balisedemod.h"
#include "balisefre.h"
#include "balisemsganalyse.h"



/*此类是应答器管理类
 *
 *
 */
class BaliseManager : public QObject
{
    Q_OBJECT
public:

    explicit BaliseManager(QObject *parent = nullptr);
    ~BaliseManager();

    static Balise mkaBalise(const int num, const QDateTime time);  //根据序号和生成时间生成一个应答器
//----------------------------------------------------------------------------------------------------------------------------

    static QString filePath;    //Balise类在创建时需要使用文件路径


    static Balise get1Blaise(int num);
    static QVector<Balise> baliseVec;  //存储所有未处理应答器信息的数组
	static QVector<float> sampleDataVec;  //
	static QVector<SampleDataGain> gainVec;
private:
     void caculateSampleData(Balise &balise, QByteArray &originalBaliseData, QVector<float> &sampleDataVec, QVector<SampleDataGain> &gainVec); //从getFPGAData线程管理的sampleData_file中获取应答器采样数据
	 void jobTracker(int jobNum);
	 void startJob(int recvNum);
	 void onlyCopyBaliseFre(Balise &des, const Balise &sor);
	 void onlyCopyBaliseDemode(Balise &des, const Balise &sor);

	 
signals:
     void sig_changeFreInfo(FreInfo freInfo);

	 void sig_hasaBalise(const Balise &balise);

     void sig_drawErrorRateWave(const Balise &balise);
     void sig_drawSampleDataWave(const int &recvNum, const QVector<float> &sampleDataVec, const QVector<SampleDataGain> &gainVec);
     void sig_drawFreDataWave(const Balise &balise, const QVector<float> &freVec);

     void sig_getPhaseData(const int &recvNum, const QVector<float> &sampleDataVec);
     void sig_drawPhaseWave(const int &recvNum, const QVector<float> &phaseDataVec);

     void sig_caculateFre(Balise balise);
     void sig_demod(Balise balise);

public slots:
     void slo_fre_finished(const Balise &balise, const QVector<float> &spectrum);
     void slo_demode_finished(const Balise &balise);
     void slo_phase_finished(const int &recvNum, const QVector<float> &phaseDataVec);
     void slo_add2BaliseVec(Balise balise);
     void slo_getHistorySampleData2Draw(int recvNum);
     void slo_getHistoryFreData2Draw(int recvNum);
     void slo_getHistoryErrorRate2Draw(int recvNum);
     void slo_getBalisePhaseData2Draw(int recvNum);
     void slo_changeFreInfo(FreInfo freInfo);
	 void slo_review(QString binFileName);
	
private:
	int jobNum;
	bool isWorkThBusy;
	bool isReview;

	QThread baliseDemodTh;
	QThread baliseFreTh;

	FreCaculator *p_baliseFre;
    //BaliseDemod *p_baliseDemod;
	QTimer *checkTimer;
	QTemporaryFile freFile;
	QTemporaryFile sampleDataFile;
	std::ifstream* binFile;
};

#endif // BALISEMANAGER_H

