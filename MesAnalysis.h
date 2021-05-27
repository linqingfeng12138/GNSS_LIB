#pragma once

#include <stdint.h>
#include "GNSSANL.h"
#include "MEAS.h"
#include "GPSEPH.h"
#include "GPSIONO.h"
class MesAnalysis : GNSSANL
{
public:
	int StreamAnaylse(unsigned long len, char* RxBuffer);

private:
	uint8_t check[2] = { 0 };		//У���

	uint16_t ID;

	int findID_Code(char* RxBuffer);

	int findHead(char* RxBuffer);

	int readChkSum(char* RxBuffer);
	
	enum Status_t			//����״ֵ̬
	{
		GOTHEAD = 1,
		GOTID,
		GOTLENGTH,
		GOTTIME,
		GOTOBS
	};


};

