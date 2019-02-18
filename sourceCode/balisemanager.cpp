#include "balisemanager.h"
#include <fstream>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QDataStream>
#include <QTextStream>

QReadWriteLock sampleFilelock;

QString BaliseManager::filePath = "";

QVector<float> BaliseManager::sampleDataVec = QVector<float>();
QVector<Balise> BaliseManager::baliseVec = QVector<Balise>();
QVector<SampleDataGain> BaliseManager::gainVec = QVector<SampleDataGain>();

QDataStream& operator<<(QDataStream& out, const SampleDataGain& outStruct)
{
	out << outStruct.gain;
	out << outStruct.position;
	return out;
}

QDataStream& operator>>(QDataStream& in, SampleDataGain& inStruct)
{
	in >> inStruct.gain;
	in >> inStruct.position; 
	return in;
}

QDataStream& operator<<(const QVector<SampleDataGain>& outVec, QDataStream& out)
{
    uint32_t size = outVec.size();
    out << size;
    for(int i = 0; i < outVec.size(); i++)
    {
        out << outVec[i];
    }
    return out;
}

QDataStream& operator>>(QDataStream& in, QVector<SampleDataGain>& inVec)
{
    uint32_t size;
    in >> size;
    inVec.resize(size);
    for(int i = 0; i < inVec.size(); i++)
    {
        in>>inVec[i];
    }
    return in;
}


QDataStream& operator<<(const QVector<double>& outVec, QDataStream& out)
{
    uint32_t size = outVec.size();
    out << size;
    for(int i = 0; i < outVec.size(); i++)
    {
        out<<outVec[i];
    }
    return out;
}

QDataStream& operator>>(QDataStream& in, QVector<double>& inVec)
{
    uint32_t size;
    in >> size;
    inVec.resize(size);
    for(int i = 0; i < inVec.size(); i++)
    {
        in>>inVec[i];
    }
    return in;
}

QDataStream& operator<<(const QVector<float>& outVec, QDataStream& out)
{
	uint32_t size = outVec.size();
	out << size;
	for (int i = 0; i < outVec.size(); i++)
	{
		out << outVec[i];
	}
	return out;
}

QDataStream& operator>>(QDataStream& in, QVector<float>& inVec)
{
	uint32_t size;
	in >> size;
	inVec.resize(size);
	for (int i = 0; i < inVec.size(); i++)
	{
		in >> inVec[i];
	}
	return in;
}


BaliseManager::BaliseManager(QObject *parent) : QObject(parent)
{
	jobNum = 0;
	isWorkThBusy = false;  
	isReview = false;
	binFile = NULL;

    //创建并开启解调线程
    BaliseDemod  *p_baliseDemod = new BaliseDemod(NULL);
    p_baliseDemod->moveToThread(&baliseDemodTh);
        //TODO信号与曹连接
    connect(&baliseDemodTh, &QThread::finished, p_baliseDemod, &QObject::deleteLater);
    connect(this, &BaliseManager::sig_demod, p_baliseDemod, &BaliseDemod::slo_demod);
	connect(p_baliseDemod, &BaliseDemod::sig_demodFinished, this, &BaliseManager::slo_demode_finished);
    connect(this, &BaliseManager::sig_getPhaseData, p_baliseDemod, &BaliseDemod::slo_caculatePhaseData);
    connect(p_baliseDemod, &BaliseDemod::sig_hasPhaseData, this, &BaliseManager::slo_phase_finished);
    baliseDemodTh.start();

	

    //创建并开启频谱分析线程
    p_baliseFre = new FreCaculator(NULL);
    p_baliseFre->moveToThread(&baliseFreTh);
        //TODO信号与曹连接
    connect(&baliseFreTh, &QThread::finished, p_baliseFre, &QObject::deleteLater);
    connect(this, &BaliseManager::sig_changeFreInfo, p_baliseFre, &FreCaculator::slo_changeFreInfo);
    connect(this, &BaliseManager::sig_caculateFre, p_baliseFre, &FreCaculator::slo_caculateFre);
    connect(p_baliseFre, &FreCaculator::sig_caculateFreFinished, this, &BaliseManager::slo_fre_finished);

    baliseFreTh.start();

    //打开临时存储文件
    if(!freFile.open() || !sampleDataFile.open()) //打开失败
    {

    }

}

