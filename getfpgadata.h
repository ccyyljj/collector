/********************************************************************************************
说明：此类用来
              初始化pcap（配置捕获和发送）；
			  开启pcap捕获并处理pcap捕获的包；
			  发送数据；
			  管理连接状态；
*********************************************************************************************/
#ifndef GETFPGADATA_H
#define GETFPGADATA_H


#include <QObject>

#include <fstream>
#include <string>
#include <unordered_set>
#include <vector>
#include <deque>
#include <chrono>
#include <mutex>
#include <ctime>
#include <chrono>
#include <fstream>

#include <pcap.h>
#include <winsock2.h>

#include <QMutex>
#include <QMutexLocker>
#include <QMutexLocker>
#include <QDebug>
#include <QtAlgorithms>
#include <QThread>
#include <QList>
#include <QDateTime>
#include "balisestructdefine.h"
#include "balisemanager.h"
#include "mainwindow.h"


class PcapLoop;

class GetFPGAData : public QObject
{
    Q_OBJECT
public:
    static QMutex mutex12;
    static bool isInResendTime;
	static unsigned char data_Buffer[PACKAGE_SIZE * MAX_PACKAGE_COUNT];
    static unsigned char sendHead[42];  //用于发送数据的以太网帧头
    static qint16 receivedPackageCount;
	static QVector<quint16> lostPackageRecordVec;
	static std::fstream *sampleData_file;
    static bool isConnected;
    static pcap_t *adhandle;
    static PcapLoop *pcapLoop;
private:
    
    bool isrite;           //数据包是否由于超时被强制写入标志。



    pcap_t *send_fp;

	QThread *workTh;

	QTimer *dog;
    Balise balise;

public:
	explicit GetFPGAData(QObject *parent = nullptr);
	~GetFPGAData();

private:
	int initPcap(QString &initInfo); //初始化pcap（配置捕获和发送）
    bool sendData(const quint16 type, const unsigned char* p_data, int length);//此函数用来向FPGA发送数据
	int writeaBaliseData();

signals:
	void sig_error(QString errorInfo);
	void sig_sucess(QString sucessInfo);
    void sig_startPcapLoop();
	void sig_hasaBalise(Balise balise);
    void sig_showConnectState(int connectedState);
    void sig_hasCommandRespons();

public slots:

    void slo_init(); //初始化各部分

    void slo_pcapEndloop();
	void slo_checkLostPackageRecordVec();
    void slo_resendTimeOver();
    void slo_hasaBalie(QDateTime recvTime);
    void slo_receiveaBaliseOver();
	void slo_dogEat();
    void slo_sendData2FPGA(unsigned char length, unsigned char gate);
    void slo_getRespondFromFPGA();
};

#endif // GETFPGADATA_H
