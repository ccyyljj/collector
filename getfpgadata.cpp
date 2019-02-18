
#include <QThread>
#include <QDebug>
#include <QDateTime>
#include "getfpgadata.h"
#include "pcaploop.h"


//静态成员初始化
QMutex GetFPGAData::mutex12;
bool GetFPGAData::isInResendTime;
unsigned char GetFPGAData::data_Buffer[PACKAGE_SIZE * MAX_PACKAGE_COUNT];
unsigned char GetFPGAData::sendHead[42];
qint16 GetFPGAData::receivedPackageCount;
QVector<quint16> GetFPGAData::lostPackageRecordVec;
std::fstream* GetFPGAData::sampleData_file;
bool GetFPGAData::isConnected;
pcap_t* GetFPGAData::adhandle;
PcapLoop* GetFPGAData::pcapLoop;

GetFPGAData::GetFPGAData(QObject *parent) : QObject(parent)
{

}

GetFPGAData::~GetFPGAData()
{

}


/************************************************************************
此函数只能通过其他线程通过信号与槽的方式调用

************************************************************************/
void GetFPGAData::slo_init()
{
	QString initInfo;
	//初始化各变量
    isInResendTime = false;

	dog = new QTimer(this);
    connect(dog, &QTimer::timeout, this, &GetFPGAData::slo_dogEat);
    dog->start(300);

	sampleData_file = new std::fstream("sampleData.bin", std::ios::out | std::ios::in | std::ios::binary | std::ios::trunc);
	if (!sampleData_file->is_open())
	{
		initInfo = "打开存储文件sampleData.bin失败!";
		emit sig_error(initInfo); //初始化失败
	}

    workTh = new QThread(this);
	pcapLoop = new PcapLoop(NULL);
	pcapLoop->moveToThread(workTh);
    connect(workTh, &QThread::finished, pcapLoop, &PcapLoop::deleteLater);
    connect(this, &GetFPGAData::sig_startPcapLoop, pcapLoop, &PcapLoop::slo_startPcapLoop);
    connect(pcapLoop, &PcapLoop::sig_hasaBalise, this, &GetFPGAData::slo_hasaBalie);
    connect(pcapLoop, &PcapLoop::sig_receiveaBaliseOver, this, &GetFPGAData::slo_receiveaBaliseOver);
    connect(pcapLoop, &PcapLoop::sig_hasCommandRespons, this, &GetFPGAData::slo_getRespondFromFPGA);
	//初始化Pcap

	if (initPcap(initInfo) != 0) 
	{
		emit sig_error(initInfo); //初始化失败
	}
	else
	{
        emit sig_sucess(initInfo);
	}

	//开启pcapLoop
	workTh->start();
    emit sig_startPcapLoop();
}

int GetFPGAData::initPcap(QString &initInfo)
{
	char errbuf[PCAP_ERRBUF_SIZE];
    u_int netmask;
    struct bpf_program fcode;
    char packet_filter[100] = "( ip and udp ) and ( src host 172.168.0.6 and dst host 172.168.0.5 )";
	pcap_if_t *alldevs;
	pcap_if_t *d;

    /* Retrieve the device list */
    if (pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        QString abort("Error in pcap_findalldevs: ");
        QString errInfo(errbuf);
		initInfo = abort + errInfo;
        return -1;

    }
    /* Print the list */
    bool findFlag = false;;
    for (d = alldevs; d; d = d->next)
    {
        /* Open the adapter */
        if ((adhandle = pcap_open_live(d->name,	// name of the device
            65535,			// portion of the packet to capture.
                            // 65536 grants that the whole packet will be captured on all the MACs.
            1,				// promiscuous mode (nonzero means promiscuous)
            1,			// read timeout
            errbuf			// error buffer
        )) != NULL)
        {
            if(pcap_datalink(adhandle) != DLT_EN10MB)
            {
                pcap_close(adhandle);      //如果不是以太网设备，则关闭打开的适配器
                continue;
            }
            else
            {
                findFlag = true;
                break;
            }
        }
    }
    if (!findFlag)
    {
		initInfo = "打开网络设备失败，请确认安装了WinPcap!";
        return -1;
    }

    /*********************************************配置发送**************************************************
    /* Open the output device */
    if ((send_fp = pcap_open(d->name,            // name of the device
        100,                // portion of the packet to capture (only the first 100 bytes)
        PCAP_OPENFLAG_PROMISCUOUS,  // promiscuous mode
        1000,               // read timeout
        NULL,               // authentication on the remote machine
        errbuf              // error buffer
    )) == NULL)
    {
		initInfo = "Unable to open the adapter. port is not supported by WinPcap";
        return -1;
    }

    pcap_setbuff(adhandle, 8 * 1024 * 1024); //内存可换为8m
    pcap_setuserbuffer(adhandle, 8 * 1024 * 1024);

    if (d->addresses != NULL)
        /* Retrieve the mask of the first address of the interface */
        netmask = ((struct sockaddr_in *)(d->addresses->netmask))->sin_addr.S_un.S_addr;
    else
        /* If the interface is without addresses we suppose to be in a C class network */
        netmask = 0xffffff;


    //compile the filter
    if (pcap_compile(adhandle, &fcode, packet_filter, 1, netmask) < 0)
    {
		initInfo = "Unable to compile the packet filter. Check the syntax.";
        return -1;
    }

    //set the filter
    if (pcap_setfilter(adhandle, &fcode) < 0)
    {
		initInfo = "Error setting the filter.";
	    return -1;
    }
    initInfo = QString(d->description);
    /* At this point, we don't need any more the device list. Free it */
    pcap_freealldevs(alldevs);
    return 0;
}

