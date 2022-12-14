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


bool SerialPort::seekIntro(uint32_t ui32Intro, int iIntroSize) {
	uint32_t ui32Buffer = 0x00;
	int iInput;
	while (ui32Intro != ui32Buffer) {
		iInput = read();
		if (iInput < 0) return false;
		ui32Buffer = (ui32Buffer << 8) | (uint32_t)iInput;
		switch (iIntroSize) {
		case 1: ui32Buffer = ui32Buffer & 0x000000FF; break;
		case 2: ui32Buffer = ui32Buffer & 0x0000FFFF; break;
		case 3: ui32Buffer = ui32Buffer & 0x00FFFFFF; break;
		case 4: ui32Buffer = ui32Buffer & 0xFFFFFFFF; break;
		}; // switch iIntroSize
	}; // while
	return true;
};


int32_t SerialPort::readUInt(int iSize) {
	if (iSize > 4) return 0;
	if (iSize < 1) return 0;
	int32_t   i32retValue = 0x00;
	uint32_t ui32retValue = 0x00;
	int iInput;
	for (int n = 0; n < iSize; n++) {
		iInput = read();
		if (iInput < 0) return 0;
		ui32retValue = ui32retValue >> 8;
		ui32retValue = ui32retValue & 0x00FFFFFF;
		ui32retValue = ui32retValue | (uint32_t)(iInput << 24);
	};
	ui32retValue = ui32retValue >> (4 - iSize) * 8;
	i32retValue = ui32retValue;//reinterpret_cast<int32_t>(ui32retValue);
	return ui32retValue;
};

int32_t SerialPort::readInt(int iSize) {
	uint32_t ui32t = readUInt(iSize);
	uint32_t msbSign = 0x80 << ((iSize - 1) * 8);	// Vorzeichenbit/(Wert) ermitteln
	int32_t  i32t = ui32t;
	if (ui32t >= msbSign) {
		switch (iSize) {
		case 1: i32t = ui32t | 0xFFFFFF80; break;
		case 2: i32t = ui32t | 0xFFFF8000; break;
		case 3: i32t = ui32t | 0xFF800000; break;
		case 4: i32t = ui32t | 0x80000000; break;
		}
	}; // if Sign bit is set
	return i32t;
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