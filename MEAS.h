#pragma once

#include <stdint.h>
#include "Obs.h"
class MEAS
{
public:
	MEAS();
	~MEAS();
	void StreamAnaylse(unsigned long len, char* RxBuffer);
private:
	//��Ϣ����
	uint16_t length;
	//�۲�ʱ�� ��
	double TOW;
	//�۲�ʱ�� ��
	uint16_t WN;
	//UTC ����
	int8_t UTC;
	//�۲�������
	uint8_t num;
	//�۲�����
	Obs * weixing[50];

	uint8_t check[2];
	//����״ֵ̬
	int FINDHEAD = 3;
	int FINDIDCODE = 3;
	int GETLENGTH = 3;
	int GETTIME = 13;
	int GETOBS = 27;

private:
	int findHead(char* RxBuffer);
	int findID_Code(char* RxBuffer);
	int readLength(char* RxBuffer);
	int readTime(char* RxBuffer);
	int readobs(char* RxBuffer , int num );
	uint16_t leftlen;
};

