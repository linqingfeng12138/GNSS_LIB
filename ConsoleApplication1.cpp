// ConsoleApplication1.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include <cstdlib>
#include <windows.h>
#include "MEAS.h"
#include "GPSEPH.h"
#include "BDSEPH.h"

HANDLE hComm;
OVERLAPPED OverLapped;
COMSTAT Comstat;
DWORD dwCommEvents;

bool OpenPort();  //打开串口
bool SetupDCB(int rate_arg);  //设置DCB
bool SetupTimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD
	ReadTotalConstant, DWORD WriteTotalMultiplier, DWORD WriteTotalConstant);   //设置超时
void ReciveChar();   //接收字符
bool WriteChar(char* szWriteBuffer, DWORD dwSend);  //发送字符

bool OpenPort()
{
	hComm = CreateFile(L"COM3",
		GENERIC_READ | GENERIC_WRITE,
		0,
		0,
		OPEN_EXISTING,
		FILE_FLAG_OVERLAPPED,
		0);
	if (hComm == INVALID_HANDLE_VALUE)
		return FALSE;
	else
		return true;
}

bool SetupDCB(int rate_arg)
{
	DCB dcb;
	memset(&dcb, 0, sizeof(dcb));
	if (!GetCommState(hComm, &dcb))//获取当前DCB配置
	{
		return FALSE;
	}
	dcb.DCBlength = sizeof(dcb);
	/* ---------- Serial Port Config ------- */
	dcb.BaudRate = rate_arg;
	dcb.Parity = NOPARITY;
	dcb.fParity = 0;
	dcb.StopBits = ONESTOPBIT;
	dcb.ByteSize = 8;
	dcb.fOutxCtsFlow = 0;
	dcb.fOutxDsrFlow = 0;
	dcb.fDtrControl = DTR_CONTROL_DISABLE;
	dcb.fDsrSensitivity = 0;
	dcb.fRtsControl = RTS_CONTROL_DISABLE;
	dcb.fOutX = 0;
	dcb.fInX = 0;
	dcb.fErrorChar = 0;
	dcb.fBinary = 1;
	dcb.fNull = 0;
	dcb.fAbortOnError = 0;
	dcb.wReserved = 0;
	dcb.XonLim = 2;
	dcb.XoffLim = 4;
	dcb.XonChar = 0x13;
	dcb.XoffChar = 0x19;
	dcb.EvtChar = 0;
	if (!SetCommState(hComm, &dcb))
	{
		return false;
	}
	else
		return true;
}

bool SetupTimeout(DWORD ReadInterval, DWORD ReadTotalMultiplier, DWORD
	ReadTotalConstant, DWORD WriteTotalMultiplier, DWORD WriteTotalConstant)
{
	COMMTIMEOUTS timeouts;
	timeouts.ReadIntervalTimeout = ReadInterval;
	timeouts.ReadTotalTimeoutConstant = ReadTotalConstant;
	timeouts.ReadTotalTimeoutMultiplier = ReadTotalMultiplier;
	timeouts.WriteTotalTimeoutConstant = WriteTotalConstant;
	timeouts.WriteTotalTimeoutMultiplier = WriteTotalMultiplier;
	if (!SetCommTimeouts(hComm, &timeouts))
	{
		return false;
	}
	else
		return true;
}



void ReciveChar()
{
	bool bRead = TRUE;
	bool bResult = TRUE;
	DWORD dwError = 0;
	DWORD BytesRead = 0;
	char RXBuff[100] = { 0 };
	static MEAS msg;
	static GPSEPH msg2;
	static BDSEPH msg3;
	for (;;)
	{
		bResult = ClearCommError(hComm, &dwError, &Comstat);
		if (Comstat.cbInQue == 0)
			continue;
		if (bRead)
		{
			bResult = ReadFile(hComm,		//通信设备（此处为串口）句柄，由CreateFile()返回值得到
				RXBuff,						//指向接收缓冲区
				1,							//指明要从串口中读取的字节数
				&BytesRead,					//
				&OverLapped);				//OVERLAPPED结构
			//std::cout << RXBuff;
			if ((bResult && BytesRead))
			{
				msg.StreamAnaylse(BytesRead, RXBuff);
				//msg2.StreamAnaylse(BytesRead, RXBuff);
				//msg3.StreamAnaylse(BytesRead, RXBuff);
			}
			if (!bResult)
			{
				switch (dwError = GetLastError())
				{
				case ERROR_IO_PENDING:
					bRead = FALSE;
					break;
				default:
					break;
				}
			}
		}
		else
		{
			bRead = TRUE;
		}
	}
	if (!bRead)
	{
		bRead = TRUE;
		bResult = GetOverlappedResult(hComm,
			&OverLapped,
			&BytesRead,
			TRUE);
	}
}

bool WriteChar(const char* szWriteBuffer, DWORD dwSend)
{
	bool bWrite = TRUE;
	bool bResult = TRUE;
	DWORD BytesSent = 0;
	HANDLE hWriteEvent = NULL;
	ResetEvent(hWriteEvent);
	if (bWrite)
	{
		OverLapped.Offset = 0;
		OverLapped.OffsetHigh = 0;
		bResult = WriteFile(hComm,		//通信设备句柄，CreateFile()返回值得到
			szWriteBuffer,				//指向写入数据缓冲区
			dwSend,						 //设置要写的字节数
			&BytesSent,					//
			&OverLapped);				//指向异步I/O数据
		if (!bResult)
		{
			DWORD dwError = GetLastError();
			switch (dwError)
			{
			case ERROR_IO_PENDING:
				BytesSent = 0;
				bWrite = FALSE;
				break;
			default:
				break;
			}
		}
	}
	if (!bWrite)
	{
		bWrite = TRUE;
		bResult = GetOverlappedResult(hComm,
			&OverLapped,
			&BytesSent,
			TRUE);
		if (!bResult)
		{
			std::cout << "GetOverlappedResults() in WriteFile()" << std::endl;
		}
	}
	if (BytesSent != dwSend)
	{
		std::cout << "WARNING: WriteFile() error.. Bytes Sent:" << BytesSent << "; Message Length: " << strlen((char*)szWriteBuffer) << std::endl;
	}
	return TRUE;
}
int main()
{
	if (OpenPort())
		std::cout << "Open port success" << std::endl;
	if (SetupDCB(9600))
		std::cout << "Set DCB success" << std::endl;
	if (SetupTimeout(0, 0, 0, 0, 0))
		std::cout << "Set timeout success" << std::endl;
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR | PURGE_RXABORT | PURGE_TXABORT);
	//WriteChar("Please send data:", 20);
	std::cout << "Received data:";
	ReciveChar();
	return EXIT_SUCCESS;
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
