#include "MEAS.h"
#include <iostream>
#include <iomanip>

using namespace std;

MEAS::MEAS()
{
	this->length = 0;
	this->TOW = 0;
	this->WN = 0;
	this->UTC = 0;
	this->num = 0;
	for (int i = 0; i < 50; i++)
	{
		this->weixing[i] = NULL;
	}
	this->check[0] = 0;
	this->check[1] = 0;
}

MEAS::~MEAS()
{
}

void MEAS::StreamAnaylse(unsigned long len, char* RxBuffer)
{
	static int state = 0 , numOfObs = 0;
	std::string str = RxBuffer;
	//std::cout << "stream:  "<<state << "\t" << len << "\t" << RxBuffer << std::endl;
	//std::cout << "StreamAnaylse" << std::endl;
	int result = 0;

	for (int i = 0; i < len; i++)
	{
		switch (state)
		{
		case 0:
			check[0] = 0;
			check[1] = 0;
			result = findHead(RxBuffer + i);
			if ( result == FINDHEAD)
			{
				state++;
			}
			break;
		case 1:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = findID_Code(RxBuffer + i);
			if(result == FINDIDCODE)
			{
				state++;
			}
			break;
		case 2:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readLength(RxBuffer + i);
			if (result == GETLENGTH)
			{
				state++;
			}
			break;
		case 3:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readTime(RxBuffer + i);
			if (result == GETTIME)
			{
				state++;
				//std::cout << "\t" << "FIND HEAD" << std::endl;
				//std::cout << "\t" << "FIND ID CODE" << std::endl;
				//std::cout << "\t" << this->length << std::endl;
				//std::cout << "\t" << this->TOW << "\t" << this->WN << "\t" << (short)this->UTC << "\t" << (short)this->num << std::endl;
				//state = 0;//debug
			}
			break;
		case 4:
			check[0] += (uint8_t)*RxBuffer;
			check[1] += check[0];
			result = readobs(RxBuffer + i , numOfObs);
			if (result == GETOBS)
			{
				numOfObs++;
			}
			if (numOfObs >= this->num)
			{
				numOfObs = 0;
				state++;
			}
			break;
		case 5:
			if (check[0] == (uint8_t)*RxBuffer)
			{
				state++;
			}
			else
			{
				state = 0;
			}
			break;
		case 6:
			if (check[1] == (uint8_t)*RxBuffer)
			{
				std::cout << left;
				std::cout << "\t" << "FIND HEAD" << std::endl;
				std::cout << "\t" << "FIND ID CODE" << std::endl;
				std::cout << "\t" << this->length << std::endl;
				std::cout << "\t" << this->TOW << "\t" << this->WN << "\t" << (short)this->UTC << "\t" << (short)this->num << std::endl;
				cout << setw(12) << "GNSS CLASS" << setw(12) << "ID" << setw(12) << "CN0"
					<< setw(15) << "PR" << setw(15) << "carrier phase" << setw(12) << "Doppler"
					<< setw(12) << "lock time" << setw(12) << "Flag" << std::endl;
				for (int k = 0; k < this->num; k++)
				{
					switch (weixing[k]->gnss)
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
					std::cout << setw(12) << (short)this->weixing[k]->ID << setw(12) << (short)this->weixing[k]->CN0
						<< setw(15) << this->weixing[k]->L << setw(15) << this->weixing[k]->ph << setw(12) << this->weixing[k]->d
						<< setw(12) << this->weixing[k]->lockTime << setw(12) << (short)this->weixing[k]->flag << std::endl;
				}
				state = 0;
			}
			else
			{
				state = 0;
			}
			break;
		default:
			break;
		}
		if (!result && state < 5)   
		{
			state = 0;
			//std::cout << "\t" << "MEAS_error!!!" << std::endl;
		}
	}
}

int MEAS::findHead(char* RxBuffer) {
	static int state = 1;
	//std::cout << "\t" << state << "\t" << *RxBuffer << std::endl;
	switch (state)
	{
	case 1:
		if (*RxBuffer == 0x23)
		{
			state++;
		}
		else
		{
			state = 1;
			return 0;
		}
		break;
	case 2:
		if (*RxBuffer == 0x3E) {
			state++;
		}
		else
		{
			state = 1;
			return 0;
		}
		state = 1;
		return FINDHEAD;
		break;
	}
	return state;
}

