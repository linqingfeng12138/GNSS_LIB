#pragma once

#include <stdint.h>
class Obs
{
public:
	Obs();
	~Obs();
	// GNSS类别
	uint8_t gnss;
	//卫星ID
	uint8_t ID;
	//信噪比
	uint8_t CN0;
	//伪距
	double L;
	//载波相位
	double ph;
	//多普勒
	float d;
	//载波相位锁定时间
	uint16_t lockTime;
	//标志
	uint8_t flag;
};

