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
* Description	: ��¼�۲�ֵ������
* Return:		: 1 ok		0 fail
**********************************************************************/
bool MEAS::StreamAnaylse(unsigned long len, char* RxBuffer)
{
	static uint8_t status = 0;        //�����������Ľ���
	static uint8_t alreadObsNum = 0;  //the number of Satellites which had already been read
	int result = 0;					//���ӳ��򷵻صĽ��

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
				//std::cout << (short)status ;  //���Դ�ӡ
			}
			break;
		case 1:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if(result == 1)
			{
				status = GOTID;
				//std::cout << (short)status ;  //���Դ�ӡ
			}
			break;
		case 2:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i,&buffdataPoint);  //����RxBuffer ��Ҫ��ŵ�λ��point
			if (result == 1)
			{
				status = GOTLENGTH;
				//std::cout << (short)status <<std::endl;  //���Դ�ӡ
				//std::cout << MEASdataU.MEASdata.length<<"!!"<<std::endl;  //���Դ�ӡ
			}
			break;
		case 3:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readTime(RxBuffer + i, &buffdataPoint);
			if (result == 1)
			{
				status = GOTTIME;
				//std::cout << (short)status ;  //���Դ�ӡ
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
				//std::cout << (short)status ;  //���Դ�ӡ
			}
			break;

		case 5:
			result = readChkSum(RxBuffer + i);
			if (result == 1)
			{
				status = 6;
				//std::cout <<  (short)status << std::endl;  //���Դ�ӡ
				//std::cout << "~!~!"<< MEASdataU.MEASdata.Obs[0].gnss << std::endl;  //���Դ�ӡ
				printdata();
				status = 0;						//��ȡ�ɹ���״̬����
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
* Description	: ����TD0D01�ļ���ȡͬ��ͷ
* Return:		: 1  ��⵽ͬ��ͷ  0 ok�ҵȴ�������ȡ  -1 ��ͬ��ͷ
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
* Description	: ����TD0D01�ļ���ȡʶ����
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
* Description	: ����TD0D01�ļ���ȡ����=12+Nx26
* Return:		: 1 ok   -1 fail   0wait for next byte
**********************************************************************/
int MEAS::readLength(char* RxBuffer,uint8_t *point)
{
	static uint8_t status = 1;

	if (status == 1)
	{
		if (*point != 0)  //ǿ�ƴ�0��ʼдbuff
		{
			*point = 0;
		}

		MEASdataU.buffdata[*point] = *RxBuffer;
		(*point)++;  //��Ҫ��д��buff��ǰ1λ
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
		if (*point != 2)  //����Ƿ��buffdata[2]��ʼд��
		{
			*point = 0;
			return -1;
		}
		else
		{
			MEASdataU.buffdata[*point] = *RxBuffer;
			(*point)++;    //д�������++������ָʾ��һ����д���λ��
			status++;
			return 0;
		}
	}
	else if (status == 2)
	{
		MEASdataU.buffdata[*point] = *RxBuffer;
		(*point)++;    //д�������++������ָʾ��һ����д���λ��
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
	static uint8_t status = 1; //ָʾ���˶��ٸ�

	if (status == 1)
	{
		if ((*point - 14) % 26 != 0)  //����Ƿ��buffdata[14,40,66...]��ʼд��
		{
			*point = 0;
			return -1;
		}
		else
		{
			MEASdataU.buffdata[*point] = *RxBuffer;
			(*point)++;    //д�������++������ָʾ��һ����д���λ��
			status++;	//status=2����ζ������дĳ�����ǣ�������׼��д��һ��������
			return 0;
		}
	}
	else if (status == 2)
	{
		MEASdataU.buffdata[*point] = *RxBuffer;
		(*point)++;   //д�������++������ָʾ��һ����д���λ��
		
		if ((*point - 14) % 26 == 0)
		{
			(*num)++; //д��һ������
			status = 1; //�´�д���µ�����
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
	else //��ȫ����ȫ��������
	{
		status = 1;
		*point = 0;
		*num = 0;
		return -1;
	}
}


/**********************************************************************
* Function		: readChkSum
* Description	: ��������У��
* Return:		: 1 ok -1 fail   0 wait for next byte
**********************************************************************/
int  MEAS::readChkSum(char* RxBuffer)
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
* Function		: printdata
* Description	: ��ӡOBS
* Return:		: 1 ok		0 fail
* Reference		: ����RINEX305/304�۲��ļ���ӡ��ʽ
**********************************************************************/
bool MEAS::printdata(void)
{
	//�����TOW,WN������UTC��ת��
	Common MEASCalender;
	MEASCalender.BDST2EPH(MEASdataU.MEASdata.TOW, MEASdataU.MEASdata.WN, MEASCalender.ep);

	//��ʼ��ӡ
	printF OBS_MEAS;

	//��ӡԴ����
	std::cout << "\t" << endl;
	std::cout << "TOW��" << this->MEASdataU.MEASdata.TOW << "\t"
				<<"WN: " << this->MEASdataU.MEASdata.WN << std::endl;
	std::cout << "UTC: " << (short)this->MEASdataU.MEASdata.UTC << "\t"
				<< "num: "<< (short)this->MEASdataU.MEASdata.num << std::endl;

	//��ӡEPOCH
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