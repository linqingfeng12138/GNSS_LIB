#pragma once

#include <stdint.h>
class Obs
{
public:
	Obs();
	~Obs();
	// GNSS���
	uint8_t gnss;
	//����ID
	uint8_t ID;
	//�����
	uint8_t CN0;
	//α��
	double L;
	//�ز���λ
	double ph;
	//������
	float d;
	//�ز���λ����ʱ��
	uint16_t lockTime;
	//��־
	uint8_t flag;
};

