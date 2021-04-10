#include"common.h"
#include <iostream>
#include <iomanip>

using namespace std;
Common::Common()
{
}

Common::~Common()
{
}

bool Common::BDST2UTC(double TOW, uint16_t WN, uint8_t yy, uint8_t mm, uint8_t dd, uint8_t hh)
{

	return 1;
}

/* beidou time (bdt) to time ---------------------------------------------------
* convert week and tow in beidou time (bdt) to gtime_t struct
* args   : int    week      I   week number in bdt
*          double sec       I   time of week in bdt (s)
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
/*
gtime_t Common::bdt2time(int week, double sec)
{
    gtime_t t = epoch2time(bdt0);

    if (sec < -1E9 || 1E9 < sec) sec = 0.0;
    t.time += 86400 * 7 * week + (int)sec;
    t.sec = sec - (int)sec;
    return t;
}
*/

/* time to gps time ------------------------------------------------------------
* convert gtime_t struct to week and tow in gps time
* args   : gtime_t t        I   gtime_t struct
*          int    *week     IO  week number in gps time (NULL: no output)
* return : time of week in gps time (s)
*-----------------------------------------------------------------------------*/
/*extern double time2gpst(gtime_t t, int* week)
{
    gtime_t t0 = epoch2time(gpst0);
    time_t sec = t.time - t0.time;
    int w = (int)(sec / (86400 * 7));

    if (week) *week = w;
    return (double)(sec - w * 86400 * 7) + t.sec;
}
*/

/* gpstime to utc --------------------------------------------------------------
* convert gpstime to utc considering leap seconds
* args   : gtime_t t        I   time expressed in gpstime
* return : time expressed in utc
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/
/*
extern gtime_t gpst2utc(gtime_t t)
{
    gtime_t tu;
    int i;

    for (i = 0; i < (int)sizeof(leaps) / (int)sizeof(*leaps); i++) {
        tu = timeadd(t, leaps[i][6]);
        if (timediff(tu, epoch2time(leaps[i])) >= 0.0) return tu;
    }
    return t;
}
*/