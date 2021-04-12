#pragma once
#include<stdint.h>
#include<corecrt.h>

typedef struct {        /* time struct */
	time_t time;        /* time (s) expressed by standard time_t */
	double sec;         /* fraction of second under 1 s */
} gtime_t;

const static double bdt0[] = { 2006,1, 1,0,0,0 }; /* beidou time reference */
const static double gpst0[] = { 1980,1, 6,0,0,0 }; /* gps time reference */
const static double leaps[][7] = { /* leap seconds {y,m,d,h,m,s,utc-gpst,...} */
    {2012,7,1,0,0,0,-16},
    {2009,1,1,0,0,0,-15},
    {2006,1,1,0,0,0,-14},
    {1999,1,1,0,0,0,-13},
    {1997,7,1,0,0,0,-12},
    {1996,1,1,0,0,0,-11},
    {1994,7,1,0,0,0,-10},
    {1993,7,1,0,0,0, -9},
    {1992,7,1,0,0,0, -8},
    {1991,1,1,0,0,0, -7},
    {1990,1,1,0,0,0, -6},
    {1988,1,1,0,0,0, -5},
    {1985,7,1,0,0,0, -4},
    {1983,7,1,0,0,0, -3},
    {1982,7,1,0,0,0, -2},
    {1981,7,1,0,0,0, -1}
};

class Common
{
public:
	Common();
	~Common();

    double ep[6];
	bool BDST2EPH(double TOW,uint16_t WN,double *ep);
    gtime_t epoch2time(const double* ep);
    void time2epoch(gtime_t t, double* ep);
    gtime_t timeadd(gtime_t t, double sec);

private:
	


    gtime_t time;
	gtime_t bdt2time(int week, double sec);
    
    double time2gpst(gtime_t t, int* week);
    gtime_t gpst2utc(gtime_t t);

    double timediff(gtime_t t1, gtime_t t2);

   

};



