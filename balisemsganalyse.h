#pragma once

#include <QObject>
#include "balisestructdefine.h"

/********************************************************************************
* 此类用于对解调后的数据进行数据分析
*
*
*
**********************************************************************************/
class BaliseMsgAnalyse 
{
public:
	BaliseMsgAnalyse();
	~BaliseMsgAnalyse();
	static bool analyse(Balise &balise, const QVector<unsigned char> demodedBitVec);
	static UCHAR leftShiftData(ULONG* regData, const size_t regBitLen, ULONG bitAppend);
	static bool isCodedMSGCorrect(QVector<UCHAR>::iterator msgBufferIter, const bool IsLong);
	static void xorData(ULONG* dataArray1, const ULONG* dataArray2);
	static bool baliseMSGSynch(QVector<UCHAR>::iterator msgBeforeSynchIter, UCHAR* msgAfterSynch);
	static inline UCHAR getBitValue(const ULONG number, const size_t bitPos) { return (number >> (bitPos - 1)) & 1; }
	static void caculateMTIEAndBPS(Balise &balise, const QVector<float>& sampleDataVec);
	/*
	功能：
	获取输入数据的高11比特有效数据

	输入参数：
	1. Num:				输入数据
	2. ValidBitNum:		输入数据中有效比特的长度，单位为bit

	输出参数：
	输入数据的高11位数据
	*/
	static inline USHORT getHigh11Bit(const ULONG num, const USHORT validBitNum)
	{
		return (num >> (validBitNum - CODE_MSG_WORD_LEN_IN_BIT)) & 0x7FF;
	}
	/*
	功能：
	获取数据的低10比特数据

	输入参数：
	1. Number	：	输入数据

	输出参数：
	输入数据的低11比特数据
	*/
	inline static USHORT getLow10Bit(ULONG number)
	{
		return number & 0x3FF;
	}
	static int isValidSynchCode(const USHORT synchCode);
	static bool decode(const UCHAR* srcPtr, UCHAR* dstPtr, const bool isLong);
	static bool checkValidWord(const USHORT* msgPtr, const size_t number);
	static void getCheckBits(const USHORT* srcPtr, size_t bitNum, ULONG* checkBitPtr);
	static QString getBaliseID(const BYTE* msgBuffer);
	static void descramble10BitData(const USHORT scrambleCode, const USHORT* srcPtr, USHORT* dstPtr, const bool isLong);
	static void calMTIE(const double realDeviationOf1bitVec[], QVector<double> &mtieVec);
	static void findMax(const double* vector, const int vecSize, double &max, int &maxPos);
	static void findMin(const double* vector, const int vecSize, double &min, int &minPos);
private:
	static const short		CONVER_TABLE_OF_11BIT_TO_10BIT[];		// 十一比特转十比特转换表
	static const size_t		LONG_MSG_LEN_IN_BIT = 830;				// 编码前的长报文长度，单位为比特
	static const size_t		SHORT_MSG_LEN_IN_BIT = 210;				// 编码前的短报文长度，单位为比特
	static const size_t		SHORT_CODE_MSG_LEN_IN_BIT = 341;		// 编码后的短报文长度，单位为比特	
	static const size_t		DECODE_MSG_WORD_LEN_IN_BIT = 10;		// 编码前每个报文的有效字长度，单位为比特
	static const size_t		CODE_MSG_WORD_LEN_IN_BIT = 11;			// 编码后每个报文的有效字长度，单位为比特
	static const size_t		USHORT_LEN_IN_BIT = 16;					// USHORT数据类型的长度，单位为比特
	static const size_t		ULONG_LEN_IN_BIT = 32;					// ULONG数据类型的长度，单位为比特
	static const size_t		UCHAR_LEN_IN_BIT = 8;					// UCHAR数据类型的长度，单位为比特
	static const size_t		LONG_MSG_LEN_IN_BYTE = 832 / 8;			// 编码前的长报文长度，单位为字节
	static const size_t		SHORT_MSG_LEN_IN_BYTE = 216 / 8;		// 编码前的短报文长度，单位为字节
	static const size_t		LONG_CODE_MSG_LEN_IN_BYTE = 1024 / 8;	// 编码后的长报文长度，单位为字节
	static const size_t		LONG_CODE_MSG_LEN_IN_BIT = 1023;		// 编码后的长报文长度，单位为比特
	static const size_t		SHORT_CODE_MSG_LEN_IN_BYTE = 344 / 8;	// 编码后的短报文长度，单位为字节
	static const int        deviationPointOf8bit_1 = 28;
	constexpr static const double     sampleRate = 63.221;
	constexpr static const double     bpsTrue = 63.221 / 0.56448;		// 标准波特率
	static const USHORT			FLX = 0x6DF;		// fl(x)的多项式		
	static const UCHAR			GLX[76];			// gl(x)的多项式		
};
