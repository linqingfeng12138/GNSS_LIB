#include "BDSEPH.h"
#include <math.h>
#include <iostream>
#include <iomanip>

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
* Description	: 数据进行比例因子的单位转化
* Return:		: 1 ok		0 fail
**********************************************************************/
bool BDSEPH::calculatedata(void) {

	BDSEPHdata.PRN = bdsephdataU.BDSEPHdataRaw.PRN;
	BDSEPHdata.SatH1 = bdsephdataU.BDSEPHdataRaw.SatH1;
	BDSEPHdata.URA1= bdsephdataU.BDSEPHdataRaw.URA1;
	BDSEPHdata.Tgd1 = bdsephdataU.BDSEPHdataRaw.Tgd1*0.1;
	BDSEPHdata.AODC = bdsephdataU.BDSEPHdataRaw.AODC;
	BDSEPHdata.toc = bdsephdataU.BDSEPHdataRaw.toc*pow(2,3);
	BDSEPHdata.a0 = bdsephdataU.BDSEPHdataRaw.a0 / pow(2,33);
	BDSEPHdata.a1 = bdsephdataU.BDSEPHdataRaw.a1 / pow(2,50);
	BDSEPHdata.a2 = bdsephdataU.BDSEPHdataRaw.a2 / pow(2,66);
	BDSEPHdata.AODE = bdsephdataU.BDSEPHdataRaw.AODE;
	BDSEPHdata.toe = bdsephdataU.BDSEPHdataRaw.toe*pow(2,3);
	BDSEPHdata.sqrtA = bdsephdataU.BDSEPHdataRaw.sqrtA/pow(2,19);
	BDSEPHdata.e = bdsephdataU.BDSEPHdataRaw.e / pow(2,33);
	BDSEPHdata.omg = bdsephdataU.BDSEPHdataRaw.omg / pow(2, 31);
	BDSEPHdata.deln = bdsephdataU.BDSEPHdataRaw.deln / pow(2, 43);
	BDSEPHdata.M0 = bdsephdataU.BDSEPHdataRaw.M0 / pow(2, 31);
	BDSEPHdata.OMG0 = bdsephdataU.BDSEPHdataRaw.OMG0 / pow(2, 31);
	BDSEPHdata.OMGd = bdsephdataU.BDSEPHdataRaw.OMGd / pow(2, 43);
	BDSEPHdata.i0 = bdsephdataU.BDSEPHdataRaw.i0 / pow(2, 31);
	BDSEPHdata.IDOT = bdsephdataU.BDSEPHdataRaw.IDOT / pow(2, 43);
	BDSEPHdata.cuc = bdsephdataU.BDSEPHdataRaw.cuc / pow(2, 31);
	BDSEPHdata.cus = bdsephdataU.BDSEPHdataRaw.cus / pow(2, 31);
	BDSEPHdata.crc = bdsephdataU.BDSEPHdataRaw.crc / pow(2, 6);
	BDSEPHdata.crs = bdsephdataU.BDSEPHdataRaw.crs / pow(2, 6);
	BDSEPHdata.cic = bdsephdataU.BDSEPHdataRaw.cic / pow(2, 31);
	BDSEPHdata.cis = bdsephdataU.BDSEPHdataRaw.cis / pow(2, 31);

	return 1;
}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: 打印BDS星历数据
* Return:		: 1 ok		0 fail
* Referrence	: 根据RINEX305.pdf的A14北斗导航文档打印格式
**********************************************************************/
bool BDSEPH::printdata(void) {

	std::cout << left;
	std::cout << setw(1) << "C"  ;
	std::cout << setw(3) << (short)BDSEPHdata.PRN ;
	std::cout << setw(20) << scientific << BDSEPHdata.toc ;
	std::cout << setw(20) << scientific << BDSEPHdata.a0 ;
	std::cout << setw(20) << scientific << BDSEPHdata.a1 ;
	std::cout << setw(20) << scientific << BDSEPHdata.a2 << std::endl;
	
	std::cout << setw(4) << "  ";
	std::cout << setw(20) << scientific << BDSEPHdata.AODE;
	std::cout << setw(20) << scientific << BDSEPHdata.crc;
	std::cout << setw(20) << scientific << BDSEPHdata.deln;
	std::cout << setw(20) << scientific << BDSEPHdata.M0 << std::endl;

	std::cout << setw(4) << "  ";
	std::cout << setw(20) << scientific << BDSEPHdata.cuc;
	std::cout << setw(20) << scientific << BDSEPHdata.e;
	std::cout << setw(20) << scientific << BDSEPHdata.cus;
	std::cout << setw(20) << scientific << BDSEPHdata.sqrtA << std::endl;
	
	std::cout << setw(4) << "  ";
	std::cout << setw(20) << scientific << BDSEPHdata.toe;
	std::cout << setw(20) << scientific << BDSEPHdata.cic;
	std::cout << setw(20) << scientific << BDSEPHdata.OMG0;
	std::cout << setw(20) << scientific << BDSEPHdata.cis << std::endl;

	std::cout << setw(4) << "  ";
	std::cout << setw(20) << scientific << BDSEPHdata.i0;
	std::cout << setw(20) << scientific << BDSEPHdata.crc;
	std::cout << setw(20) << scientific << BDSEPHdata.omg;
	std::cout << setw(20) << scientific << BDSEPHdata.OMGd << std::endl;

	std::cout << setw(4) << "  ";
	std::cout << setw(20) << scientific << BDSEPHdata.IDOT;
	std::cout << setw(20) << "------";
	std::cout << setw(20) << "BDS WEEK";
	std::cout << setw(20) << "------" << std::endl;

	std::cout << setw(4) << "  ";
	std::cout << setw(20) << scientific << BDSEPHdata.URA1;
	std::cout << setw(20) << scientific << BDSEPHdata.SatH1;
	std::cout << setw(20) << scientific << BDSEPHdata.Tgd1;
	std::cout << setw(20) << scientific << "------" << std::endl;

	std::cout << setw(4) << "  ";
	std::cout << setw(20) << "BDS WEEK";
	std::cout << setw(20) << scientific << BDSEPHdata.AODC;
	std::cout << setw(20) << "------";
	std::cout << setw(20) << "------" << std::endl;

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
				std::cout << "\t" << (short)status;
			}
			break;

		case 1:									//读取ID		
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if (result == 1)
			{
				status = GOTID;
				std::cout << (short)status;
			}
			break;

		case 2:									//读取长度
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i);
			if (result == 1)
			{
				status = GOTLENGTH;
				std::cout << (short)status;
			}
			break;

		case 3:									//读取数据
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readnav(RxBuffer + i);
			if (result == 1)
			{
				status = GOTBDSEPH;
				std::cout << (short)status;
				std::cout << "  !!  " << (short)bdsephdataU.BDSEPHdataRaw.PRN;
				calculatedata();
			}
			break;

		case 4:									//读取检验和
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 5;		std::cout << (short)status << std::endl;  //调试打印
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
		}
	}
	return 1;
}