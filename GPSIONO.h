#pragma once

#include<stdint.h>
#include "GNSSANL.h"
class GPSIONO : public GNSSANL
{
public:
	GPSIONO();
	~GPSIONO();
	GPSIONO(uint8_t check0, uint8_t check1);
	int StreamAnaylse(unsigned long len, char* RxBuffer);
private:

#pragma pack(1) //解决了计算机由于对齐问题在结构体中储存地址不连续问题

	struct GPSIONOdataRaw_t
	{
		int8_t a0;
		int8_t a1;
		int8_t a2;
		int8_t a3;
		int8_t b0;
		int8_t b1;
		int8_t b2;
		int8_t b3;
	};

#pragma pack()		/* 结束按八位对齐的规则 */

	union GPSIONOdata_U			/* 联合体 */
	{
		GPSIONOdataRaw_t GPSIONOdataRaw;		/* 未转化单位的数据 */
		uint8_t buffdata[63];
	};
	GPSIONOdata_U gpsIONOdataU;  

	struct GPSIONOdata_t
	{
		double a0;
		double a1;
		double a2;
		double a3;
		double b0;
		double b1;
		double b2;
		double b3;
	};
	GPSIONOdata_t GPSIONOdata;	/* 已转化单位的数据 */

	/* -----读数据状态status----*/

	enum Status_t
	{
		GOTHEAD = 1,
		GOTID,
		GOTLENGTH,
		GOTGPSEPH
	};
	const uint16_t datalength = 8;	 /* GPS电离层数据长度恒为8 */

/* -----数据校验----*/

	uint8_t check[2];

	/* -----用于解析的函数----*/
	int findHead(char* RxBuffer);
	int findID_Code(char* RxBuffer);
	int readLength(char* RxBuffer);		/* length = 63 */
	bool readnav(char* RxBuffer);
	int readChkSum(char* RxBuffer);

	/* -----打印结果函数----*/
	bool printdata(void);

	/* -----转化单位函数----*/
	bool calculatedata(void);

};

