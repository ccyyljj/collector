#ifndef BALISESTRUCTDEFINE_H
#define BALISESTRUCTDEFINE_H

#include <QString>
#include <QFile>
#include <QDateTime>
#include <QVector>
#include "Winsock2.h"
#include "windows.h"

#define ID_RESEND 65532
#define ID_COMMAND 65534
#define ID_HEART_BEAT 65533

#define PACKAGE_SIZE 1026         //两字节包头和1024字节数据
#define MAX_PACKAGE_COUNT 16384   //一个应答器最多有16384包
#define PI 3.1415926
#define TWICE_PI 6.2831852

#define POINTS1BIT_3 37
#define POINTS1BIT_1 112 

typedef struct
{
    int position;					//增益生效的起始位置
    int gain;						//增益
}SampleDataGain;					//采样数据增益

typedef struct
{
    QString					baliseID = "";
    //应答器接收时的基本信息
    int						recvNum = -1;
    QDateTime				recvTime;
    QVector<quint16>		lostPackageRecordVec;

    qint64					dataPos = 0;
    quint32					dataSize = 0;

	//
    qint64                  sampleDataPos = -1;      //采样数据在缓存文件中的位置
    qint64                  freDataPos = -1;         //频谱数据在缓存文件中的位置

    //screenData 除去解调结果和频谱和原始数据，频幕上需要显示的其余部分
    double					FL[3];					//低频频点,第一个为瞬时最大值，第二个为瞬时最小值，第三个为平均值
    double				    FH[3];					//高频频点,第一个为瞬时最大值，第二个为瞬时最小值，第三个为平均值
    double					FC[3];					//中频值,第一个为瞬时最大值，第二个为瞬时最小值，第三个为平均值
    double					resolution;
	bool                    isFreFinished;          //频谱是否计算完成

	//
    QVector<BYTE>			demodedByteVec;			//解调后的bit流,合并为BYTE形式存储
	BYTE					msgData[128];			//1023编码报文
	BYTE					userData[104];			//830译码报文
	QVector<double>			errorRateVec;			//解调后报文的误码率
	int						firstCorrectBitPos;     //第一个能解出应答器报文的正确比特
	int						totalCorrectBitNum;		// 所有的正确比特个数
	int						maxRightBitNum;			// 最大连续正确比特个数
	int						correctMSGNum;			// 正确的应答器报文个数
	QVector<double>			MTIEVec_1;
	QVector<double>			MTIEVec_2;
	double                  bps;                    //实际波特率
	double                  minTransSpeed;          //最小数据速率
	double                  maxTransSpeed;          //最大数据速率
	double                  averTranSpeed;          //实际数据速率 
	bool                    isDemodFinished;         //解调是否完成
}Balise;

//全局配置参数结构体定义
typedef struct
{
    double fre_low;
    double fre_high;
    int sampleRate;
    int fre_Resolution; //频谱分辨率
}ConfigParameter;

enum PackageType
{
    HeartBeat,
    Data,
    RetransData
};

/* 4 bytes IP address */
typedef struct ip_address
{
    u_char byte1;
    u_char byte2;
    u_char byte3;
    u_char byte4;
}ip_address;

/* IPv4 header */
typedef struct ip_header
{
    u_char	ver_ihl;		// Version (4 bits) + Internet header length (4 bits)
    u_char	tos;			// Type of service
    u_short tlen;			// Total length
    u_short identification; // Identification
    u_short flags_fo;		// Flags (3 bits) + Fragment offset (13 bits)
    u_char	ttl;			// Time to live
    u_char	proto;			// Protocol
    u_short crc;			// Header checksum
    ip_address	saddr;		// Source address
    ip_address	daddr;		// Destination address
    u_int	op_pad;			// Option + Padding
}ip_header;


/* UDP header*/
typedef struct udp_header
{
    u_short sport;			// Source port
    u_short dport;			// Destination port
    u_short len;			// Datagram length
    u_short crc;			// Checksum
}udp_header;

typedef struct FreInfo
{
    double sampleRate;          //fsk信号的采样率
    size_t cutOffFre;           //频谱分析的最高截至频率
    double resolution;          //频谱分辨率
    size_t fillterRank;         //burg算法滤波器阶数
}FreInfo;


//typedef void (GetFPGAData::*MFP)(u_char *, const struct pcap_pkthdr *, const u_char *);

#endif // BALISESTRUCTDEFINE_H
