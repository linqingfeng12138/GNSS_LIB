#pragma once

#include<stdint.h>

class GPSEPH
{
public:
	GPSEPH();
	~GPSEPH();
	bool StreamAnaylse(unsigned long len, char* RxBuffer);
private:
	
	/* GPSEPH星历数据:顺序参照TD0D01协议;部分命名参照rtklib->typedef struct{}eph_t----*/
	struct GPSEPHdataRaw_t
	{
		uint8_t PRN;
		uint8_t svh;
		uint8_t sva;  /* SV accuracy (URA index) */
		uint8_t fit;
		int8_t	tgd;
		uint16_t iodc;
		uint16_t toc;
		int8_t	f2;
		int16_t f1;
		int32_t f0;
		uint8_t iode;
		int16_t crs;
		int16_t deln;
		int32_t M0;
		int16_t cuc;
		uint32_t e;
		int16_t cus;
		uint32_t sqrtA;
		uint16_t toes;
		int16_t cic;
		int32_t OMG0;
		int16_t cis;
		int32_t i0;
		int16_t crc;
		int32_t omg;
		int32_t OMGd;   /* OMG_dot */
		int16_t idot;
	};
	
	union GPSEPHdata_U			/* 联合体 */
	{
		GPSEPHdataRaw_t GPSEPHdataRaw;		/* 未转化单位的数据 */
		uint8_t buffdata[63];
	};
	GPSEPHdata_U gpsephdataU;

	struct GPSEPHdata_t
	{
		double PRN;
		double svh;
		double sva;  /* SV accuracy (URA index) */
		double fit;
		double tgd;
		double iodc;
		double toc;
		double	f2;
		double f1;
		double f0;
		double iode;
		double crs;
		double deln;
		double M0;
		double cuc;
		double e;
		double cus;
		double sqrtA;
		double toes;
		double cic;
		double OMG0;
		double cis;
		double i0;
		double crc;
		double omg;
		double OMGd;   /* OMG_dot */
		double idot;
	};
	GPSEPHdata_t GPSEPHdata;

	/* -----读数据状态status----*/

	uint8_t GOTHEAD = 1;
	uint8_t GOTID = 2;
	uint8_t GOTLENGTH = 3;
	uint8_t GOTGPSEPH = 4;

	uint16_t datalength = 63;	 /* GPS的星历数据长度恒为63 */

	/* -----数据校验----*/

	uint8_t check[2];

private:

	/* -----用于解析的函数----*/
	int findHead(char* RxBuffer);
	int findID_Code(char* RxBuffer);
	int readLength(char* RxBuffer);		/* length = 63 */
	bool readnav(char* RxBuffer);    
	int readChkSum(char* RxBuffer );

	/* -----打印结果函数----*/
	bool printdata(void);

	/* -----转化单位函数----*/
	bool calculatedata(void);

};


