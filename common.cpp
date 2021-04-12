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

bool Common::BDST2EPH(double TOW, uint16_t WN,double *ep)
{
    gtime_t ttemp;
    ttemp =  bdt2time((int)WN, TOW);
    time2epoch(ttemp, ep);

	return 1;
}




/* beidou time (bdt) to time ---------------------------------------------------
* convert week and tow in beidou time (bdt) to gtime_t struct
* args   : int    week      I   week number in bdt
*          double sec       I   time of week in bdt (s)
* return : gtime_t struct
*-----------------------------------------------------------------------------*/
gtime_t Common::bdt2time(int week, double sec)
{
    gtime_t t = epoch2time(bdt0);

    if (sec < -1E9 || 1E9 < sec) sec = 0.0;
    t.time += 86400 * 7 * week + (int)sec; //从1970 1 1开始的总秒数
    t.sec = sec - (int)sec;
    return t;
}

/* convert calendar day/time to time -------------------------------------------
* convert calendar day/time to gtime_t struct
* args   : double *ep       I   day/time {year,month,day,hour,min,sec}
* return : gtime_t struct
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
gtime_t Common::epoch2time(const double* ep)
{
    const int doy[] = { 1,32,60,91,121,152,182,213,244,274,305,335 }; //平年的每个月的首月是该年的第几天，但是这一天只是个标记，要减去
    gtime_t time = { 0 };
    int days, sec, year = (int)ep[0], mon = (int)ep[1], day = (int)ep[2];

    if (year < 1970 || 2099 < year || mon < 1 || 12 < mon) return time;

    // leap year if year%4==0 in 1901-2099   //这里只有一个世纪闰年即2000年，是闰年，而1900这些100倍数且被4整除的不是闰年
    days = (year - 1970) * 365 + (year - 1969) / 4 + doy[mon - 1] + day - 2 + (year % 4 == 0 && mon >= 3 ? 1 : 0);
    //days = 从1970年开始，每年基础的365天 + 该年有多少个四年就多多少个1天 + 该月的1号在当年第几天 - 这个不应该算的首月第一天 + 当月的第几天 - 由于这天没有过去所以减1 + 该年是否为闰年，如果过了2月，就多1天
    //从1970年1月1日0时记起为第0天！
    sec = (int)floor(ep[5]); //取秒的整数部分
    time.time = (time_t)days * 86400 + (int)ep[3] * 3600 + (int)ep[4] * 60 + sec; //是从1970年xx月xx日开始算起的秒数
    time.sec = ep[5] - sec; //取秒的小数部分
    return time;
}



/* time to gps time ------------------------------------------------------------
* convert gtime_t struct to week and tow in gps time
* args   : gtime_t t        I   gtime_t struct
*          int    *week     IO  week number in gps time (NULL: no output)
* return : time of week in gps time (s)
*-----------------------------------------------------------------------------*/
double Common::time2gpst(gtime_t t, int* week)
{
    gtime_t t0 = epoch2time(gpst0);
    time_t sec = t.time - t0.time;
    int w = (int)(sec / (86400 * 7));

    if (week) *week = w;
    return (double)(sec - w * 86400 * 7) + t.sec;
}



/* gpstime to utc --------------------------------------------------------------
* convert gpstime to utc considering leap seconds
* args   : gtime_t t        I   time expressed in gpstime
* return : time expressed in utc
* notes  : ignore slight time offset under 100 ns
*-----------------------------------------------------------------------------*/

gtime_t Common::gpst2utc(gtime_t t)
{
    gtime_t tu;
    int i;

    for (i = 0; i < (int)sizeof(leaps) / (int)sizeof(*leaps); i++) {
        tu = timeadd(t, leaps[i][6]);
        if (timediff(tu, epoch2time(leaps[i])) >= 0.0) return tu;
    }
    return t;
}


/* add time --------------------------------------------------------------------
* add time to gtime_t struct
* args   : gtime_t t        I   gtime_t struct
*          double sec       I   time to add (s)
* return : gtime_t struct (t+sec)
*-----------------------------------------------------------------------------*/

gtime_t Common::timeadd(gtime_t t, double sec)
{
    double tt;

    t.sec += sec; tt = floor(t.sec); t.time += (int)tt; t.sec -= tt;
    return t;
}


/* time difference -------------------------------------------------------------
* difference between gtime_t structs
* args   : gtime_t t1,t2    I   gtime_t structs
* return : time difference (t1-t2) (s)
*-----------------------------------------------------------------------------*/
double Common::timediff(gtime_t t1, gtime_t t2)
{
    return difftime(t1.time, t2.time) + t1.sec - t2.sec; //difftime为C库的标准函数，只计算秒的差别
}


/* time to calendar day/time ---------------------------------------------------
* convert gtime_t struct to calendar day/time
* args   : gtime_t t        I   gtime_t struct
*          double *ep       O   day/time {year,month,day,hour,min,sec}
* return : none
* notes  : proper in 1970-2037 or 1970-2099 (64bit time_t)
*-----------------------------------------------------------------------------*/
void Common::time2epoch(gtime_t t, double* ep)
{
    const int mday[] = { /* # of days in a month */
        31,28,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31,
        31,29,31,30,31,30,31,31,30,31,30,31,31,28,31,30,31,30,31,31,30,31,30,31
    };
    int days, sec, mon, day;

    /* leap year if year%4==0 in 1901-2099 */
    days = (int)(t.time / 86400);
    sec = (int)(t.time - (time_t)days * 86400);
    for (day = days % 1461, mon = 0; mon < 48; mon++) {
        if (day >= mday[mon]) day -= mday[mon]; else break;
    }
    ep[0] = 1970 + days / 1461 * 4 + mon / 12; ep[1] = mon % 12 + 1; ep[2] = day + 1;
    ep[3] = sec / 3600; ep[4] = sec % 3600 / 60; ep[5] = sec % 60 + t.sec;
}

