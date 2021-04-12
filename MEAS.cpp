#include "MEAS.h"
#include "printF.h"
#include "common.h"
#include <iostream>
#include <iomanip>

using namespace std;

MEAS::MEAS()
{
	this->MEASdataU.MEASdata.length = 0;
	this->MEASdataU.MEASdata.TOW = 0;
	this->MEASdataU.MEASdata.WN = 0;
	this->MEASdataU.MEASdata.UTC = 0;
	this->MEASdataU.MEASdata.num = 0;
	//for (int i = 0; i < MAXOBS; i++)
	//{
	//	this->MEASdataU.MEASdata.Obs[i] = 0;
	//}
	this->check[0] = 0;
	this->check[1] = 0;
	this->buffdataPoint = 0;
}

MEAS::~MEAS()
{
}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: 记录观测值数据流
* Return:		: 1 ok		0 fail
**********************************************************************/
bool MEAS::StreamAnaylse(unsigned long len, char* RxBuffer)
{
	static uint8_t status = 0;        //解码数据流的进程
	static uint8_t alreadObsNum = 0;  //the number of Satellites which had already been read
	int result = 0;					//各子程序返回的结果

	for (int i = 0; i < len; i++)
	{
		switch (status)
		{
		case 0:
			check[0] = 0;
			check[1] = 0;
			result = findHead(RxBuffer + i);
			if ( result == 1)
			{
				status = GOTHEAD;
				//std::cout << (short)status ;  //调试打印
			}
			break;
		case 1:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if(result == 1)
			{
				status = GOTID;
				//std::cout << (short)status ;  //调试打印
			}
			break;
		case 2:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i,&buffdataPoint);  //数据RxBuffer 需要存放的位置point
			if (result == 1)
			{
				status = GOTLENGTH;
				//std::cout << (short)status <<std::endl;  //调试打印
				//std::cout << MEASdataU.MEASdata.length<<"!!"<<std::endl;  //调试打印
			}
			break;
		case 3:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readTime(RxBuffer + i, &buffdataPoint);
			if (result == 1)
			{
				status = GOTTIME;
				//std::cout << (short)status ;  //调试打印
			}
			break;
		case 4:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readobs(RxBuffer + i , &alreadObsNum, &buffdataPoint);			
			if (result == 1)
			{
				alreadObsNum = 0;
				status = GOTOBS;
				//std::cout << (short)status ;  //调试打印
			}
			break;

		case 5:
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 6;
				//std::cout <<  (short)status << std::endl;  //调试打印
				//std::cout << "~!~!"<< MEASdataU.MEASdata.Obs[0].gnss << std::endl;  //调试打印
				printdata();
				status = 0;						//读取成功后状态清零
			}
			break;
		default:
			break;
		}

		if ( result == -1)
		{
			status = 0;
			return 0;
		}
		return 1;
	}
}

/**********************************************************************
* Function		: findHead
* Description	: 根据TD0D01文件读取同步头
* Return:		: 1  检测到同步头  0 ok且等待继续读取  -1 无同步头
**********************************************************************/
int MEAS::findHead(char* RxBuffer) {
	static uint8_t status = 1;

	switch (status)
	{
	case 1:
		if (*RxBuffer == 0x23)
		{
			status++;
			return 0;
		}
		else
		{
			status = 1;
			return -1;
		}
		break;
	case 2:
		if (*RxBuffer == 0x3E) 
		{
			status = 1;
			return 1;
		}
		else
		{
			status = 1;
			return -1;
		}
		break;
	}
}


/**********************************************************************
* Function		: findID_Code
* Description	: 根据TD0D01文件读取识别码
* Return:		:   1 ok   -1 fail  0wait for next byte
**********************************************************************/
int MEAS::findID_Code(char* RxBuffer)
{
	static uint8_t status = 1;
	switch (status)
	{
	case 1:
		if (*RxBuffer == 0x06)
		{
			status++;
			return 0;
		}
		else
		{
			status = 1;
			return -1;
		}
		break;
	case 2:
		if (*RxBuffer == 0x09)
		{
			status = 1;
			return 1;
		}
		else
		{
			status = 1;
			return -1;
		}
		break;
	}
}