/*************************************************************************************
结束PcapLoop的槽函数,因为pcapLoop进入死循环，不能响应任何信号，所以
只能通过外部线程将pcapLoop结束
*************************************************************************************/
void GetFPGAData::slo_pcapEndloop()
{
    pcap_breakloop(adhandle);
    pcap_close(adhandle);
    qDebug() << "pacpLOOPEnd";
    workTh->quit();
    workTh->wait();
}



/**************************************************************************************
检查丢包信息，如果未丢包，则写入应答器数据，如有丢包，发送重传命令

**************************************************************************************/
void GetFPGAData::slo_checkLostPackageRecordVec()
{
    if(mutex12.tryLock())
    {
        QMutexLocker locker(&mutex12);
    }

	if (lostPackageRecordVec.isEmpty())  //没有丢包，直接写入应答器数据
	{
		writeaBaliseData();

        emit sig_hasaBalise(balise);  //向baliseManager发送应答器接收到信号
        qDebug() << "get:" << balise.recvNum;
	}
	else
	{
		//发送重传指令
        for(int i = 0; i < lostPackageRecordVec.size(); i++)
        {
            lostPackageRecordVec[i] = htons(lostPackageRecordVec[i]);
        }

        sendData(ID_RESEND, (unsigned char*)lostPackageRecordVec.data(), lostPackageRecordVec.size()*2);
	}
}

/**************************************************************************************
 * 允许重传时间结束，不管重传是否完成，均强制写入
 *
 *
 * ************************************************************************************/
void GetFPGAData::slo_resendTimeOver()
{
    QMutexLocker locker(&mutex12);
    isInResendTime = false;    //重传允许时间间隔结束
	if (balise.dataSize != 0) //已经写入过文件了
	{
		return;
	}
	else //还未写入文件，强制写入
	{
		writeaBaliseData();
        emit sig_hasaBalise(balise);
        qDebug()<<"收到"<<balise.recvNum;
	}
}

/**************************************************************************************
 * 响应PcapLoop线程收到一个应答器信号的槽函数，在此函数中记录应答器接收时间和序号,并重置应答器信息
 *
 * ************************************************************************************/
void GetFPGAData::slo_hasaBalie(QDateTime recvTime)
{
    QMutexLocker locker(&mutex12);
    static int i = 0;
    isInResendTime = false;

    balise = BaliseManager::mkaBalise(i++, recvTime);

    receivedPackageCount = 1; //此时已经收到第一个数据包
    lostPackageRecordVec.clear();
}


void GetFPGAData::slo_receiveaBaliseOver()
{
    QMutexLocker locker(&mutex12);
    isInResendTime = true;     //重传时间开始
    QTimer::singleShot(800, this, SLOT(slo_resendTimeOver()));
    slo_checkLostPackageRecordVec();
}

