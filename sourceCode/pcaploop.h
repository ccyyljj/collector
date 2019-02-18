#pragma once

#include <QObject>
#include <pcap.h>
#include <qlist.h>
#include <QDateTime>
#include "balisestructdefine.h"



class PcapLoop : public QObject
{
	Q_OBJECT

public:
	PcapLoop(QObject *parent);
	~PcapLoop();
     void my_pcap_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
	 static void pack_fun(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);
private:
        int now_id;
        int last_id;
	bool isData = false;   //�Ƿ����ڴ�������
    unsigned char sendHead_temp[42];  //��ȡ������̫��֡ͷ
	PackageType now_package_type;
	PackageType last_package_type;
signals:
        void sig_checkLostPackageRecordVec();
        void sig_hasaBalise(QDateTime recvTime);
        void sig_receiveaBaliseOver();
        void sig_hasCommandRespons();

public slots:
        void slo_startPcapLoop();
};
