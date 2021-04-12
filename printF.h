#pragma once

#include<stdint.h>

class printF
{
public:
	printF();
	~printF();

	void printNAVdata1dot12(double data);
	void printPRN(double data);
	bool print0ahead(double data, uint8_t digits);
	bool printNavEpoch(double *ep);
	bool printObsEpoch(double* ep);
private:


};