int MEAS::readTime(char* RxBuffer)
{
	static int state = 1;
	static union
	{
		double result;
		uint8_t data[8];
	}TOW;

	static union 
	{
		uint16_t result;
		uint8_t data[2];
	}WN;

	static union 
	{
		int8_t result;
		uint8_t data;
	}UTC;

	static union 
	{
		uint8_t result;
		uint8_t data;
	}num;


	if (state < GETTIME)
	{
		if (state <= 8)
		{
			TOW.data[state - 1] = *RxBuffer;
		}
		else if (state <= 10)
		{
			WN.data[state - 8] = *RxBuffer;
		}
		else if (state <= 11)
		{
			UTC.data = *RxBuffer;
		}
		else if (state <= 12)
		{
			num.data = *RxBuffer;
		}
		state++;
	}
	//this->leftlen--;
	if (state >= GETTIME)
	{
		state = 1;
		this->TOW = TOW.result;
		this->WN = WN.result;
		this->UTC = UTC.result;
		this->num = num.result;
		return GETTIME;
	}
	else
	{
		return state;
	}

}

int MEAS::readobs(char* RxBuffer , int num )
{
	static int state = 1;
	static union
	{
		double result;
		uint8_t data[8];
	}L{ 0 };

	static union 
	{
		double result;
		uint8_t data[8];
	}ph{ 0 };

	static union 
	{
		float result;
		uint8_t data[4];
	}d{ 0 };

	static union 
	{
		uint16_t result;
		uint8_t data[2];
	}lockTime{ 0 };

	if (!this->weixing[num])
	{
		this->weixing[num] = new Obs();
	}
	if (state < GETOBS)
	{
		// 读取数据
		if (state == 1)
		{
			this->weixing[num]->gnss = *RxBuffer;
		}
		else if (state == 2)
		{
			this->weixing[num]->ID = *RxBuffer;
		}
		else if (state == 3)
		{
			this->weixing[num]->CN0 = *RxBuffer;
		}
		else if (state <= 11)
		{
			L.data[state - 4] = *RxBuffer;
			if (state == 11)
			{
				this->weixing[num]->L = L.result;
			}
		}
		else if (state <= 19)
		{
			ph.data[state - 12] = *RxBuffer;
			if (state == 19)
			{
				this->weixing[num]->ph = ph.result;
			}
		}
		else if (state <= 23)
		{
			d.data[state - 20] = *RxBuffer;
			if (state == 23)
			{
				this->weixing[num]->d = d.result;
			}
		}
		else if (state <= 25 )
		{
			lockTime.data[state - 24] = *RxBuffer;
			if (state == 25)
			{
				this->weixing[num]->lockTime = lockTime.result;
			}
		}
		else if (state == 26)
		{
			this->weixing[num]->flag = *RxBuffer;
		}
		state++;
	}
	this->leftlen--;
	if (state >= GETOBS)
	{
		state = 1;
		
		// 数据转换

		return GETOBS;
	}
	else
	{
		return state;
	}
	return 0;
}

int MEAS::findID_Code(char* RxBuffer)
{
	static int state = 1;
//	std::cout << "\t" << state << "\t" << *RxBuffer << std::endl;
	switch (state)
	{
	case 1:
		if (*RxBuffer == 0x06)
		{
			state++;
		}
		else
		{
			state = 1;
			return 0;
		}
		break;
	case 2:
		if (*RxBuffer == 0x09) {
			state++;
		}
		else
		{
			state = 1;
			return 0;
		}
		state = 1;
		return FINDIDCODE;
		break;
	}
	return state;
}

int MEAS::readLength(char* RxBuffer)
{
	static int state = 1;
	static union
	{
		uint16_t a;
		uint8_t data[2];
	}result;

	if (state < GETLENGTH)
	{
		result.data[state-1] = *RxBuffer;
		state++;
	}
	if (state >= GETLENGTH)
	{
		state = 1;
		this->length = result.a;
		this->leftlen = this->length;
		if (this->length > 952)
		{
			return 0;
		}
		else
		{
			return GETLENGTH;
		}
	}
	else
	{
		return state;
	}

}
