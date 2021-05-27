#pragma once

#include <stdint.h>
#include "GNSSANL.h"

#define MAXOBS      36     /* max number of obs  受协议中的952长度限制 */
#define MAXLength   952    /* MEAS数据的最大长度 */

class MEAS :public GNSSANL
{
public:
	MEAS();
	MEAS(uint8_t check0, uint8_t check1);
	~MEAS();
	int StreamAnaylse(unsigned long len, char* RxBuffer);

private:

#pragma pack(1) //解决了计算机由于对齐问题在结构体中储存地址不连续问题	
	
	struct Obs_t			/*每个卫星的观测值包含以下*/
	{
		uint8_t gnss; 		// GNSS类别
		uint8_t ID;			//卫星ID
		uint8_t CN0;		//信噪比
		double L;			//伪距
		double ph;			//载波相位
		float d;			//多普勒
		uint16_t lockTime;	//载波相位锁定时间
		uint8_t flag;		//标志
	};
	struct MEASdata_t
	{
		uint16_t length; 	//信息长度
		double TOW;			//观测时刻 秒
		uint16_t WN;		//观测时刻 周
		int8_t UTC;			//UTC 闰秒
		uint8_t num;		//观测量个数

		Obs_t Obs[MAXOBS];	//观测数据
	};
	
	union MEASdata_U			/* 联合体 */
	{
		MEASdata_t MEASdata;		
		uint8_t buffdata[MAXLength];
	};
	MEASdata_U MEASdataU;

#pragma pack()		/* 结束按八位对齐的规则 */

	uint8_t buffdataPoint; //指示联合体的buffdata坐标，写完后立即++，所以指示下一个待写入的位置

	uint8_t check[2];		//校验和

	enum Status_t			//各种状态值
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
	uint8_t status = 0;        //解码数据流的进程
};