/**************************************************************************************
每隔一段时间去检查连接状态，如果连接断开，则强制写入应答器数据，并向主界面发送连接断开提示

*************************************************************************************/
void GetFPGAData::slo_dogEat()
{
    QMutexLocker locker(&mutex12);
    if(isConnected)
    {
        emit sig_showConnectState(1);
        isConnected = false;
        return;
    }
    else
    {
        if(balise.dataSize == 0 && receivedPackageCount != 0) //还有未写入的应答器数据
        {
            writeaBaliseData();  //强制写入文件
            emit sig_hasaBalise(balise);
            qDebug()<<"收到"<<balise.recvNum;
        }
        emit sig_showConnectState(0);
    }
}

/**************************************************************************************
 * 向FPGA发送采样长度和采样门限的槽函数
 *
 *
 * ************************************************************************************/
void GetFPGAData::slo_sendData2FPGA(unsigned char length, unsigned char gate)
{
    unsigned char buffer[2];
    buffer[1] = length;
    buffer[0] = gate;
    sendData(ID_COMMAND, buffer, 2);
}

/**************************************************************************************
 * 收到pcapLoop线程指令回复的槽函数
 * 在此函数里发送信号到主线程,表明收到写指令回复
 *
 * ***********************************************************************************/
void GetFPGAData::slo_getRespondFromFPGA()
{
    emit sig_hasCommandRespons();
}


//由于ARP协议中，FPGA负责查询局域网内ip，所以需要FPGA给PC发数据包后，PC才能获得FPGA mac地址
//发送MAC包标识，指令包标识65534， 数据包标识65535
//通信过程：1.FPGA首先发送包标识为65533，数据区结尾为0xFF, 0xFF的数据包，目的是让PC获取自己的MAC地址
//        2.PC收到后，发送包标识为65534，数据区为1字节门限，1字节长度，结尾为0xFF, 0xFF的数据包
//        3.FPGA收到后，回复包标识为65534，数据区结尾为0xFF, 0xFF的数据包，表示参数修改成功
//        4.FPGA开始发送数据，包标识变化0-16383
//        5.发送完成后PC统计丢包，回复包标识为65535，



/*参数说明： type:要发送数据
 *
 *
 */
bool GetFPGAData::sendData(const quint16 type, const unsigned char *p_data, int length)
{
    quint16 id = htons(type);
    sendHead[18] = (unsigned char)id;
    sendHead[19] = (unsigned char)(id >> 8);
    unsigned char sendBuf[1070];
    memset(sendBuf, 0xff, 1070);
    memcpy(sendBuf, sendHead, 42);
    memset(&sendBuf[1066], 0x00, 4); //4位CRC校验设为0

    //如果长度大于1024则需要分包发送
    int sum = 0;
    int sendPos = 0;
    while(1)
    {
        sum += 1024;
        if(sum <= length) //发送1024个数据并使发送位置加1024
        {
            memcpy(&sendBuf[42], p_data + sendPos, 1024);
            //发送
            if (pcap_sendpacket(send_fp, sendBuf, 1070) != 0)
            {
                qDebug() << "send error";
                return false;
            }
            sendPos += 1024;
        }
        else
        {
            memcpy(&sendBuf[42], p_data + sendPos, length - sendPos);
            if (pcap_sendpacket(send_fp, sendBuf, 1070) != 0)
            {
                qDebug() << "send error";
                return false;
            }
            break;
        }
    }

    return true;
}

/*********************************************************************************************
将dtatbuffer中应答器数据写入到文件,因为balisemanager中数据处理速度比接受的慢，所以利用sampleData_file作文件
缓存
**********************************************************************************************/
int GetFPGAData::writeaBaliseData()
{
	if (!sampleData_file->bad())
	{
        balise.dataPos = sampleData_file->tellp();
		balise.dataPos += 4;
		balise.dataSize = receivedPackageCount * (PACKAGE_SIZE - 2);
		balise.lostPackageRecordVec = lostPackageRecordVec;
		if (sampleData_file->good())
		{
			sampleData_file->write((char*)&balise.dataSize, 4);   //先写入本次数据长度        
            sampleData_file->write((char*)data_Buffer, balise.dataSize); //再写入本次数据
		}
		return 0;
	}
	else
	{
		emit sig_error("写入应答器采样数据失败!");
		return -1;
	}
}

