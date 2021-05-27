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

#pragma pack(1) //����˼�������ڶ��������ڽṹ���д����ַ����������

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

#pragma pack()		/* ��������λ����Ĺ��� */

	union GPSIONOdata_U			/* ������ */
	{
		GPSIONOdataRaw_t GPSIONOdataRaw;		/* δת����λ������ */
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
	GPSIONOdata_t GPSIONOdata;	/* ��ת����λ������ */

	/* -----������״̬status----*/

	enum Status_t
	{
		GOTHEAD = 1,
		GOTID,
		GOTLENGTH,
		GOTGPSEPH
	};
	const uint16_t datalength = 8;	 /* GPS��������ݳ��Ⱥ�Ϊ8 */

/* -----����У��----*/

	uint8_t check[2];

	/* -----���ڽ����ĺ���----*/
	int findHead(char* RxBuffer);
	int findID_Code(char* RxBuffer);
	int readLength(char* RxBuffer);		/* length = 63 */
	bool readnav(char* RxBuffer);
	int readChkSum(char* RxBuffer);

	/* -----��ӡ�������----*/
	bool printdata(void);

	/* -----ת����λ����----*/
	bool calculatedata(void);

};

