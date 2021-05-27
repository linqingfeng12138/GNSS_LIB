#pragma once

#include <stdint.h>
#include "GNSSANL.h"

#define MAXOBS      36     /* max number of obs  ��Э���е�952�������� */
#define MAXLength   952    /* MEAS���ݵ���󳤶� */

class MEAS :public GNSSANL
{
public:
	MEAS();
	MEAS(uint8_t check0, uint8_t check1);
	~MEAS();
	int StreamAnaylse(unsigned long len, char* RxBuffer);

private:

#pragma pack(1) //����˼�������ڶ��������ڽṹ���д����ַ����������	
	
	struct Obs_t			/*ÿ�����ǵĹ۲�ֵ��������*/
	{
		uint8_t gnss; 		// GNSS���
		uint8_t ID;			//����ID
		uint8_t CN0;		//�����
		double L;			//α��
		double ph;			//�ز���λ
		float d;			//������
		uint16_t lockTime;	//�ز���λ����ʱ��
		uint8_t flag;		//��־
	};
	struct MEASdata_t
	{
		uint16_t length; 	//��Ϣ����
		double TOW;			//�۲�ʱ�� ��
		uint16_t WN;		//�۲�ʱ�� ��
		int8_t UTC;			//UTC ����
		uint8_t num;		//�۲�������

		Obs_t Obs[MAXOBS];	//�۲�����
	};
	
	union MEASdata_U			/* ������ */
	{
		MEASdata_t MEASdata;		
		uint8_t buffdata[MAXLength];
	};
	MEASdata_U MEASdataU;

#pragma pack()		/* ��������λ����Ĺ��� */

	uint8_t buffdataPoint; //ָʾ�������buffdata���꣬д�������++������ָʾ��һ����д���λ��

	uint8_t check[2];		//У���

	enum Status_t			//����״ֵ̬
	{
		GOTHEAD = 1,
		GOTID,
		GOTLENGTH,
		GOTTIME,
		GOTOBS
	};

	struct ObsTime
	{
		uint8_t year;
		uint8_t month;
		uint8_t day;
		uint8_t hour;
		uint8_t minute;
		double second;

	};

private:
	int findHead(char* RxBuffer);
	int findID_Code(char* RxBuffer);
	int readLength(char* RxBuffer, uint8_t *point);
	int readTime(char* RxBuffer,uint8_t* point);
	int readobs(char* RxBuffer , uint8_t* num ,uint8_t* point);
	int readChkSum(char* RxBuffer);
	bool printdata(void);
	uint8_t status = 0;        //�����������Ľ���
};

