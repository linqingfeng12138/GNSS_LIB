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
* Description	: ����TD0D01�ļ���ȡͬ��ͷ
* Return:		: 1  ��⵽ͬ��ͷ  0 ok�ҵȴ�������ȡ  -1 ��ͬ��ͷ
**********************************************************************/
int BDSINO::findHead(char* RxBuffer)
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
int BDSINO::findID_Code(char* RxBuffer)
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
* Description	: ����TD0D01�ļ���ȡ����=8
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
bool BDSINO::readnav(char* RxBuffer)
{
	static uint8_t rank = 0;  //������ ����������±�

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
* Description	: ��������У��
* Return:		: 1 ok -1 fail   0 wait for next byte
**********************************************************************/
int  BDSINO::readChkSum(char* RxBuffer)
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
* Description	: ��ӡGPS��������
* Return:		: 1 ok		0 fail
* Reference		: ����RINEX305/304��ϵ����ļ���ӡ��ʽ
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
* Description	: ����GPS���������
* Return:		: 1 ok		0 fail
**********************************************************************/
bool BDSINO::StreamAnaylse(unsigned long len, char* RxBuffer)
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