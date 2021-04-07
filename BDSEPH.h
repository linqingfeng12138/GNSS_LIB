# pragma once

#include<stdint.h>

#pragma pack(1)

class BDSEPH
{
public:
	BDSEPH();
	~BDSEPH();
	bool StreamAnaylse(unsigned long len, char* RxBuffer);

private:

	struct BDSEPHdataRaw_t
	{
		uint8_t PRN;
		uint8_t SatH1;
		uint8_t URA1;  /* SV accuracy (URA index) */
		int16_t Tgd1;
		uint8_t AODC;
		uint32_t toc;
		int32_t a0;		/* s */
		int32_t a1;		/* s/s */
		int16_t a2;		/* s/s2 */
		uint8_t AODE;
		uint32_t toe;
		uint32_t sqrtA;
		uint32_t e;
		int32_t omg;
		int16_t	deln;
		int32_t M0;
		int32_t OMG0;
		int32_t OMGd;   /* OMG_dot */
		int32_t i0;
		int16_t IDOT;
		int32_t cuc;
		int32_t cus;
		int32_t crc;
		int32_t crs;
		int32_t cic;
		int32_t cis;
	};

	union BDSEPHdata_U			/* 联合体 */
	{
		BDSEPHdataRaw_t BDSEPHdataRaw;		/* 未转化单位的数据 */
		uint8_t buffdata[63];
	};
	BDSEPHdata_U bdsephdataU;


	struct BDSEPHdata_t
	{
		double PRN;
		double SatH1;
		double URA1;  /* SV accuracy (URA index) */
		double Tgd1;
		double AODC;
		double toc;
		double a0;		/* s */
		double a1;		/* s/s */
		double a2;		/* s/s2 */
		double AODE;
		double toe;
		double sqrtA;
		double e;
		double omg;
		double	deln;
		double M0;
		double OMG0;
		double OMGd;   /* OMG_dot */
		double i0;
		double IDOT;
		double cuc;
		double cus;
		double crc;
		double crs;
		double cic;
		double cis;
	};
	BDSEPHdata_t BDSEPHdata;			/* 已转化单位的数据 */

	
	
	/* -----读数据状态status----*/
	enum Status_t
	{
		GOTHEAD = 1,
		GOTID,
		GOTLENGTH,
		GOTBDSEPH
	};

	/* BDS的星历数据长度恒为81 */
	uint16_t datalength = 81;	 

	/* -----数据校验----*/
	uint8_t check[2];

private:

	/* -----用于解析的函数----*/
	int findHead(char* RxBuffer);
	int findID_Code(char* RxBuffer);
	int readLength(char* RxBuffer);		
	bool readnav(char* RxBuffer);
	int readChkSum(char* RxBuffer);

	/* -----打印结果函数----*/
	bool printdata(void);

	/* -----转化单位函数----*/
	bool calculatedata(void);

};


