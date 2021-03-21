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
	//信息长度
	uint16_t length;
	//观测时刻 秒
	double TOW;
	//观测时刻 周
	uint16_t WN;
	//UTC 闰秒
	int8_t UTC;
	//观测量个数
	uint8_t num;
	//观测数据
	Obs * weixing[50];

	uint8_t check[2];
	//各种状态值
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

