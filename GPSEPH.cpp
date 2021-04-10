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
* Description	: ���ݽ��б������ӵĵ�λת��  unit
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
* Description	: ��ӡGPS��������
* Return:		: 1 ok		0 fail
* Reference		: ����RINEX305/304��ϵ����ļ���ӡ��ʽ
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
				//std::cout << "\t" << (short)status ;
			}
			break;

		case 1:									//��ȡID		
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if (result == 1)		
			{
				status = GOTID;
				//std::cout <<  (short)status ;				
			}
			break;
			
		case 2:									//��ȡ����
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i);
			if (result == 1)				
			{
				status = GOTLENGTH;
				//std::cout <<  (short)status;
			}
			break;
			
			case 3:									//��ȡ����
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
		
		case 4:									//��ȡ�����
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 5;		
				//std::cout <<  (short)status << std::endl;  //���Դ�ӡ
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
			return 0;
		}
	}
	return 1;
}
