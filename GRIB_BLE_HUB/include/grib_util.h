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

#include "grib_define.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define GRIB_CONFIG_FILE_PATH							"./config/grib.config"
#define GRIB_CONFIG_SEPARATOR							":"

#define GRIB_CONFIG_HUB_ID							"HUB_ID"

#define GRIB_CONFIG_PLATFORM_SERVER_IP				"PLATFORM_SERVER_IP"
#define GRIB_CONFIG_PLATFORM_SERVER_PORT				"PLATFORM_SERVER_PORT"
#define GRIB_CONFIG_MYSQL_DB_HOST						"MYSQL_DB_HOST"
#define GRIB_CONFIG_MYSQL_DB_PORT						"MYSQL_DB_PORT"
#define GRIB_CONFIG_MYSQL_DB_USER						"MYSQL_DB_USER"
#define GRIB_CONFIG_MYSQL_DB_PASSWORD				"MYSQL_DB_PASSWORD"
#define GRIB_CONFIG_DEBUG_ONEM2M						"GRIB_DEBUG_ONEM2M"
#define GRIB_CONFIG_DEBUG_BLE							"GRIB_DEBUG_BLE"
#define GRIB_CONFIG_DEBUG_THREAD						"GRIB_DEBUG_THREAD"
#define GRIB_CONFIG_BLE_TOMBSTONE						"GRIB_BLE_TOMBSTONE"

typedef struct
{
	char hubID[DEVICE_MAX_SIZE_ID];

	char platformServerIP[GRIB_MAX_SIZE_IP_STR];
	unsigned int platformServerPort;

	char iotDbHost[GRIB_MAX_SIZE_IP_STR];
	unsigned int iotDbPort;

	char iotDbUser[GRIB_MAX_SIZE_SHORT];
	char iotDbPswd[GRIB_MAX_SIZE_SHORT];

	int debugThread;
	int debugOneM2M;
	int debugBLE;

	int bleTombStone;
	
}Grib_ConfigInfo;


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_CountChar(char* arr, char c);
char* Grib_Split(char* arr, char c, int n);

int skipSpace(char **str2Buff);
int strStartsWith(const char *line, const char *prefix);
int strLower(char *strBuff);
int strUpper(char *strBuff);
void mSleep(long long mSec);
int systemCommand(char *pCommand, char *pLineBuffer, int iBufferSize);
int Grib_LoadConfig(Grib_ConfigInfo* pConfigInfo);

int Grib_GetHostName(char* pBuff);
int Grib_GetIPAddr(char* pBuff);


const char* Grib_FuncAttrToStr(int iAttr);
const char* Grib_InterfaceToStr(Grib_DeviceIfType iType);
const char* Grib_ThreadStatusToStr(int iStatus);
const char* Grib_BleErrorToStr(Grib_BleErrorCode iType);


#endif
