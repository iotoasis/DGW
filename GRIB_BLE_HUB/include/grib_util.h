#ifndef __GRIB_UTIL_H__
#define __GRIB_UTIL_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <ifaddrs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>
#include <arpa/inet.h>

#include "grib_define.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define READ_OPT_IGNORE_LF							0x0001

#define BIT_MASK_2										0x03
#define BIT_MASK_4										0x0F
#define BIT_MASK_6										0x3F
#define BASE64_ENC_SRC_BYTE							3
#define BASE64_DEC_SRC_BYTE							4

#define BASE64_PAD										'='


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int 	Grib_CountChar(char* arr, char c);
char* 	Grib_Split(char* arr, char c, int n);
int 	Grib_isHexString(char* strNum, int checkSize);

int 	skipSpace(char **str2Buff);
int 	strStartsWith(const char *line, const char *prefix);
int 	strLower(char *strBuff);
int 	strUpper(char *strBuff);
void 	mSleep(long long mSec);
void    systemReboot(int waitSec, char* msg);
int 	systemCommand(const char *pCommand, char *pLineBuffer, int iBufferSize);

int 	Grib_GetHostName(char* pBuff);
int 	Grib_GetIPAddr(char* pBuff);
int 	Grib_GetDnsIP(char* domain, char** ip);

const char* Grib_FuncAttrToStr(int iAttr);
const char* Grib_InterfaceToStr(Grib_DeviceIfType iType);
const char* Grib_ThreadStatusToStr(int iStatus);
const char* Grib_BleErrorToStr(Grib_BleErrorCode iType);

char* Grib_TrimAll(char *pSource);
int Grib_RandNum(int iMin, int iRange);

int Grib_ReadTextFile(char* filePath, char* pBuff, int opt);

//shbaek: "TEST+" (5Byte String) -> "544553542B" (10Byte Hex String)
int Grib_StrToHex(char* strBuff, char* hexBuff);

//shbaek: "114D" (4Byte Hex String) -> 0x11 0x4D (2Byte Binary)
int Grib_HexToBin(char* hexBuff, char* binBuff, int strSize);

long Grib_GetStackLimit(void);

int Grib_Base64Encode(char* srcBuff, char* encBuff, int opt);
int Grib_Base64EncodeBin(char* srcBuff, char* encBuff, int srcSize, int opt);

int Grib_Base64Decode(char* srcBuff, char* decBuff, int opt);
int Grib_Base64DecodeBin(char* srcBuff, char* decBuff, int srcSize, int opt);

int Grib_ReadTextFile(char* filePath, char* pBuff, int opt);
int Grib_WriteTextFile(char* filePath, char* pBuff, char* opt);

int Grib_DoubleFree(void** ppMem, int count);
int Grib_GetCurrDateTime(char* pBuff);
void Grib_ShowCurrDateTime(void);

void Grib_MemLog(char* logDir);

#endif
