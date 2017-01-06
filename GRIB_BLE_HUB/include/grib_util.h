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

#include "grib_define.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define GRIB_CONFIG_FILE_PATH							"./config/grib.config"
#define GRIB_CONFIG_SEPARATOR							":"

#define GRIB_CONFIG_HUB_ID							"HUB_ID"

#define GRIB_CONFIG_PLATFORM_SERVER_IP				"PLATFORM_SERVER_IP"
#define GRIB_CONFIG_PLATFORM_SERVER_PORT				"PLATFORM_SERVER_PORT"

#define GRIB_CONFIG_AUTH_SERVER_IP					"AUTH_SERVER_IP"
#define GRIB_CONFIG_AUTH_SERVER_PORT					"AUTH_SERVER_PORT"

#define GRIB_CONFIG_SDA_SERVER_IP						"SDA_SERVER_IP"
#define GRIB_CONFIG_SDA_SERVER_PORT					"SDA_SERVER_PORT"

#define GRIB_CONFIG_MYSQL_DB_HOST						"MYSQL_DB_HOST"
#define GRIB_CONFIG_MYSQL_DB_PORT						"MYSQL_DB_PORT"
#define GRIB_CONFIG_MYSQL_DB_USER						"MYSQL_DB_USER"
#define GRIB_CONFIG_MYSQL_DB_PASSWORD				"MYSQL_DB_PASSWORD"

#define GRIB_CONFIG_RESET_TIMER_USE					"RESET_TIMER_USE"
#define GRIB_CONFIG_RESET_TIME_HOUR					"RESET_TIME_HOUR"

#define GRIB_CONFIG_DEBUG_ONEM2M						"GRIB_DEBUG_ONEM2M"
#define GRIB_CONFIG_DEBUG_BLE							"GRIB_DEBUG_BLE"
#define GRIB_CONFIG_DEBUG_THREAD						"GRIB_DEBUG_THREAD"
#define GRIB_CONFIG_TOMBSTONE_BLE						"GRIB_TOMBSTONE_BLE"
#define GRIB_CONFIG_TOMBSTONE_HTTP					"GRIB_TOMBSTONE_HTTP"

#define READ_OPT_IGNORE_LF							0x0001

#define BIT_MASK_2										0x03
#define BIT_MASK_4										0x0F
#define BIT_MASK_6										0x3F
#define BASE64_ENC_SRC_BYTE							3
#define BASE64_DEC_SRC_BYTE							4

#define BASE64_PAD										'='

typedef struct
{
	int  isLoad;
	char hubID[DEVICE_MAX_SIZE_ID];

	char platformServerIP[GRIB_MAX_SIZE_IP_STR];
	unsigned int platformServerPort;

	char authServerIP[GRIB_MAX_SIZE_IP_STR];
	unsigned int authServerPort;

	char sdaServerIP[GRIB_MAX_SIZE_IP_STR];
	unsigned int sdaServerPort;

	char iotDbHost[GRIB_MAX_SIZE_IP_STR];
	unsigned int iotDbPort;

	char iotDbUser[GRIB_MAX_SIZE_SHORT];
	char iotDbPswd[GRIB_MAX_SIZE_SHORT];

	int resetTimerSwitch;
	int resetTimeHour;

	int debugThread;
	int debugOneM2M;
	int debugBLE;

	int tombStoneBLE;
	int tombStoneHTTP;

}Grib_ConfigInfo;


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
int 	systemCommand(const char *pCommand, char *pLineBuffer, int iBufferSize);
int 	Grib_LoadConfig(Grib_ConfigInfo* pConfigInfo);

int 	Grib_GetHostName(char* pBuff);
int 	Grib_GetIPAddr(char* pBuff);

const char* Grib_FuncAttrToStr(int iAttr);
const char* Grib_InterfaceToStr(Grib_DeviceIfType iType);
const char* Grib_ThreadStatusToStr(int iStatus);
const char* Grib_BleErrorToStr(Grib_BleErrorCode iType);

Grib_ConfigInfo* Grib_GetConfigInfo(void);

char* Grib_TrimAll(char *pSource);
int Grib_RandNum(int iMin, int iRange);

int Grib_ReadTextFile(char* filePath, char* pBuff, int opt);

void Grib_PrintHex(const char* LABEL, char* pHexBuff, int iSize);
void Grib_PrintOnlyHex(char* pHexBuff, int iSize);

long Grib_GetStackLimit(void);

int Grib_Base64Encode(char* srcBuff, char* encBuff, int opt);
int Grib_Base64Decode(char* srcBuff, char* decBuff, int opt);

int Grib_ReadTextFile(char* filePath, char* pBuff, int opt);
int Grib_WriteTextFile(char* filePath, char* pBuff, char* opt);


#endif
