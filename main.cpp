#include <iostream>
#include <windows.h>

using namespace std;

#define STX 0x02
#define ETX 0x03
#define SI 0x0F
#define SO 0x0E
#define ACK 0x06
#define EOT 0x04
#define NAK 0x15
#define MODE "DUAL"

HANDLE hSerial;

uint8_t openPort(int portNumber) {
    char sPortName[10];
    sprintf (sPortName, "COM%i", portNumber);
    hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if(!GetCommState(hSerial, &dcbSerialParams)){
        cout << "getting state error\n";
        exit(0);
    }
    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 7;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = EVENPARITY;
    if(!SetCommState(hSerial, &dcbSerialParams)){
        cout << "error setting serial port state\n";
        exit(0);
    }

    cout << "Opened port: " << sPortName << endl;
    return 1;
}


uint8_t ReadCOM(uint8_t & sReceivedByte) {
    DWORD iSize;
     ReadFile(hSerial, &sReceivedByte, 1, &iSize, 0);  // получаем 1 байт
    return iSize;
    }

DWORD WriteCOM(uint8_t data) {
    DWORD dwBytesWritten;    // тут будет количество собственно переданных байт
    DWORD dwSize = 1;
    WriteFile(hSerial, &data, dwSize, &dwBytesWritten, NULL);
    return dwBytesWritten;
}

void WriteStringCOM(string data) {
for(int i = 0; i < data.length(); i++) WriteCOM(data[i]);
}

uint8_t BCC(string data) {
char temp = data[1];
for(int i = 2; i < data.length(); i++) temp ^= data[i];
return temp;
}

void indicateHostDone() {
string input = "";
input += STX;
input += "Q2";
input += ETX;
input += BCC(input);
 WriteStringCOM(input);
}

void setMKEY() {
string input = "";
uint8_t data = 0;
input += SI;
input += "02";
input += "0";
input += "0123456789ABCDEF";
input += SO;
input += BCC(input);
 WriteStringCOM(input);
 for(int i = 0; i < 6; i++) {
    while(!ReadCOM(data));;
    printf("%02X ",data);
 }
 WriteCOM(0x06);
 while(!ReadCOM(data));;
 printf("%02X ",data);
}

void selectMKEY() {
string input = "";
input += SI;
input += "08";
input += "0";
input += SO;
input += BCC(input);
 WriteStringCOM(input);
}

void requestPINEntry() {
uint8_t data = 0;
string input = "";
input += STX;
input += "70";
input += 0x2E;
input += "4034351574462072";
input += 0x1C;
input += "59216EC9E36F8EF8";
input += "2.08";
input += ETX;
input += BCC(input);
WriteStringCOM(input);
 for(int i = 0; i < 11; i++) {
    while(!ReadCOM(data));;
    printf("%02X ",data);
 }
for(int i = 0; i < 16; i++) {
    while(!ReadCOM(data));;
    cout << data;
 }
 cout << ' ';
for(int i = 0; i < 2; i++) {
    while(!ReadCOM(data));;
    printf("%02X ",data);
 }
WriteCOM(0x06);
Sleep(1000);
indicateHostDone();
}

int main()
{
    openPort(4);
// setMKEY();
  // selectMKEY();
   requestPINEntry();
    return 0;
}
