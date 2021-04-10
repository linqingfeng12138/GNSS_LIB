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
* Description	: ��ӡ1��double����  ��-1.075677573680e-07 �ܳ���19
* Reference		: ����RINEX305/304��ϵ����ļ���ӡ��ʽ
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
* Description	: ��ӡPRN  ��G05
* Reference		: ����RINEX305/304��ϵ����ļ���ӡ��ʽ
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