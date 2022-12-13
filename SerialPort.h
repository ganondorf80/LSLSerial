#pragma once
#include <windows.h>

class SerialPort
{
private:
	DCB portSettings;
	//static byte detectedPorts[256];
	HANDLE portHandle = 0;
	//OVERLAPPED Overlapped; //Used for Async File Access, if opend with FILE_FLAG_OVERLAPPED
public:
	SerialPort();
	~SerialPort();
	static int scanPorts(unsigned int *portList,int iLastPort); //returns number of elements in portList. portList is handled by Class instance
	bool begin(int iPortNum, DWORD dwBaudRate);
	int  read(bool bBlocking);
	bool write(unsigned char ucChar);
	void close();
};

