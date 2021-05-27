#include "MesAnalysis.h"

int MesAnalysis::findID_Code(char* RxBuffer)
{
	static uint8_t status = 1;
	status++;
	ID <<= 8;
	ID += *RxBuffer;
	if (status > 2)
	{
		status = 1;
		return 1;
	}
	else
	{
		return 0;
	}
}

int MesAnalysis::findHead(char* RxBuffer) {
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

int MesAnalysis::readChkSum(char* RxBuffer)
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

int MesAnalysis::StreamAnaylse(unsigned long len, char* RxBuffer)
{
	static uint8_t status = 0;        //解码数据流的进程
	static uint8_t alreadObsNum = 0;  //the number of Satellites which had already been read
	static GNSSANL * meas = NULL;
	static int result = 2;					//各子程序返回的结果

	for (int i = 0; i < len; i++)
	{
		switch (status)
		{
		case 0:
			check[0] = 0;
			check[1] = 0;
			result = findHead(RxBuffer + i);
			if (result == 1)
			{
				status = GOTHEAD;
				//std::cout << (short)status ;  //调试打印
			}
			break;
		case GOTHEAD:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if (result == 1)
			{
				status = GOTID;
				//std::cout << (short)status ;  //调试打印
			}
			break;
		case GOTID:
			//check[0] += (uint8_t)*RxBuffer;
			//check[1] += check[0];
			switch (ID)
			{
				case 0x0609:
					if (meas)
					{
						result = meas->StreamAnaylse(len, RxBuffer);
					}
					else
					{
						meas = new MEAS(check[0],check[1]);
						result =  meas->StreamAnaylse(len, RxBuffer);
					}
					break;
				case 0x0622:
					if (meas)
					{
						result = meas->StreamAnaylse(len, RxBuffer);
					}
					else
					{
						meas = new GPSEPH(check[0],check[1]);
						result = meas->StreamAnaylse(len, RxBuffer);
					}
					break;
				case 0x0625:
					if (meas)
					{
						result = meas->StreamAnaylse(len, RxBuffer);
					}
					else
					{
						meas = new GPSIONO(check[0],check[1]);
						result = meas->StreamAnaylse(len, RxBuffer);
					}
					break;
				default:
					ID = 0;
					status = 0;
					break;
			}
			if (result)
			{
				status = 0;
				if (meas != NULL)
				{
					delete meas;
					meas = NULL;
				}
			}
			break;
		}

		if (result == -1)
		{
			status = 0;
			return -1;
		}
		return 1;
	}
}