/**********************************************************************
* Function		: readLength
* Description	: 根据TD0D01文件读取长度=12+Nx26
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int MEAS::readLength(char* RxBuffer,uint8_t *point)
{
	static uint8_t status = 1;

	if (status == 1)
	{
		if (*point != 0)  //强制从0开始写buff
		{
			*point = 0;
		}

		MEASdataU.buffdata[*point] = *RxBuffer;
		(*point)++;  //需要读写的buff向前1位
		status++;
		return 0;
	}
	else if (status == 2)
	{
		MEASdataU.buffdata[*point] = *RxBuffer;
		(*point)++;
		status = 1;
		if (MEASdataU.MEASdata.length <= MAXLength)
		{
			return 1;
		}
		else
		{
			MEASdataU.MEASdata.length = 0;
			return -1;
		}
	}
	else
	{
		status = 1;
		return -1;
	}
}

/**********************************************************************
* Function		: readTime
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int MEAS::readTime(char* RxBuffer, uint8_t* point)
{
	static uint8_t status = 1;

	if (status == 1)
	{
		if (*point != 2)  //检测是否从buffdata[2]开始写入
		{
			*point = 0;
			return -1;
		}
		else
		{
			MEASdataU.buffdata[*point] = *RxBuffer;
			(*point)++;    //写完后立即++，所以指示下一个待写入的位置
			status++;
			return 0;
		}
	}
	else if (status == 2)
	{
		MEASdataU.buffdata[*point] = *RxBuffer;
		(*point)++;    //写完后立即++，所以指示下一个待写入的位置
		if (*point < 14)
		{
			return 0;
		}
		else if (*point == 14)
		{
			status = 1;
			return 1;
		}
		else
		{
			status = 1;
			return -1;
		}
	}
	else
	{
		status = 1;
		*point = 0;
		return -1;
	}
}
			

/**********************************************************************
* Function		: readobs
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int MEAS::readobs(char* RxBuffer, uint8_t* num, uint8_t* point)
{
	static uint8_t status = 1; //指示读了多少个

	if (status == 1)
	{
		if ((*point - 14) % 26 != 0)  //检测是否从buffdata[14,40,66...]开始写入
		{
			*point = 0;
			return -1;
		}
		else
		{
			MEASdataU.buffdata[*point] = *RxBuffer;
			(*point)++;    //写完后立即++，所以指示下一个待写入的位置
			status++;	//status=2，意味着正在写某个卫星，而不是准备写入一个新卫星
			return 0;
		}
	}
	else if (status == 2)
	{
		MEASdataU.buffdata[*point] = *RxBuffer;
		(*point)++;   //写完后立即++，所以指示下一个待写入的位置
		
		if ((*point - 14) % 26 == 0)
		{
			(*num)++; //写完一个卫星
			status = 1; //下次写入新的卫星
		}

		if (*num == MEASdataU.MEASdata.num)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else //完全出错全部重新来
	{
		status = 1;
		*point = 0;
		*num = 0;
		return -1;
	}
}


/**********************************************************************
* Function		: readChkSum
* Description	: 进行数据校验
* Return:		: 1 ok -1 fail   0 wait for next byte
**********************************************************************/
int  MEAS::readChkSum(char* RxBuffer)
{
	static uint8_t rank = 0;

	if (rank == 0)
	{
		if ((uint8_t)*RxBuffer == check[0])  //此处一定要做数据转换：char是有符号的，而check是uint8_t
		{
			rank++;
			return 0;
		}
		else
		{
			rank = 0;
			return -1;
		}
	}
	else if (rank == 1)
	{
		if ((uint8_t)*RxBuffer == check[1])
		{
			rank = 0;
			return 1;
		}
		else
		{
			rank = 0;
			return -1;
		}
	}
	else
	{
		rank = 0;
		return -1;
	}
}

/**********************************************************************
* Function		: printdata
* Description	: 打印OBS
* Return:		: 1 ok		0 fail
* Reference		: 根据RINEX305/304观测文件打印格式
**********************************************************************/
bool MEAS::printdata(void)
{
	//计算从TOW,WN到日历UTC的转换
	Common MEASCalender;
	MEASCalender.BDST2EPH(MEASdataU.MEASdata.TOW, MEASdataU.MEASdata.WN, MEASCalender.ep);

	//开始打印
	printF OBS_MEAS;

	//打印源数据
	std::cout << "\t" << endl;
	std::cout << "TOW：" << this->MEASdataU.MEASdata.TOW << "\t"
				<<"WN: " << this->MEASdataU.MEASdata.WN << std::endl;
	std::cout << "UTC: " << (short)this->MEASdataU.MEASdata.UTC << "\t"
				<< "num: "<< (short)this->MEASdataU.MEASdata.num << std::endl;

	//打印EPOCH
	std::cout << left;
	std::cout << "> ";
	OBS_MEAS.printObsEpoch(MEASCalender.ep);
	std::cout << "  ";
	std::cout << (short)MEASdataU.MEASdata.num;
	std::cout << endl;

	cout << setw(12) << "GNSS CLASS" 
		<< setw(12) << "ID" 
		<< setw(12) << "CN0"
		<< setw(15) << "PR" 
		<< setw(15) << "carrier phase" 
		<< setw(12) << "Doppler"
		<< setw(12) << "lock time" 
		<< setw(12) << "Flag" 
		<< std::endl;

	for (int k = 0; k < this->MEASdataU.MEASdata.num; k++)
	{
		switch (MEASdataU.MEASdata.Obs[k].gnss)
		{
		case 1:
			std::cout << setw(12) << "BDS";
			break;
		case 2:
			std::cout << setw(12) << "GPS";
			break;
		case 3:
			std::cout << setw(12) << "GLONASS";
			break;
		default:
			break;
		}
		std::cout << setw(12) << (short)this->MEASdataU.MEASdata.Obs[k].ID 
			<< setw(12) << (short)this->MEASdataU.MEASdata.Obs[k].CN0
			<< setw(15) << this->MEASdataU.MEASdata.Obs[k].L 
			<< setw(15) << this->MEASdataU.MEASdata.Obs[k].ph 
			<< setw(12) << this->MEASdataU.MEASdata.Obs[k].d
			<< setw(12) << this->MEASdataU.MEASdata.Obs[k].lockTime 
			<< setw(12) << (short)this->MEASdataU.MEASdata.Obs[k].flag 
			<< std::endl;
	}

	return 1;
}