BaliseManager::~BaliseManager()
{
    baliseDemodTh.quit();
    baliseDemodTh.wait();

    baliseFreTh.quit();
    baliseFreTh.wait();
}


Balise BaliseManager::mkaBalise(const int num, const QDateTime time)
{
    Balise balise;

    balise.dataPos = 0;
    balise.dataSize = 0;

    balise.recvNum = num;
    balise.recvTime = time;

	balise.baliseID = "";

	balise.isFreFinished = false;
	balise.isDemodFinished = false;

    return balise;
}

/*******************************************************************************************
 * 使用balise中dataPos和dataSize从sampleData_file中读取ADC编码后数据，并将编码后书数据转化为原始电压值，
 * 并提取增益。
 *
 * *****************************************************************************************/

void BaliseManager::caculateSampleData(Balise &balise, QByteArray &originalBaliseData, QVector<float> &sampleDataVec, QVector<SampleDataGain> &gainVec)
{
    //将ADC量化编码后值转化为原始值
    sampleDataVec.resize(originalBaliseData.size()/2);
    int16_t *p_16 = (int16_t*)originalBaliseData.data();
    double sampleValue;
    int16_t temp = 0;
    uint16_t b = 0;
    uint16_t a = 0;
    bool isSync = false;
    int count = 0;
    SampleDataGain sampleDataGain;

    for(int i = 0; i < sampleDataVec.size(); i++)
    {
        b = ((*p_16) >> 14)&0x0003;
        a <<= 2;
        a |= b;
        if(isSync)
        {
            ++count;
            if(count == 4)
            {
                sampleDataGain.position = i - 7;
                sampleDataGain.gain = a;
                if(gainVec.isEmpty())
                {
                    gainVec.push_back(sampleDataGain);
                }
                else
                {
                    if(a != gainVec.last().gain) //增益发生变化
                    {
                        gainVec.push_back(sampleDataGain);
                    }
                }
                a = 0;
                isSync = false;
            }
        }

        if(a == 0x00ff)//已同步
        {
            isSync = true;
            count = 0;
            a = 0;
            //取下面四个采样值前两位组成增益
        }

        temp = (*p_16) << 2;
        temp = temp < 0?(*p_16++|0xc000):(*p_16++&0x3fff);
        sampleValue = temp/8192.0;
        sampleDataVec[i] = sampleValue;
    }
    //写入缓存文件
    QDataStream sampleDataStream(&sampleDataFile);
    balise.sampleDataPos = sampleDataFile.pos();
    sampleDataStream << sampleDataVec << gainVec;
}



Balise BaliseManager::get1Blaise(int num)
{
    Balise balise;
   /* baliseVecMutex.lock();
    balise = baliseVec[num];
    baliseVecMutex.unlock();*/
    return balise;
}

/*******************************************************************************
 * 应答器频谱计算完成后的槽函数，此函数对主动向主线程发送频谱数据以及存储数据到本线程
 *
 *
 * ****************************************************************************/
void BaliseManager::slo_fre_finished(const Balise &balise, const QVector<float> &freVec)
{
   //向主界面发送频谱计算完成信号
   emit sig_drawFreDataWave(balise, freVec);

   //存储频谱数据到缓存文件
   QDataStream ds(&freFile);
   baliseVec[balise.recvNum].freDataPos = freFile.pos();
   ds << freVec;

   onlyCopyBaliseFre(baliseVec[balise.recvNum], balise);
   jobTracker(balise.recvNum);
}

/***************************************************************************
 * 响应应答器数据分析完成后的槽函数，由于解调后数据量较小，所以不在设临时文件进行存储
 *
 * **************************************************************************/
void BaliseManager::slo_demode_finished(const Balise &balise)
{
	onlyCopyBaliseDemode(baliseVec[balise.recvNum], balise);
    //向主界面发送应答器分析完信号
    emit sig_drawErrorRateWave(balise);
	jobTracker(balise.recvNum);
}

