#include "GPSEPH.h"
#include <iostream>
#include <iomanip>
#include <math.h>

using namespace std;

GPSEPH::GPSEPH()
{
	//this->PRN = 0;
	//this->svh = 0;
	//this->sva = 0;
	//this->fit = 0;
	//this->tgd = 0;
	//this->iodc = 0;
	//this->toc = 0;
	//this->f2 = 0;	
	//this->f1 = 0;
	//this->f0 = 0;	
	//this->iode = 0;
	//this->crs = 0;
	//this->deln = 0;
	//this->M0 = 0;
	//this->cuc = 0;
	//this->e	= 0;
	//this->cus = 0;
	//this->sqrtA = 0;
	//this->toes	= 0;
	//this->cic = 0;
	//this->OMG0 = 0;
	//this->cis = 0;
	//this->i0 = 0; 
	//this->crc = 0;
	//this->omg = 0;
	//this->OMGd = 0;
	//this->idot = 0;

	this->check[0] = 0;
	this->check[1] = 0;
}

GPSEPH::~GPSEPH()
{
}

/**********************************************************************
* Function		: findHead
* Description	: ����TD0D01�ļ���ȡͬ��ͷ
* Return:		: 1  ��⵽ͬ��ͷ  0 ok�ҵȴ�������ȡ  -1 ��ͬ��ͷ
**********************************************************************/
int GPSEPH::findHead(char* RxBuffer) 
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
int GPSEPH::findID_Code(char* RxBuffer)
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
* Description	: ����TD0D01�ļ���ȡ����=63 bytes
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
* Description	: ��ȡGPS����
* Return:		: 1 ok   0 wait for next byte
**********************************************************************/
bool GPSEPH::readnav(char* RxBuffer)
{
	static uint8_t rank = 0;  //������ ����������±�
	
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
* Description	: ��������У��
* Return:		: 1 ok -1 fail   0 wait for next byte
**********************************************************************/
int  GPSEPH::readChkSum(char* RxBuffer)
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
bool GPSEPH::calculatedata(void)
{
	GPSEPHdata.PRN = gpsephdataU.GPSEPHdataRaw.PRN;
	GPSEPHdata.svh = gpsephdataU.GPSEPHdataRaw.svh;
	GPSEPHdata.sva = gpsephdataU.GPSEPHdataRaw.sva;
	GPSEPHdata.fit = gpsephdataU.GPSEPHdataRaw.fit;
	GPSEPHdata.tgd = gpsephdataU.GPSEPHdataRaw.tgd/pow(2,31);
	GPSEPHdata.iodc = gpsephdataU.GPSEPHdataRaw.iodc;
	GPSEPHdata.toc = gpsephdataU.GPSEPHdataRaw.toc * pow(2,4);
	GPSEPHdata.f2 = gpsephdataU.GPSEPHdataRaw.f2/pow(2,55);
	GPSEPHdata.f1 = gpsephdataU.GPSEPHdataRaw.f1 / pow(2, 43);
	GPSEPHdata.f0 = gpsephdataU.GPSEPHdataRaw.f0 / pow(2, 31);
	GPSEPHdata.iode = gpsephdataU.GPSEPHdataRaw.iode;
	GPSEPHdata.crs = gpsephdataU.GPSEPHdataRaw.crs / pow(2, 5);
	GPSEPHdata.deln = gpsephdataU.GPSEPHdataRaw.deln / pow(2, 43);
	GPSEPHdata.M0 = gpsephdataU.GPSEPHdataRaw.M0 / pow(2, 31);
	GPSEPHdata.cuc = gpsephdataU.GPSEPHdataRaw.cuc / pow(2, 29);
	GPSEPHdata.e = gpsephdataU.GPSEPHdataRaw.e / pow(2, 33);
	GPSEPHdata.cus = gpsephdataU.GPSEPHdataRaw.cus / pow(2, 29);
	GPSEPHdata.sqrtA = gpsephdataU.GPSEPHdataRaw.sqrtA / pow(2, 19);
	GPSEPHdata.toes = gpsephdataU.GPSEPHdataRaw.toes * pow(2, 4);
	GPSEPHdata.cic = gpsephdataU.GPSEPHdataRaw.cic / pow(2, 29);
	GPSEPHdata.OMG0 = gpsephdataU.GPSEPHdataRaw.OMG0 / pow(2, 31);
	GPSEPHdata.cis = gpsephdataU.GPSEPHdataRaw.cis / pow(2, 29);
	GPSEPHdata.i0 = gpsephdataU.GPSEPHdataRaw.i0 / pow(2, 31);
	GPSEPHdata.crc = gpsephdataU.GPSEPHdataRaw.crc / pow(2, 5);
	GPSEPHdata.omg = gpsephdataU.GPSEPHdataRaw.omg / pow(2, 31);
	GPSEPHdata.OMGd = gpsephdataU.GPSEPHdataRaw.OMGd / pow(2, 43);
	GPSEPHdata.idot = gpsephdataU.GPSEPHdataRaw.idot / pow(2, 43);

	return 1;
}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: ��ӡGPS��������
* Return:		: 1 ok		0 fail
**********************************************************************/
bool GPSEPH::printdata(void)
{
	//std::cout << left;
	std::cout << (short)GPSEPHdata.PRN	<<  std::endl;
	std::cout << "\t";
	std::cout << setw(15) << scientific << GPSEPHdata.toc << "\t""\t";
	std::cout << setw(15) << scientific << GPSEPHdata.f0 << "\t""\t";
	std::cout << setw(15) << scientific << GPSEPHdata.f1 << "\t""\t";
	std::cout << setw(15) << scientific << GPSEPHdata.f2 << std::endl;

	std::cout << "\t";
	std::cout << setw(15) << GPSEPHdata.iode << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.crs << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.deln << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.M0 << std::endl;

	std::cout << "\t";
	std::cout << setw(15) << GPSEPHdata.cuc << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.e << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.cus << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.sqrtA << std::endl;

	std::cout << "\t";
	std::cout << setw(15) << GPSEPHdata.toes << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.cic << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.OMG0 << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.cis << std::endl;

	std::cout << "\t";
	std::cout << setw(15) << GPSEPHdata.idot << "\t""\t";
	std::cout << setw(15) << "L2�ز��ϵ���"				<< "\t""\t";
	std::cout << setw(15) << "GPS�ܺ�"					<< "\t""\t";
	std::cout << setw(15) << "L2P�����ݱ�־"			<< std::endl;

	std::cout << "\t";
	std::cout << setw(15) << GPSEPHdata.i0 << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.crc << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.omg << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.OMGd << std::endl;

	std::cout << "\t";
	std::cout << setw(15) << GPSEPHdata.sva << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.svh << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.tgd << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.iodc << std::endl;

	std::cout << "\t";
	std::cout << setw(15) << "��Ϣ����ʱ��" << "\t""\t";
	std::cout << setw(15) << GPSEPHdata.fit << "\t""\t";
	std::cout << setw(15) << "����" << "\t""\t";
	std::cout << setw(15)<< "����" << std::endl;

	return 1;

}

/**********************************************************************
* Function		: StreamAnaylse
* Description	: ����GPS��������
* Return:		: 1 ok		0 fail	
**********************************************************************/
bool GPSEPH::StreamAnaylse(unsigned long len, char* RxBuffer)
{
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
				std::cout << "\t" << (short)status ;
			}
			break;

		case 1:									//��ȡID		
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if (result == 1)		
			{
				status = GOTID;
				std::cout <<  (short)status ;				
			}
			break;
			
		case 2:									//��ȡ����
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i);
			if (result == 1)				
			{
				status = GOTLENGTH;
				std::cout <<  (short)status;
			}
			break;
			
			case 3:									//��ȡ����
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readnav(RxBuffer + i);
			if (result == 1)
			{
				status = GOTGPSEPH;
				std::cout <<  (short)status;
				std::cout << "  !!  " <<(short)gpsephdataU.GPSEPHdataRaw.PRN;
				calculatedata();
			}
			break;
		
		case 4:									//��ȡ�����
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 5;		std::cout <<  (short)status << std::endl;  //���Դ�ӡ
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
