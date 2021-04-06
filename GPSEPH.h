#pragma once

#include<stdint.h>

class GPSEPH
{
public:
	GPSEPH();
	~GPSEPH();
	bool StreamAnaylse(unsigned long len, char* RxBuffer);
private:
	
	/* GPSEPH��������:˳�����TD0D01Э��;������������rtklib->typedef struct{}eph_t----*/
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
	
	union GPSEPHdata_U			/* ������ */
	{
		GPSEPHdataRaw_t GPSEPHdataRaw;		/* δת����λ������ */
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

	/* -----������״̬status----*/

	uint8_t GOTHEAD = 1;
	uint8_t GOTID = 2;
	uint8_t GOTLENGTH = 3;
	uint8_t GOTGPSEPH = 4;

	uint16_t datalength = 63;	 /* GPS���������ݳ��Ⱥ�Ϊ63 */

	/* -----����У��----*/

	uint8_t check[2];

private:

	/* -----���ڽ����ĺ���----*/
	int findHead(char* RxBuffer);
	int findID_Code(char* RxBuffer);
	int readLength(char* RxBuffer);		/* length = 63 */
	bool readnav(char* RxBuffer);    
	int readChkSum(char* RxBuffer );

	/* -----��ӡ�������----*/
	bool printdata(void);

	/* -----ת����λ����----*/
	bool calculatedata(void);

};


