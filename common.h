#pragma once
#include<stdint.h>
#include<corecrt.h>

class Common
{
public:
	Common();
	~Common();

	bool BDST2UTC(double TOW,uint16_t WN,uint8_t yy, uint8_t mm, uint8_t dd, uint8_t hh);
private:

	struct {        /* time struct */
		time_t time;        /* time (s) expressed by standard time_t */
		double sec;         /* fraction of second under 1 s */
	} gtime_t;


	//gtime_t bdt2time(int week, double sec);
};

