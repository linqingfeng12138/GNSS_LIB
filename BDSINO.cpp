#include "BDSINO.h"
#include "printF.h"
#include <iostream>
#include <iomanip>
#include <math.h>

#define PI  3.1415926535898	/* pi in BDS-ICD-2019 */

using namespace std;

BDSINO::BDSINO()
{
}

BDSINO::~BDSINO()
{
}

/**********************************************************************
* Function		: findHead
* Description	: 根据TD0D01文件读取同步头
* Return:		: 1  检测到同步头  0 ok且等待继续读取  -1 无同步头
**********************************************************************/
int BDSINO::findHead(char* RxBuffer)
{
	static uint8_t rank = 0;	//用于指示数据的第X-1个Byte

	if (rank == 0)
	{
		if (*RxBuffer == 0x23)
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
		if (*RxBuffer == 0x3E)
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
* Function		: findID_Code
* Description	: 根据TD0D01文件读取识别码
* Return:		:   1 ok   -1 fail  0wait for next byte
**********************************************************************/
int BDSINO::findID_Code(char* RxBuffer)
{
	static uint8_t rank = 0;	//用于指示数据的第X-1个Byte

	if (rank == 0)
	{
		if (*RxBuffer == 0x06)
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
		if (*RxBuffer == 0x24)
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
* Function		: readLength
* Description	: 根据TD0D01文件读取长度=8
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int BDSINO::readLength(char* RxBuffer)
{
	static uint8_t rank = 0;
	static union
	{
		uint16_t a;
		uint8_t data[2];
	}Aa;

	if (rank == 0)
	{
		Aa.data[rank] = *RxBuffer;
		rank++;
		return 0;
	}
	else if (rank == 1)
	{
		Aa.data[rank] = *RxBuffer;
		rank = 0;					//必须马上清零！
		if (Aa.a == datalength)
		{
			return 1;
		}
		else
		{
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
* Function		: readnav
* Description	: 读取GPS星历
* Return:		: 1 ok   0 wait for next byte
**********************************************************************/
bool BDSINO::readnav(char* RxBuffer)
{
	static uint8_t rank = 0;  //联合体 数组里面的下标

	bdsINOdataU.buffdata[rank++] = *RxBuffer;
	if (rank == datalength)
	{
		rank = 0;
		return 1;
	}
	else
		return 0;

}


/**********************************************************************
* Function		: readChkSum
* Description	: 进行数据校验
* Return:		: 1 ok -1 fail   0 wait for next byte
**********************************************************************/
int  BDSINO::readChkSum(char* RxBuffer)
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
* Function		: calculatedata
* Description	: 数据进行比例因子的单位转化  unit
* Return:		: 1 ok		0 fail
**********************************************************************/
bool BDSINO::calculatedata(void)
{
	BDSINOdata.a0 = bdsINOdataU.BDSINOdataRaw.a0 / pow(2, 30);
	BDSINOdata.a1 = bdsINOdataU.BDSINOdataRaw.a1 / pow(2, 27) / PI;
	BDSINOdata.a2 = bdsINOdataU.BDSINOdataRaw.a2 / pow(2, 24) / pow(PI, 2);
	BDSINOdata.a3 = bdsINOdataU.BDSINOdataRaw.a3 / pow(2, 24) / pow(PI, 3);
	BDSINOdata.b0 = bdsINOdataU.BDSINOdataRaw.b0 * pow(2, 11);
	BDSINOdata.b1 = bdsINOdataU.BDSINOdataRaw.b1 * pow(2, 14) / PI;
	BDSINOdata.b2 = bdsINOdataU.BDSINOdataRaw.b2 * pow(2, 16) / pow(PI, 2);
	BDSINOdata.b3 = bdsINOdataU.BDSINOdataRaw.b3 * pow(2, 16) / pow(PI, 3);

	return 1;
}

/**********************************************************************
* Function		: printdata
* Description	: 打印GPS星历数据
* Return:		: 1 ok		0 fail
* Reference		: 根据RINEX305/304混合导航文件打印格式
**********************************************************************/
bool BDSINO::printdata(void)
{
	cout << endl;
	cout << BDSINOdata.a0 << '\t'
		<< BDSINOdata.a1 << '\t'
		<< BDSINOdata.a2 << '\t'
		<< BDSINOdata.a3
		<< endl;
	cout << BDSINOdata.b0 << '\t'
		<< BDSINOdata.b1 << '\t'
		<< BDSINOdata.b2 << '\t'
		<< BDSINOdata.b3 << '\t'
		<< endl;

	return 1;
}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: 解析GPS电离层数据
* Return:		: 1 ok		0 fail
**********************************************************************/
bool BDSINO::StreamAnaylse(unsigned long len, char* RxBuffer)
{
	int8_t result = 0;	//读取结果
	static uint8_t status = 0;  //解析数据进程

	for (uint8_t i = 0; i < len; i++)
	{
		switch (status)
		{
		case 0:									//读取头
			check[0] = 0;
			check[1] = 0;
			result = findHead(RxBuffer + i);	//+0 等下一次读取  +1 完成读取  -1失败
			if (result == 1)					//如果完成读取则状态更新
			{
				status = GOTHEAD;
				//std::cout << "\t" << (short)status ;
			}
			break;

		case 1:									//读取ID		
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if (result == 1)
			{
				status = GOTID;
				//std::cout <<  (short)status ;				
			}
			break;

		case 2:									//读取长度
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i);
			if (result == 1)
			{
				status = GOTLENGTH;
				//std::cout <<  (short)status;
			}
			break;

		case 3:									//读取数据
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readnav(RxBuffer + i);
			if (result == 1)
			{
				status = GOTGPSEPH;
				//std::cout <<  (short)status;
				//std::cout << "  !!  " <<(short)gpsephdataU.GPSEPHdataRaw.PRN;
				calculatedata();
			}
			break;

		case 4:									//读取检验和
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 5;
				//std::cout <<  (short)status << std::endl;  //调试打印
				printdata();
				status = 0;						//读取成功后状态清零
			}
			break;
		default:
			status = 0;
			break;
		}

		if (result == -1)				//如果读取失败，则状态重新开始
		{
			status = 0;
			return 0;
		}
	}
	return 1;
}