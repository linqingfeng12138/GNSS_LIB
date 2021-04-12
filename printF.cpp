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
* Description	: ��ӡ1��double����  ��-1.075677573680e-07 �ܳ���19
* Reference		: ����RINEX305/304��ϵ����ļ���ӡ��ʽ
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
* Description	: ��ӡPRN  ��G05
* Reference		: ����RINEX305/304��ϵ����ļ���ӡ��ʽ
**********************************************************************/
void printF::printPRN(double data)
{
	print0ahead(data, 2);
	std::cout << setw(1) << " ";

}

/**********************************************************************
* Function		: print0ahead
* Description	: ��ӡ!!����!!ǰ�油��0  �� 05  002 0189
*       		: digits����Ҫ��ӡ�ĺ�0λ�� 
**********************************************************************/
bool printF::print0ahead(double data, uint8_t digits)
{
    //�ж�dataλ��
    uint8_t d;
    uint8_t i;
    uint64_t temp = data;
    for (d = 0; fabs(temp) > 0; d++)
    {
        temp = temp / 10;
    }
    //�޷���ӡ��ԭ�����ٵ�λ
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
    std::cout << setw(4) << (short)ep[0]; //��
    std::cout << setw(1) << " ";
    print0ahead(ep[1], 2); //��
    std::cout << setw(1) << " ";
    print0ahead(ep[2], 2); //��
    std::cout << setw(1) << " ";
    print0ahead(ep[3], 2); //ʱ
    std::cout << setw(1) << " ";
    print0ahead(ep[4], 2); //��
    std::cout << setw(1) << " ";
    print0ahead(ep[5], 2); //��

    return 1;
}

bool printF::printObsEpoch(double* ep)
{
    std::cout << setw(5) << (short)ep[0]; //��
    print0ahead(ep[1], 2); //��
    std::cout << setw(1) << " ";
    print0ahead(ep[2], 2); //��
    std::cout << setw(1) << " ";
    print0ahead(ep[3], 2); //ʱ
    std::cout << setw(1) << " ";
    print0ahead(ep[4], 2); //��
    std::cout << setw(1) << " ";
    //std::cout << setw(11) << setiosflags(ios::fixed) << setprecision(7) << ep[5]; //��
    std::cout << setw(11)  << ep[5]; //��

    
    return 1;
}