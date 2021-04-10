#include"printNAV.h"
#include <iostream>
#include <iomanip>

using namespace std;

printNAV::printNAV()
{
}

printNAV::~printNAV()
{
}


/**********************************************************************
* Function		: printNAVdata1dot12
* Description	: 打印1个double数据  如-1.075677573680e-07 总长度19
* Reference		: 根据RINEX305/304混合导航文件打印格式
**********************************************************************/
void printNAV::printNAVdata1dot12(double data) 
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
void printNAV::printPRN(double data)
{
	std::cout << left;

	if (data < 10)
	{
		std::cout << setw(1) << "0";
		std::cout << setw(2) << (short)data;
	}
	else
	{
		std::cout << setw(3) << (short)data;
	}
}