void BaliseManager::slo_phase_finished(const int &recvNum, const QVector<float> &phaseDataVec)
{
    emit sig_drawPhaseWave(recvNum, phaseDataVec);
}

/*******************************************************************************
 * 收到getFPGAData线程收到一个应答器信号的槽函数，再此函数中将该应答器添加到待处理应答器数组中。
 * baliseManager线程每隔100ms检查待处理应答器数组
 * 进行解调和频谱分析
 * ****************************************************************************/
void BaliseManager::slo_add2BaliseVec(Balise balise)
{
    baliseVec.push_back(balise);
    //发信号给主线程
    emit sig_hasaBalise(balise);
	if (!isWorkThBusy)
	{
		startJob(balise.recvNum);
	}

}

void BaliseManager::slo_getHistorySampleData2Draw(int recvNum)
{
    if(recvNum < 0)
    {
        return;
    }
	else if (baliseVec[recvNum].sampleDataPos == -1) //采样数据还未写入
	{
		return;
	}
	else
	{
		QVector<float> temp_sampleDataVec;
		QVector<SampleDataGain> gainVec;
		QDataStream sampleDataStream(&sampleDataFile);
		qint64 tempPos = sampleDataFile.pos();
		sampleDataFile.seek(baliseVec[recvNum].sampleDataPos);
		sampleDataStream >> temp_sampleDataVec >> gainVec;
		sampleDataFile.seek(tempPos);
		emit sig_drawSampleDataWave(recvNum, temp_sampleDataVec, gainVec);
	}
}

void BaliseManager::slo_getHistoryFreData2Draw(int recvNum)
{
    if(recvNum < 0)
    {
        return;
    }
    else if(baliseVec[recvNum].freDataPos == -1) //此时该应答器的频谱计算还没有完成
    {
        return;
    }
    else
    {
        QVector<float> temp_freDataVec;
        QDataStream ds(&freFile);
        qint64 tempPos = freFile.pos();
        freFile.seek(baliseVec[recvNum].freDataPos);
        ds >> temp_freDataVec;
        freFile.seek(tempPos);
        emit sig_drawFreDataWave(baliseVec[recvNum], temp_freDataVec);
    }
}

void BaliseManager::slo_getHistoryErrorRate2Draw(int recvNum)
{
	if (recvNum < 0)
	{
		return;
	}
	else if (baliseVec[recvNum].demodedByteVec.isEmpty())
	{
		return;
	}
	else
	{
		emit sig_drawErrorRateWave(baliseVec[recvNum]);
	}
}

void BaliseManager::slo_getBalisePhaseData2Draw(int recvNum)
{
    if(recvNum < 0)
    {
        return;
    }
    QVector<float> temp_sampleDataVec;
    QDataStream sampleDataStream(&sampleDataFile);
    qint64 tempPos = sampleDataFile.pos();
    sampleDataFile.seek(baliseVec[recvNum].sampleDataPos);
    sampleDataStream >> temp_sampleDataVec;
    sampleDataFile.seek(tempPos);
    emit sig_getPhaseData(recvNum, temp_sampleDataVec);
}



void BaliseManager::slo_changeFreInfo(FreInfo freInfo)
{
    emit sig_changeFreInfo(freInfo);
}

void BaliseManager::slo_review(QString binFileName)
{
	isReview = true;
	binFile = new std::ifstream(binFileName.toStdString(), std::ios::in | std::ios::binary);
	if (!binFile->is_open())
	{
		qDebug() << "打开存储文件sampleData.bin失败!";
		return;
	}
	else
	{
		quint32 size;
		static int i = 0;
		static int pos = 0;
		while(1)
		{
			binFile->read((char*)&size, 4);
			if (!binFile->eof())
			{
				Balise balise = mkaBalise(i++, QDateTime::currentDateTime());
				balise.dataPos = binFile->tellg();
				balise.dataSize = size;
				baliseVec.push_back(balise);
				emit sig_hasaBalise(balise);//发信号给主线程
				binFile->seekg(size, std::ios::cur);
			}
			else
			{
				break;
			}
		}
		if (!isWorkThBusy)
		{
			if (!baliseVec.isEmpty())
			{
				startJob(0);
			}	
		}	
	}
}

