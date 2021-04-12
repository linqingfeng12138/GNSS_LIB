#include"printF.h"
#include "common.h"
#include <iostream>
#include <iomanip>

using namespace std;

printF::printF()
{
}

printF::~printF()
{
}


/**********************************************************************
* Function		: printNAVdata1dot12
* Description	: 打印1个double数据  如-1.075677573680e-07 总长度19
* Reference		: 根据RINEX305/304混合导航文件打印格式
**********************************************************************/
void printF::printNAVdata1dot12(double data) 
{
	std::cout << left;

	if (data < 0)
	{
		std::cout << setw(1) << "-";
		std::cout << setw(18) << scientific << std::setprecision(12) << -data;
	}
	else
	{
		std::cout << setw(1) << " ";
		std::cout << setw(18) << scientific << std::setprecision(12) << data;
	}
}

/**********************************************************************
* Function		: printPRN
* Description	: 打印PRN  如G05
* Reference		: 根据RINEX305/304混合导航文件打印格式
**********************************************************************/
void printF::printPRN(double data)
{
	print0ahead(data, 2);
	std::cout << setw(1) << " ";

}

/**********************************************************************
* Function		: print0ahead
* Description	: 打印!!整数!!前面补齐0  如 05  002 0189
*       		: digits：需要打印的含0位数 
**********************************************************************/
bool printF::print0ahead(double data, uint8_t digits)
{
    //判断data位数
    uint8_t d;
    uint8_t i;
    uint64_t temp = data;
    for (d = 0; fabs(temp) > 0; d++)
    {
        temp = temp / 10;
    }
    //无法打印比原数还少的位
    if (data == 0.0)
    {
        d = 1;
    }
    if (d > digits)
    {
        return 0;
    }
    else if (d == digits)
    {
        std::cout << setw(digits) << (short)data;
        return 1;
    }
    else
    {
        for (i = 0; i < (digits - d); i++)
        {
            std::cout << setw(1) << "0";
        }
        std::cout << setw(d) << (short)data;
        return 1;
    }
}


bool printF::printNavEpoch(double *ep)
{
    std::cout << setw(4) << (short)ep[0]; //年
    std::cout << setw(1) << " ";
    print0ahead(ep[1], 2); //月
    std::cout << setw(1) << " ";
    print0ahead(ep[2], 2); //日
    std::cout << setw(1) << " ";
    print0ahead(ep[3], 2); //时
    std::cout << setw(1) << " ";
    print0ahead(ep[4], 2); //分
    std::cout << setw(1) << " ";
    print0ahead(ep[5], 2); //秒

    return 1;
}

bool printF::printObsEpoch(double* ep)
{
    std::cout << setw(5) << (short)ep[0]; //年
    print0ahead(ep[1], 2); //月
    std::cout << setw(1) << " ";
    print0ahead(ep[2], 2); //日
    std::cout << setw(1) << " ";
    print0ahead(ep[3], 2); //时
    std::cout << setw(1) << " ";
    print0ahead(ep[4], 2); //分
    std::cout << setw(1) << " ";
    //std::cout << setw(11) << setiosflags(ios::fixed) << setprecision(7) << ep[5]; //秒
    std::cout << setw(11)  << ep[5]; //秒

    
    return 1;
}