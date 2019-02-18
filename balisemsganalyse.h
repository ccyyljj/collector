#pragma once

#include <QObject>
#include "balisestructdefine.h"

/********************************************************************************
* �������ڶԽ��������ݽ������ݷ���
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
	���ܣ�
	��ȡ�������ݵĸ�11������Ч����

	���������
	1. Num:				��������
	2. ValidBitNum:		������������Ч���صĳ��ȣ���λΪbit

	���������
	�������ݵĸ�11λ����
	*/
	static inline USHORT getHigh11Bit(const ULONG num, const USHORT validBitNum)
	{
		return (num >> (validBitNum - CODE_MSG_WORD_LEN_IN_BIT)) & 0x7FF;
	}
	/*
	���ܣ�
	��ȡ���ݵĵ�10��������

	���������
	1. Number	��	��������

	���������
	�������ݵĵ�11��������
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
	static const short		CONVER_TABLE_OF_11BIT_TO_10BIT[];		// ʮһ����תʮ����ת����
	static const size_t		LONG_MSG_LEN_IN_BIT = 830;				// ����ǰ�ĳ����ĳ��ȣ���λΪ����
	static const size_t		SHORT_MSG_LEN_IN_BIT = 210;				// ����ǰ�Ķ̱��ĳ��ȣ���λΪ����
	static const size_t		SHORT_CODE_MSG_LEN_IN_BIT = 341;		// �����Ķ̱��ĳ��ȣ���λΪ����	
	static const size_t		DECODE_MSG_WORD_LEN_IN_BIT = 10;		// ����ǰÿ�����ĵ���Ч�ֳ��ȣ���λΪ����
	static const size_t		CODE_MSG_WORD_LEN_IN_BIT = 11;			// �����ÿ�����ĵ���Ч�ֳ��ȣ���λΪ����
	static const size_t		USHORT_LEN_IN_BIT = 16;					// USHORT�������͵ĳ��ȣ���λΪ����
	static const size_t		ULONG_LEN_IN_BIT = 32;					// ULONG�������͵ĳ��ȣ���λΪ����
	static const size_t		UCHAR_LEN_IN_BIT = 8;					// UCHAR�������͵ĳ��ȣ���λΪ����
	static const size_t		LONG_MSG_LEN_IN_BYTE = 832 / 8;			// ����ǰ�ĳ����ĳ��ȣ���λΪ�ֽ�
	static const size_t		SHORT_MSG_LEN_IN_BYTE = 216 / 8;		// ����ǰ�Ķ̱��ĳ��ȣ���λΪ�ֽ�
	static const size_t		LONG_CODE_MSG_LEN_IN_BYTE = 1024 / 8;	// �����ĳ����ĳ��ȣ���λΪ�ֽ�
	static const size_t		LONG_CODE_MSG_LEN_IN_BIT = 1023;		// �����ĳ����ĳ��ȣ���λΪ����
	static const size_t		SHORT_CODE_MSG_LEN_IN_BYTE = 344 / 8;	// �����Ķ̱��ĳ��ȣ���λΪ�ֽ�
	static const int        deviationPointOf8bit_1 = 28;
	constexpr static const double     sampleRate = 63.221;
	constexpr static const double     bpsTrue = 63.221 / 0.56448;		// ��׼������
	static const USHORT			FLX = 0x6DF;		// fl(x)�Ķ���ʽ		
	static const UCHAR			GLX[76];			// gl(x)�Ķ���ʽ		
};
