#include "BDSEPH.h"
#include "printNAV.h"
#include <math.h>
#include <iostream>
#include <iomanip>

#define PI  3.1415926535898	/* pi in BDS-ICD-2019 */

using namespace std;

BDSEPH::BDSEPH()
{
}

BDSEPH::~BDSEPH()
{
}

/**********************************************************************
* Function		: findHead
* Description	: 根据TD0D01文件读取同步头
* Return:		: 1  检测到同步头  0 ok且等待继续读取  -1 无同步头
**********************************************************************/
int BDSEPH::findHead(char* RxBuffer)
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
int BDSEPH::findID_Code(char* RxBuffer)
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
		if (*RxBuffer == 0x21)
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
* Description	: 根据TD0D01文件读取长度=81 bytes
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int BDSEPH::readLength(char* RxBuffer)
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
* Description	: 读取BDS星历
* Return:		: 1 ok   0 wait for next byte
**********************************************************************/
bool BDSEPH::readnav(char* RxBuffer)
{
	static uint8_t rank = 0;  //联合体 数组里面的下标

	bdsephdataU.buffdata[rank++] = *RxBuffer;
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
int BDSEPH::readChkSum(char* RxBuffer) 
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
* Description	: 数据进行比例因子的单位转化   unit
* Return:		: 1 ok		0 fail
**********************************************************************/
bool BDSEPH::calculatedata(void) {

	BDSEPHdata.PRN = bdsephdataU.BDSEPHdataRaw.PRN;
	BDSEPHdata.SatH1 = bdsephdataU.BDSEPHdataRaw.SatH1;
	BDSEPHdata.URA1= bdsephdataU.BDSEPHdataRaw.URA1;
	BDSEPHdata.Tgd1 = bdsephdataU.BDSEPHdataRaw.Tgd1*0.1;			/* ns */
	BDSEPHdata.AODC = bdsephdataU.BDSEPHdataRaw.AODC;
	BDSEPHdata.toc = bdsephdataU.BDSEPHdataRaw.toc*pow(2,3);		/* s */
	BDSEPHdata.a0 = bdsephdataU.BDSEPHdataRaw.a0 / pow(2,33);		/* s */
	BDSEPHdata.a1 = bdsephdataU.BDSEPHdataRaw.a1 / pow(2,50);		/* s/s */
	BDSEPHdata.a2 = bdsephdataU.BDSEPHdataRaw.a2 / pow(2,66);		/* s/s2 */
	BDSEPHdata.AODE = bdsephdataU.BDSEPHdataRaw.AODE;
	BDSEPHdata.toe = bdsephdataU.BDSEPHdataRaw.toe*pow(2,3);		/* s */
	BDSEPHdata.sqrtA = bdsephdataU.BDSEPHdataRaw.sqrtA/pow(2,19);	/* m1/2 */
	BDSEPHdata.e = bdsephdataU.BDSEPHdataRaw.e / pow(2,33);
	BDSEPHdata.omg = bdsephdataU.BDSEPHdataRaw.omg * PI / pow(2, 31);	/* pi */
	BDSEPHdata.deln = bdsephdataU.BDSEPHdataRaw.deln * PI / pow(2, 43);	/* pi/s */
	BDSEPHdata.M0 = bdsephdataU.BDSEPHdataRaw.M0 * PI / pow(2, 31);		/* pi */
	BDSEPHdata.OMG0 = bdsephdataU.BDSEPHdataRaw.OMG0 * PI / pow(2, 31);	/* pi */
	BDSEPHdata.OMGd = bdsephdataU.BDSEPHdataRaw.OMGd * PI / pow(2, 43);	/* pi/s */
	BDSEPHdata.i0 = bdsephdataU.BDSEPHdataRaw.i0 * PI / pow(2, 31);		/* pi */
	BDSEPHdata.IDOT = bdsephdataU.BDSEPHdataRaw.IDOT * PI / pow(2, 43);	/* pi/s */
	BDSEPHdata.cuc = bdsephdataU.BDSEPHdataRaw.cuc / pow(2, 31);	/* rad */
	BDSEPHdata.cus = bdsephdataU.BDSEPHdataRaw.cus / pow(2, 31);	/* rad */
	BDSEPHdata.crc = bdsephdataU.BDSEPHdataRaw.crc / pow(2, 6);		/* m */
	BDSEPHdata.crs = bdsephdataU.BDSEPHdataRaw.crs / pow(2, 6);		/* m */
	BDSEPHdata.cic = bdsephdataU.BDSEPHdataRaw.cic / pow(2, 31);	/* rad */
	BDSEPHdata.cis = bdsephdataU.BDSEPHdataRaw.cis / pow(2, 31);	/* rad */

	return 1;
}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: 打印BDS星历数据
* Return:		: 1 ok		0 fail
* Reference		: 根据RINEX305/304混合导航文件打印格式
**********************************************************************/
bool BDSEPH::printdata(void) 
{
	printNAV NAVbds;

	std::cout << left;
	std::cout << setw(1) << "C";

	NAVbds.printPRN(BDSEPHdata.PRN);
	NAVbds.printNAVdata1dot12(BDSEPHdata.toc);
	NAVbds.printNAVdata1dot12(BDSEPHdata.a0);
	NAVbds.printNAVdata1dot12(BDSEPHdata.a1);
	NAVbds.printNAVdata1dot12(BDSEPHdata.a2);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVbds.printNAVdata1dot12(BDSEPHdata.AODE);
	NAVbds.printNAVdata1dot12(BDSEPHdata.crs);
	NAVbds.printNAVdata1dot12(BDSEPHdata.deln);
	NAVbds.printNAVdata1dot12(BDSEPHdata.M0);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVbds.printNAVdata1dot12(BDSEPHdata.cuc);
	NAVbds.printNAVdata1dot12(BDSEPHdata.e);
	NAVbds.printNAVdata1dot12(BDSEPHdata.cus);
	NAVbds.printNAVdata1dot12(BDSEPHdata.sqrtA);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVbds.printNAVdata1dot12(BDSEPHdata.toe);
	NAVbds.printNAVdata1dot12(BDSEPHdata.cic);
	NAVbds.printNAVdata1dot12(BDSEPHdata.OMG0);
	NAVbds.printNAVdata1dot12(BDSEPHdata.cis);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVbds.printNAVdata1dot12(BDSEPHdata.i0);
	NAVbds.printNAVdata1dot12(BDSEPHdata.crc);
	NAVbds.printNAVdata1dot12(BDSEPHdata.omg);
	NAVbds.printNAVdata1dot12(BDSEPHdata.OMGd);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVbds.printNAVdata1dot12(BDSEPHdata.IDOT);
	NAVbds.printNAVdata1dot12(0.0);
	NAVbds.printNAVdata1dot12(0.0);
	NAVbds.printNAVdata1dot12(0.0);
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVbds.printNAVdata1dot12(BDSEPHdata.URA1);
	NAVbds.printNAVdata1dot12(BDSEPHdata.SatH1);
	NAVbds.printNAVdata1dot12(BDSEPHdata.Tgd1);
	NAVbds.printNAVdata1dot12(0.0);  //TGD2 B2/B3 (seconds)
	std::cout << std::endl;

	std::cout << setw(4) << " ";
	NAVbds.printNAVdata1dot12(0.0);
	NAVbds.printNAVdata1dot12(BDSEPHdata.AODC);
	std::cout << std::endl;

	return 1;
}


/**********************************************************************
* Function		: StreamAnaylse
* Description	: 解析BDS星历数据
* Return:		: 1 ok		0 fail
**********************************************************************/
bool BDSEPH::StreamAnaylse(unsigned long len, char* RxBuffer) {

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
				//std::cout << "\t" << (short)status;
			}
			break;

		case 1:									//读取ID		
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if (result == 1)
			{
				status = GOTID;
				//std::cout << (short)status;
			}
			break;

		case 2:									//读取长度
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i);
			if (result == 1)
			{
				status = GOTLENGTH;
				//std::cout << (short)status;
			}
			break;

		case 3:									//读取数据
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readnav(RxBuffer + i);
			if (result == 1)
			{
				status = GOTBDSEPH;
				//std::cout << (short)status;
				//std::cout << "  !!  " << (short)bdsephdataU.BDSEPHdataRaw.PRN;
				calculatedata();
			}
			break;

		case 4:									//读取检验和
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 5;		
				//std::cout << (short)status << std::endl;  //调试打印
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