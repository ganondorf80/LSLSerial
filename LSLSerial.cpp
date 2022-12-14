// LSLSerial.cpp : Diese Datei enthält die Funktion "main". Hier beginnt und endet die Ausführung des Programms.
//

#pragma comment (lib,"lsl.dll")
//#pragma comment (lib,"lsl.lib")
#include <lsl_cpp.h>

#include <iostream>
#include <exception>
#include <cstdio> //printf
#include "SerialPort.h"
#include "CmdParameters.h"

enum CT {
    CT_UINT8, CT_INT8,
    CT_UINT16, CT_INT16,
    CT_UINT24, CT_INT24,
    CT_UINT32, CT_INT32,
};

struct channelInfo {
    int iChannelId;
    CT  ctChannelType;
    int iSize;
    float fValue;
};

std::vector<channelInfo> channels;
channelInfo chan;

#define MAX_CHANNELS 64
#define LAST_PORT 255

std::string strLslName;
std::string strLslType;
int iLslFrameBoarderSize;
uint32_t ui32FrameBorder;

float fLslChannels[MAX_CHANNELS];
int iRequestedPort = -1;
int iRequestedRate = -1;

int iLslChannelCount;
SerialPort Serial;
CmdParameters cmd;
std::string strOutMissing;

int main(int argc, char* argv[])
{
    cmd.init(argc, argv);
    strLslName = cmd.getArg("-streamname");
    strLslType = cmd.getArg("-streamtype");
    
    iLslFrameBoarderSize = 0; 
    
    try {
        if (std::stoi(cmd.getArg("-fb8")) != 0) { iLslFrameBoarderSize = 1; ui32FrameBorder = std::stoi(cmd.getArg("-fb8")); };
    }
    catch (std::invalid_argument e) {};
    try {
        if (std::stoi(cmd.getArg("-fb16")) != 0) { iLslFrameBoarderSize = 2; ui32FrameBorder = std::stoi(cmd.getArg("-fb16")); };
    }
    catch (std::invalid_argument e) {};
    try {
        if (std::stoi(cmd.getArg("-fb24")) != 0) { iLslFrameBoarderSize = 3; ui32FrameBorder = std::stoi(cmd.getArg("-fb24")); };
    }
    catch (std::invalid_argument e) {};
    try {
        if (std::stoi(cmd.getArg("-fb32")) != 0) { iLslFrameBoarderSize = 4; ui32FrameBorder = std::stoi(cmd.getArg("-fb32")); };
    }
    catch (std::invalid_argument e) {};

    try {
        if (std::stoi(cmd.getArg("-s")) > -1) { iRequestedPort = std::stoi(cmd.getArg("-s")); };
    }
    catch (std::invalid_argument e) {};

    try {
        if (std::stoi(cmd.getArg("-b")) > -1) { iRequestedRate = std::stoi(cmd.getArg("-b")); };
    }
    catch (std::invalid_argument e) {};


//    if (cmd.has("-fb16")) iLslFrameBoarderSize = 2;
//    if (cmd.has("-fb24")) iLslFrameBoarderSize = 3;
//    if (cmd.has("-fb32")) iLslFrameBoarderSize = 4;
    iLslChannelCount = 0;
    std::string strChanId;
    //std::string strChanType;
    std::string strChanName;
    for (int n = 0; n < MAX_CHANNELS; n++) {
        strChanId = "-c" + std::to_string(n);
        if (cmd.has(strChanId + "ui8")) { chan.ctChannelType = CT::CT_UINT8;   chan.iChannelId = n; chan.iSize = 1; channels.push_back(chan); iLslChannelCount++; };
        if (cmd.has(strChanId + "i8"))   { chan.ctChannelType = CT::CT_INT8;   chan.iChannelId = n; chan.iSize = 1; channels.push_back(chan); iLslChannelCount++; };
        if (cmd.has(strChanId + "ui16")) { chan.ctChannelType = CT::CT_UINT16; chan.iChannelId = n; chan.iSize = 2; channels.push_back(chan); iLslChannelCount++; };
        if (cmd.has(strChanId + "i16"))  { chan.ctChannelType = CT::CT_INT16;  chan.iChannelId = n; chan.iSize = 2; channels.push_back(chan); iLslChannelCount++; };
        if (cmd.has(strChanId + "ui24")) { chan.ctChannelType = CT::CT_UINT24; chan.iChannelId = n; chan.iSize = 3; channels.push_back(chan); iLslChannelCount++; };
        if (cmd.has(strChanId + "i24"))  { chan.ctChannelType = CT::CT_INT24;  chan.iChannelId = n; chan.iSize = 3; channels.push_back(chan); iLslChannelCount++; };
        if (cmd.has(strChanId + "ui32")) { chan.ctChannelType = CT::CT_UINT32; chan.iChannelId = n; chan.iSize = 4; channels.push_back(chan); iLslChannelCount++; };
        if (cmd.has(strChanId + "i32"))  { chan.ctChannelType = CT::CT_INT32;  chan.iChannelId = n; chan.iSize = 4; channels.push_back(chan); iLslChannelCount++; };
    }; // for n

    if (strLslName == "") strOutMissing += "-streamname \"<Name>\"\r\n";
    if (strLslType == "") strOutMissing += "-streamtype \"<Type>\"\r\n";
    if (iLslFrameBoarderSize == 0) strOutMissing += "-fb<8/16/24/32> <FrameBoarder as Decimal>\r\n";
    if (iRequestedPort < 0) strOutMissing += "-s \"<PortNum>\"\r\n";
    if (iRequestedRate < 0) strOutMissing += "-b \"<Baud Rate>\"\r\n";

    std::string strDataType;
    printf_s("LSL Stream Info:\r\n");
    printf_s("NAME: %s    TYPE: %s\r\n\r\n",strLslName.c_str(), strLslType.c_str());

    printf_s("Expected Serial Format:\r\n");
    printf_s("ID:    Type:     ByteSize:\r\n");
    switch (iLslFrameBoarderSize) {
    case 1:strDataType = "uint  8"; break;
    case 2:strDataType = "uint 16"; break;
    case 3:strDataType = "uint 24"; break;
    case 4:strDataType = "uint 32"; break;
    }; // switch CT
    printf_s("HDR  %7s     %1i (%8X)\r\n", strDataType.c_str(), iLslFrameBoarderSize, ui32FrameBorder);
    
    for (int n = 0; n < (int)channels.size(); n++) {
        switch (channels[n].ctChannelType) {
        case CT::CT_INT8:strDataType = "int  8"; break;
        case CT::CT_INT16:strDataType = "int 16"; break;
        case CT::CT_INT24:strDataType = "int 24"; break;
        case CT::CT_INT32:strDataType = "int 32"; break;
        case CT::CT_UINT8:strDataType = "uint  8"; break;
        case CT::CT_UINT16:strDataType = "uint 16"; break;
        case CT::CT_UINT24:strDataType = "uint 24"; break;
        case CT::CT_UINT32:strDataType = "uint 32"; break;
        }; // switch CT
        printf_s("%3i  %7s     %1i\r\n",channels[n].iChannelId, strDataType.c_str(), channels[n].iSize);

    }

    unsigned int portList[LAST_PORT];
    int count = SerialPort::scanPorts(portList, LAST_PORT);

    printf("\r\nAbout Serial:\r\n");
    printf("Available Ports: ");
    for (int n = 0; n < count; n++) {
        printf(" %i", (int)portList[n]);
        if (n < count - 1) printf(",");
    }; // for n

    printf("\r\nRequested port: ");
    if (iRequestedPort > -1) { printf("COM%i:\r\n", iRequestedPort); }
    else { printf("<none>\r\n"); };

    if (strOutMissing.size()>0) { printf("Missing Parameters:\r\n%s\r\n", strOutMissing.c_str()); return 0; }
    bool bSuccess;
    bSuccess = Serial.begin(iRequestedPort, iRequestedRate);

    // make a new stream_info (nchannelsch) and open an outlet with it
    //lsl::stream_info info(argc > 1 ? argv[1] : "SimpleStream", "EEG", nchannels);
    if (channels.size() > MAX_CHANNELS) { printf("To many Channels Requestet, limit is %i channels", MAX_CHANNELS); return 0; }
    lsl::stream_info info(strLslName,strLslType,channels.size(),lsl::IRREGULAR_RATE,lsl::cf_float32);
    lsl::stream_outlet outlet(info);

    while (true) {
        bSuccess = Serial.seekIntro(ui32FrameBorder, iLslFrameBoarderSize);
        if (bSuccess) {
            for (int n = 0; n < channels.size(); n++) {
                switch (channels[n].ctChannelType) {
                case CT::CT_INT8:   fLslChannels[n] = (float)Serial.readInt(1); break;
                case CT::CT_INT16:  fLslChannels[n] = (float)Serial.readInt(2); break;
                case CT::CT_INT24:  fLslChannels[n] = (float)Serial.readInt(3); break;
                case CT::CT_INT32:  fLslChannels[n] = (float)Serial.readInt(4); break;
                case CT::CT_UINT8:  fLslChannels[n] = (float)Serial.readUInt(1); break;
                case CT::CT_UINT16: fLslChannels[n] = (float)Serial.readUInt(2); break;
                case CT::CT_UINT24: fLslChannels[n] = (float)Serial.readUInt(3); break;
                case CT::CT_UINT32: fLslChannels[n] = (float)Serial.readUInt(4); break;

                }; // switch ctChannelType
            };// for n
            outlet.push_sample(fLslChannels);
        }; // if bSuccess
    }; // while true



    //std::cout << "Hello World!\n";
    return 0;
}

// Programm ausführen: STRG+F5 oder Menüeintrag "Debuggen" > "Starten ohne Debuggen starten"
// Programm debuggen: F5 oder "Debuggen" > Menü "Debuggen starten"

// Tipps für den Einstieg: 
//   1. Verwenden Sie das Projektmappen-Explorer-Fenster zum Hinzufügen/Verwalten von Dateien.
//   2. Verwenden Sie das Team Explorer-Fenster zum Herstellen einer Verbindung mit der Quellcodeverwaltung.
//   3. Verwenden Sie das Ausgabefenster, um die Buildausgabe und andere Nachrichten anzuzeigen.
//   4. Verwenden Sie das Fenster "Fehlerliste", um Fehler anzuzeigen.
//   5. Wechseln Sie zu "Projekt" > "Neues Element hinzufügen", um neue Codedateien zu erstellen, bzw. zu "Projekt" > "Vorhandenes Element hinzufügen", um dem Projekt vorhandene Codedateien hinzuzufügen.
//   6. Um dieses Projekt später erneut zu öffnen, wechseln Sie zu "Datei" > "Öffnen" > "Projekt", und wählen Sie die SLN-Datei aus.
