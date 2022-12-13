#include "SerialPort.h"
#include <stdio.h>

//byte SerialPort::detectedPorts[256];

SerialPort::SerialPort() {
	//ZeroMemory(&Overlapped, sizeof(OVERLAPPED));
};
SerialPort::~SerialPort() {};

int SerialPort::scanPorts(unsigned int *portList,int iLastPort) {
	char cNameBuf[32];
	int  iDummyCount = 0;
	HANDLE hDummyHandle;

	int n = 0;
	int count = 0;
	for (n = 0; n < iLastPort; n++) {
		iDummyCount = sprintf_s(cNameBuf, 30, "/COM%i", n);
		hDummyHandle = CreateFileA(cNameBuf, (GENERIC_READ | GENERIC_WRITE), 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
		if (hDummyHandle != INVALID_HANDLE_VALUE) {
			CloseHandle(hDummyHandle);
			//Append Successull Portnumber to List
			portList[count] = n;
			count++;
		}; // if Error
	}; // for
	return count;
}

bool SerialPort::begin(int iPortNum, DWORD dwBaudRate) {
	char cNameBuf[32];
	int  iCount;

	iCount = sprintf_s(cNameBuf, 30, "/COM%i", iPortNum);
	
	//portHandle = CreateFile("/COM5", GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	portHandle = CreateFileA(cNameBuf, (GENERIC_READ | GENERIC_WRITE), 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0); //FILE_FLAG_OVERLAPPED is used for Assyncron access.
	if (portHandle == INVALID_HANDLE_VALUE) {
		DWORD dwLastError = GetLastError();
		return false;
	}; // if Error

	portSettings.DCBlength = sizeof(DCB);
	portSettings.BaudRate = dwBaudRate;//CBR_19200;
	portSettings.ByteSize = 8;
	portSettings.StopBits = ONESTOPBIT;
	portSettings.Parity = NOPARITY;
	SetCommState(portHandle, &portSettings);
	return true;
}

int  SerialPort::read(bool bBlocking = false) {
	unsigned char ucBuffer[2];
	DWORD dwReadCount;
	bool  bSuccess;
	//TODO bBlocking not implemented yet.
	//if (bBlocking) Overlapped.hEvent = NULL;
	bSuccess = ReadFile(portHandle, &ucBuffer, (DWORD)1, &dwReadCount, NULL);
	if (bSuccess == false) return -1;
	return (int)(unsigned char)ucBuffer[0];
};

bool SerialPort::write(unsigned char ucChar) {
	unsigned char ucBuffer[2];
	DWORD dwReadCount;
	bool  bSuccess;
	//TODO bBlocking not implemented yet.
	//if (bBlocking) Overlapped.hEvent = NULL;
	bSuccess = WriteFile(portHandle, &ucBuffer, (DWORD)1, &dwReadCount, NULL);
	if (bSuccess == false) {
		DWORD dwError = GetLastError();
		return false;
	};
	return true;
};

void SerialPort::close() {
	CloseHandle(portHandle);
}