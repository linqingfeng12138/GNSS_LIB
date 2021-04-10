#include "GPSEPH.h"
#include "printNAV.h"
#include <iostream>
#include <iomanip>
#include <math.h>

#define PI          3.1415926535897932  /* pi in GPS-ICD-2000 */

using namespace std;

GPSEPH::GPSEPH()
{
}

GPSEPH::~GPSEPH()
{
}

/**********************************************************************
* Function		: findHead
* Description	: 根据TD0D01文件读取同步头
* Return:		: 1  检测到同步头  0 ok且等待继续读取  -1 无同步头
**********************************************************************/
int GPSEPH::findHead(char* RxBuffer) 
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
int GPSEPH::findID_Code(char* RxBuffer)
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
		if (*RxBuffer == 0x22)
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
* Description	: 根据TD0D01文件读取长度=63 bytes
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int GPSEPH::readLength(char* RxBuffer)
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
bool GPSEPH::readnav(char* RxBuffer)
{
	static uint8_t rank = 0;  //联合体 数组里面的下标
	
	gpsephdataU.buffdata[rank++] = *RxBuffer;
	if (rank == datalength )
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
int  GPSEPH::readChkSum(char* RxBuffer)
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
bool GPSEPH::calculatedata(void)
{
	GPSEPHdata.PRN = gpsephdataU.GPSEPHdataRaw.PRN;
	GPSEPHdata.svh = gpsephdataU.GPSEPHdataRaw.svh;
	GPSEPHdata.sva = gpsephdataU.GPSEPHdataRaw.sva;
	GPSEPHdata.fit = gpsephdataU.GPSEPHdataRaw.fit;
	GPSEPHdata.tgd = gpsephdataU.GPSEPHdataRaw.tgd/pow(2,31);			/* s */
	GPSEPHdata.iodc = gpsephdataU.GPSEPHdataRaw.iodc;
	GPSEPHdata.toc = gpsephdataU.GPSEPHdataRaw.toc * pow(2,4);			/* s */
	GPSEPHdata.f2 = gpsephdataU.GPSEPHdataRaw.f2/pow(2,55);				/* s/s2 */
	GPSEPHdata.f1 = gpsephdataU.GPSEPHdataRaw.f1 / pow(2, 43);			/* s/s */
	GPSEPHdata.f0 = gpsephdataU.GPSEPHdataRaw.f0 / pow(2, 31);			/* s */
	GPSEPHdata.iode = gpsephdataU.GPSEPHdataRaw.iode;
	GPSEPHdata.crs = gpsephdataU.GPSEPHdataRaw.crs / pow(2, 5);			/* m */
	GPSEPHdata.deln = gpsephdataU.GPSEPHdataRaw.deln *PI / pow(2, 43);	/* pi/s */
	GPSEPHdata.M0 = gpsephdataU.GPSEPHdataRaw.M0 *PI / pow(2, 31);		/* pi */
	GPSEPHdata.cuc = gpsephdataU.GPSEPHdataRaw.cuc / pow(2, 29);		/* rad */
	GPSEPHdata.e = gpsephdataU.GPSEPHdataRaw.e / pow(2, 33);	
	GPSEPHdata.cus = gpsephdataU.GPSEPHdataRaw.cus / pow(2, 29);		/* rad */
	GPSEPHdata.sqrtA = gpsephdataU.GPSEPHdataRaw.sqrtA / pow(2, 19);	/* m1/2 */
	GPSEPHdata.toes = gpsephdataU.GPSEPHdataRaw.toes * pow(2, 4);		/* s */
	GPSEPHdata.cic = gpsephdataU.GPSEPHdataRaw.cic / pow(2, 29);		/* rad */
	GPSEPHdata.OMG0 = gpsephdataU.GPSEPHdataRaw.OMG0 *PI / pow(2, 31);	/* pi */
	GPSEPHdata.cis = gpsephdataU.GPSEPHdataRaw.cis / pow(2, 29);		/* rad */
	GPSEPHdata.i0 = gpsephdataU.GPSEPHdataRaw.i0 *PI / pow(2, 31);		/* pi */
	GPSEPHdata.crc = gpsephdataU.GPSEPHdataRaw.crc / pow(2, 5);			/* rad */
	GPSEPHdata.omg = gpsephdataU.GPSEPHdataRaw.omg *PI / pow(2, 31);	/* pi */
	GPSEPHdata.OMGd = gpsephdataU.GPSEPHdataRaw.OMGd *PI / pow(2, 43);	/* pi/s */
	GPSEPHdata.idot = gpsephdataU.GPSEPHdataRaw.idot *PI / pow(2, 43);	/* pi/s */

	return 1;
}

/**********************************************************************
* Function		: printdata
* Description	: 打印GPS星历数据
* Return:		: 1 ok		0 fail
* Reference		: 根据RINEX305/304混合导航文件打印格式
**********************************************************************/
bool GPSEPH::printdata(void)
{
	printNAV NAVgps;

	std::cout << left;
	std::cout << setw(1) << "G";

	NAVgps.printPRN(GPSEPHdata.PRN);
	NAVgps.printNAVdata1dot12(GPSEPHdata.toc);
	NAVgps.printNAVdata1dot12(GPSEPHdata.f0);
	NAVgps.printNAVdata1dot12(GPSEPHdata.f1);
	NAVgps.printNAVdata1dot12(GPSEPHdata.f2);
	std::cout  << std::endl;

	std::cout << setw(4) << " ";
	NAVgps.printNAVdata1dot12(GPSEPHdata.iode);
	NAVgps.printNAVdata1dot12(GPSEPHdata.crs);
	NAVgps.printNAVdata1dot12(GPSEPHdata.deln);
	NAVgps.printNAVdata1dot12(GPSEPHdata.M0);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVgps.printNAVdata1dot12(GPSEPHdata.cuc);
	NAVgps.printNAVdata1dot12(GPSEPHdata.e);
	NAVgps.printNAVdata1dot12(GPSEPHdata.cus);
	NAVgps.printNAVdata1dot12(GPSEPHdata.sqrtA);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVgps.printNAVdata1dot12(GPSEPHdata.toes);
	NAVgps.printNAVdata1dot12(GPSEPHdata.cic);
	NAVgps.printNAVdata1dot12(GPSEPHdata.OMG0);
	NAVgps.printNAVdata1dot12(GPSEPHdata.cis);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVgps.printNAVdata1dot12(GPSEPHdata.i0);
	NAVgps.printNAVdata1dot12(GPSEPHdata.crc);
	NAVgps.printNAVdata1dot12(GPSEPHdata.omg);
	NAVgps.printNAVdata1dot12(GPSEPHdata.OMGd);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVgps.printNAVdata1dot12(GPSEPHdata.idot);
	NAVgps.printNAVdata1dot12(0.0);
	NAVgps.printNAVdata1dot12(0.0);
	NAVgps.printNAVdata1dot12(0.0);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVgps.printNAVdata1dot12(GPSEPHdata.sva);
	NAVgps.printNAVdata1dot12(GPSEPHdata.svh);
	NAVgps.printNAVdata1dot12(GPSEPHdata.tgd);
	NAVgps.printNAVdata1dot12(GPSEPHdata.iodc);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVgps.printNAVdata1dot12(0.0);
	NAVgps.printNAVdata1dot12(GPSEPHdata.fit);
	std::cout << std::endl;

	return 1;

}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: 解析GPS星历数据
* Return:		: 1 ok		0 fail	
**********************************************************************/
bool GPSEPH::StreamAnaylse(unsigned long len, char* RxBuffer)
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
