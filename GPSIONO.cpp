#include "GPSIONO.h"
#include "printF.h"
#include <iostream>
#include <iomanip>
#include <math.h>

#define PI          3.1415926535897932  /* pi in GPS-ICD-2000 */

using namespace std;

GPSIONO::GPSIONO()
{
}

GPSIONO::~GPSIONO()
{
}

/**********************************************************************
* Function		: findHead
* Description	: 根据TD0D01文件读取同步头
* Return:		: 1  检测到同步头  0 ok且等待继续读取  -1 无同步头
**********************************************************************/
int GPSIONO::findHead(char* RxBuffer)
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
int GPSIONO::findID_Code(char* RxBuffer)
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
		if (*RxBuffer == 0x25)
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
* Description	: 根据TD0D01文件读取长度=8 bytes
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int GPSIONO::readLength(char* RxBuffer)
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
bool GPSIONO::readnav(char* RxBuffer)
{
	static uint8_t rank = 0;  //联合体 数组里面的下标

	gpsIONOdataU.buffdata[rank++] = *RxBuffer;
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
int  GPSIONO::readChkSum(char* RxBuffer)
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
bool GPSIONO::calculatedata(void)
{
	GPSIONOdata.a0 = gpsIONOdataU.GPSIONOdataRaw.a0 / pow(2, 30);
	GPSIONOdata.a1 = gpsIONOdataU.GPSIONOdataRaw.a1 / pow(2, 27)  / PI ;
	GPSIONOdata.a2 = gpsIONOdataU.GPSIONOdataRaw.a2 / pow(2, 24) / pow(PI, 2);
	GPSIONOdata.a3 = gpsIONOdataU.GPSIONOdataRaw.a3 / pow(2, 24) / pow(PI, 3);
	GPSIONOdata.b0 = gpsIONOdataU.GPSIONOdataRaw.b0 * pow(2, 11);
	GPSIONOdata.b1 = gpsIONOdataU.GPSIONOdataRaw.b1 * pow(2, 14) / PI;
	GPSIONOdata.b2 = gpsIONOdataU.GPSIONOdataRaw.b2 * pow(2, 16) / pow(PI, 2);
	GPSIONOdata.b3 = gpsIONOdataU.GPSIONOdataRaw.b3 * pow(2, 16) / pow(PI, 3);

	return 1;
}

/**********************************************************************
* Function		: printdata
* Description	: 打印GPS星历数据
* Return:		: 1 ok		0 fail
* Reference		: 根据RINEX305/304混合导航文件打印格式
**********************************************************************/
bool GPSIONO::printdata(void)
{
	cout << endl;
	cout << GPSIONOdata.a0 << '\t'
		<< GPSIONOdata.a1 << '\t'
		<< GPSIONOdata.a2 << '\t'
		<< GPSIONOdata.a3
		<< endl;
	cout << GPSIONOdata.b0 << '\t'
		<< GPSIONOdata.b1 << '\t'
		<< GPSIONOdata.b2 << '\t'
		<< GPSIONOdata.b3 << '\t'
		<<endl;
	
	return 1;
}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: 解析GPS电离层数据
* Return:		: 1 ok		0 fail
**********************************************************************/
bool GPSIONO::StreamAnaylse(unsigned long len, char* RxBuffer)
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