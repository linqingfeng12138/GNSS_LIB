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
* Description	: ����TD0D01�ļ���ȡͬ��ͷ
* Return:		: 1  ��⵽ͬ��ͷ  0 ok�ҵȴ�������ȡ  -1 ��ͬ��ͷ
**********************************************************************/
int BDSEPH::findHead(char* RxBuffer)
{
	static uint8_t rank = 0;	//����ָʾ���ݵĵ�X-1��Byte

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
* Description	: ����TD0D01�ļ���ȡʶ����
* Return:		:   1 ok   -1 fail  0wait for next byte
**********************************************************************/
int BDSEPH::findID_Code(char* RxBuffer)
{
	static uint8_t rank = 0;	//����ָʾ���ݵĵ�X-1��Byte

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
* Description	: ����TD0D01�ļ���ȡ����=81 bytes
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
		rank = 0;					//�����������㣡
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
* Description	: ��ȡBDS����
* Return:		: 1 ok   0 wait for next byte
**********************************************************************/
bool BDSEPH::readnav(char* RxBuffer)
{
	static uint8_t rank = 0;  //������ ����������±�

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
* Description	: ��������У��
* Return:		: 1 ok -1 fail   0 wait for next byte
**********************************************************************/
int BDSEPH::readChkSum(char* RxBuffer) 
{
	static uint8_t rank = 0;

	if (rank == 0)
	{
		if ((uint8_t)*RxBuffer == check[0])  //�˴�һ��Ҫ������ת����char���з��ŵģ���check��uint8_t
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
* Description	: ���ݽ��б������ӵĵ�λת��
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
* Description	: ��ӡBDS��������
* Return:		: 1 ok		0 fail
* Referrence	: ����RINEX305.pdf��A14���������ĵ���ӡ��ʽ
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
* Description	: ����BDS��������
* Return:		: 1 ok		0 fail
**********************************************************************/
bool BDSEPH::StreamAnaylse(unsigned long len, char* RxBuffer) {

	int8_t result = 0;	//��ȡ���
	static uint8_t status = 0;  //�������ݽ���

	for (uint8_t i = 0; i < len; i++)
	{
		switch (status)
		{
		case 0:									//��ȡͷ
			check[0] = 0;
			check[1] = 0;
			result = findHead(RxBuffer + i);	//+0 ����һ�ζ�ȡ  +1 ��ɶ�ȡ  -1ʧ��
			if (result == 1)					//�����ɶ�ȡ��״̬����
			{
				status = GOTHEAD;
				std::cout << "\t" << (short)status;
			}
			break;

		case 1:									//��ȡID		
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if (result == 1)
			{
				status = GOTID;
				std::cout << (short)status;
			}
			break;

		case 2:									//��ȡ����
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i);
			if (result == 1)
			{
				status = GOTLENGTH;
				std::cout << (short)status;
			}
			break;

		case 3:									//��ȡ����
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

		case 4:									//��ȡ�����
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 5;		std::cout << (short)status << std::endl;  //���Դ�ӡ
				printdata();
				status = 0;						//��ȡ�ɹ���״̬����
			}
			break;
		default:
			status = 0;
			break;
		}

		if (result == -1)				//�����ȡʧ�ܣ���״̬���¿�ʼ
		{
			status = 0;
		}
	}
	return 1;
}