/********************************************************************************************
* 任务调度函数，保证任务顺序执行
*
* *********************************************************************************************/
void BaliseManager::jobTracker(int jobNum)
{
	if (baliseVec[jobNum].isFreFinished && baliseVec[jobNum].isDemodFinished)//检查两个线程是否都完成工作
	{
		//检查是否都处理完
		if (baliseVec.size() - jobNum > 1)
		{//还有任务需要处理
			jobNum++;
			startJob(jobNum);  //自动开始下一个任务
		}
		else
		{//最后一个任务已经处理完
			isWorkThBusy = false;
			if (binFile != NULL && binFile->is_open())
			{
				binFile->close();
				isReview = false;
			}
		}
	}
}

void BaliseManager::startJob(int recvNum)
{
	//首先从缓存文件中读取应答器数据
	QByteArray originalBaliseData;
	originalBaliseData.resize(baliseVec[recvNum].dataSize);
	if (isReview)
	{
		binFile->clear();
		binFile->seekg(baliseVec[recvNum].dataPos, std::ios::beg);
		binFile->read(originalBaliseData.data(), baliseVec[recvNum].dataSize);
	}
	else
	{
		QMutexLocker locker(&GetFPGAData::mutex12);
		int old_write_pos = GetFPGAData::sampleData_file->tellp();
		GetFPGAData::sampleData_file->seekg(baliseVec[recvNum].dataPos, std::ios::beg);
		GetFPGAData::sampleData_file->read(originalBaliseData.data(), baliseVec[recvNum].dataSize);
		GetFPGAData::sampleData_file->seekp(old_write_pos);
	}

	caculateSampleData(baliseVec[recvNum], originalBaliseData, sampleDataVec, gainVec);  //获取采样数
	
	//发信号给主线程
	//采样数据进行三倍降采样
	emit sig_drawSampleDataWave(recvNum, sampleDataVec, gainVec); 
	//发信号给解调线程
	emit sig_demod(baliseVec[recvNum]);
	//发信号给求频谱线程
	emit sig_caculateFre(baliseVec[recvNum]);

	isWorkThBusy = true;
}

void BaliseManager::onlyCopyBaliseFre(Balise & des, const Balise & sor)
{
	des.freDataPos = sor.freDataPos;
	for (int i = 0; i < 3; i++)
	{
		des.FC[i] = sor.FC[i];
		des.FH[i] = sor.FH[i];
		des.FL[i] = sor.FL[i];
	}
	des.resolution = sor.resolution;
	des.isFreFinished = sor.isFreFinished;
}

void BaliseManager::onlyCopyBaliseDemode(Balise & des, const Balise & sor)
{
	des.demodedByteVec = sor.demodedByteVec;
	for (int i = 0; i < 128; i++)
	{
		des.msgData[i] = sor.msgData[i];
	}
	for (int i = 0; i < 104; i++)
	{
		des.userData[i] = sor.userData[i];
	}
	des.errorRateVec = sor.errorRateVec;

	des.firstCorrectBitPos = sor.firstCorrectBitPos;     //第一个能解出应答器报文的正确比特
	des.totalCorrectBitNum = sor.totalCorrectBitNum;		// 所有的正确比特个数
	des.maxRightBitNum = sor.maxRightBitNum;			// 最大连续正确比特个数
	des.correctMSGNum = sor.correctMSGNum;			// 正确的应答器报文个数
	des.MTIEVec_1 = sor.MTIEVec_1;
	des.MTIEVec_2 = sor.MTIEVec_2;
	des.bps = des.bps;                    //实际波特率
	des.minTransSpeed = sor.minTransSpeed;          //最小数据速率
	des.maxTransSpeed = sor.maxTransSpeed;          //最大数据速率
	des.averTranSpeed = sor.averTranSpeed;          //实际数据速率 
	des.isDemodFinished = sor.isDemodFinished;         //解调是否完成
}




