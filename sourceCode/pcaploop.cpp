#include "pcaploop.h"
#include "getfpgadata.h"
#include <pcap.h>

PcapLoop::PcapLoop(QObject *parent)
    : QObject(parent)
{
    now_package_type = HeartBeat;
    last_package_type = HeartBeat;
    last_id = -1;
}

PcapLoop::~PcapLoop()
{

}

void PcapLoop::slo_startPcapLoop()
{
    pcap_loop(GetFPGAData::adhandle, -1, (pcap_handler)pack_fun, NULL);
}

/*                                ?????????
*
* ???MAC???  ?MAC???  ????                ????                        crc
*     6          6       2                46~1500                      4
*                           \                                        /
*
*        IP?????              ?汾      ???????     ????????      ?????
*         20                  4         4          8            16
*                             ???                  ???         ???
*                             16                   3            13
*                             ???????   Э??        ???У????
*                             8         8          16
*                             ?IP???
*                             32
*                             ???IP???
*                             32
*                                               ????
*                             \                                      /
*
*
*         UDP?????             ?????               ??????
*           8                  16                    16
*                              UDP????                UDPУ????
*                              16                    16
*                                          ????
*
*/
void PcapLoop::my_pcap_handler(u_char * param, const pcap_pkthdr * header, const u_char * pkt_data)
{
    if (header->caplen != 42 + PACKAGE_SIZE)
    {
        return;
    }

    //?????????
    now_id = ntohs(*((quint16*)(pkt_data + 18)));
    if(now_id == ID_COMMAND)
    {
        emit sig_hasCommandRespons();
        return;
    }

    if (now_id == ID_HEART_BEAT)  //??????????
    {
        QMutexLocker locker(&GetFPGAData::mutex12);
        GetFPGAData::isConnected = true; //ι??
        now_package_type = HeartBeat;
        //?????ж?mac???????????仯
        if (memcmp(GetFPGAData::sendHead, pkt_data + 6, 6) != 0 || memcmp(GetFPGAData::sendHead + 6, pkt_data, 6) != 0)
        {  //???仯?????·????
            memcpy(GetFPGAData::sendHead, pkt_data, 42);
            //??????????????
            //mac???????
            memcpy(GetFPGAData::sendHead, pkt_data + 6, 6);
            memcpy(GetFPGAData::sendHead + 6, pkt_data, 6);

            //16λ???У???????0
            memset(GetFPGAData::sendHead + 24, 0, 2);
            //ip???????
            memcpy(GetFPGAData::sendHead + 30, pkt_data + 26, 4);
            memcpy(GetFPGAData::sendHead + 26, pkt_data + 30, 4);
            //???????
            memcpy(GetFPGAData::sendHead + 36, pkt_data + 34, 2);
            memcpy(GetFPGAData::sendHead + 34, pkt_data + 36, 2);
            //????udp?????1024+8
            GetFPGAData::sendHead[38] = 0x04;
            GetFPGAData::sendHead[39] = 0x08;
            //????udpУ???0
            GetFPGAData::sendHead[40] = 0;
            GetFPGAData::sendHead[41] = 0;
        }

        if (last_package_type == Data) //???????????????????????????????????????(?????????????????? ?? ??????????????????????
        {

            if (GetFPGAData::isInResendTime) //????????????
            {
                //??GetFPGAData?????????????ж???????
                emit sig_checkLostPackageRecordVec();
            }
            else
            {
                last_id = -1;
                emit sig_receiveaBaliseOver();
            }
        }
    }
    if (now_id >= 0 && now_id <= MAX_PACKAGE_COUNT) //????????????
    {
        now_package_type = Data;
        bool canWrite = false;
        QMutexLocker locker(&GetFPGAData::mutex12);
        GetFPGAData::isConnected = true; //ι??
        if (GetFPGAData::isInResendTime) //???????????????????
        {
            //???????????
            for (int i = 0; i < GetFPGAData::lostPackageRecordVec.size(); i++)
            {
                if (now_id == GetFPGAData::lostPackageRecordVec[i])
                {
                    canWrite = true;
                    GetFPGAData::lostPackageRecordVec.removeAt(i);
                }
            }
            if(!canWrite)
            {
                qDebug() << "wrong id??" << now_id;
                qDebug() << "lostid" << GetFPGAData::lostPackageRecordVec;
            }
        }
        else                             //????????????????????????
        {
            canWrite = true;
            //???????????????????????
            GetFPGAData::receivedPackageCount = now_id + 1;
            //?ж??????????,??????????
            if (now_id - last_id != 1)
            {
                for (int i = now_id + 1; i < last_id; i++)
                {
                    GetFPGAData::lostPackageRecordVec.push_back(i);
                }
            }
            if (last_package_type == HeartBeat) //???????????????????????????????????
            {
                //TODO??????
                 QDateTime recvTime = QDateTime::currentDateTime();
                 emit sig_hasaBalise(recvTime);
            }
        }
        if (canWrite)
        {
            //???д?????????
            memcpy(GetFPGAData::data_Buffer + now_id * (PACKAGE_SIZE - 2), pkt_data + 44, PACKAGE_SIZE - 2);
        }
        last_id = now_id;
    }

    last_package_type = now_package_type;

}

void PcapLoop::pack_fun(u_char * param, const pcap_pkthdr * header, const u_char * pkt_data)
{
    GetFPGAData::pcapLoop->my_pcap_handler(param, header, pkt_data);
}


