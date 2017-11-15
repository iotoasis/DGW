#ifndef __GRIB_DEFINE_H__
#define __GRIB_DEFINE_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <sys/types.h>
#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <cstdlib>
#include <ctype.h>
#include <ctime>
#include <cerrno>

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Basic Keyword
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define GRIB_HUB_VERSION					"1700818_MOD_HTTP_PARSER"

#define GRIB_PLATFORM_SERVER_USE_DNS		TRUE
#define GRIB_PLATFORM_SERVER_DOMAIN		"si.iotoasis.org"

#define GRIB_SMD_SERVER_USE_DNS			TRUE
#define GRIB_SMD_SERVER_DOMAIN			"sda.iotoasis.org"

#ifndef FALSE
#define FALSE								0
#endif

#ifndef TRUE
#define TRUE								(!FALSE)
#endif

#ifndef NULL
#define NULL								0
#endif

#ifndef OFF
#define OFF									0
#endif

#ifndef ON
#define ON									(!OFF)
#endif

#ifndef __FILE_PATH__
#define __FILE_PATH__						__FILE__
#endif

#ifndef __FILE_NAME__
#define __FILE_NAME__ 						(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__)
#endif

#ifndef __FUNC__
#define __FUNC__							__func__
#endif

#ifndef __FUNC_NAME__
#define __FUNC_NAME__						__func__
#endif

#ifndef SIZE_0
#define SIZE_0								0
#endif

#ifndef SIZE_1K
#define SIZE_1K								(1024)
#endif

#ifndef SIZE_1M
#define SIZE_1M								(SIZE_1K*SIZE_1K)
#endif

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Constant
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define GRIB_NOT_USED									0
#define GRIB_NOT_SUPPORTED							0
#define GRIB_INIT										0x00

#define GRIB_SUCCESS									0
#define GRIB_FAIL										(-1)

#define GRIB_DONE										GRIB_SUCCESS
#define GRIB_ERROR										GRIB_FAIL

#define GRIB_PROGRAM_REBOOT							"grib_reboot"
#define GRIB_PROGRAM_HCI								"grib_hci"
#define GRIB_PROGRAM_BLE_HUB							"grib_ble_hub"
#define GRIB_PROGRAM_GATTTOOL							"gatttool"

#define GRIB_SPACE										' '
#define GRIB_TAB										'\t'
#define GRIB_HASH										'#'
#define GRIB_COLON										':'
#define GRIB_STR_COLON									":"
#define GRIB_COMMA										','
#define GRIB_CR											'\r'
#define GRIB_LN											'\n'
#define GRIB_CRLN										"\r\n"

#define GRIB_FILE_PATH_LOG_ROOT						"log"

#define GRIB_STR_OK									"OK"
#define GRIB_STR_ERROR									"ERROR"
#define GRIB_STR_NOT_USED								"NOT_USED"

#define GRIB_STR_SUCCESS_L							"success"
#define GRIB_STR_FAILURE_L							"failure"

#define GRIB_STR_TIME_FORMAT 							"%04d%02d%02dT%02d%02d%02d"

#define GRIB_BOOL_TO_STR(iValue)						(iValue==TRUE?"TRUE":iValue==FALSE?"FALSE":"INVALID")
#define GRIB_ONOFF_TO_STR(iValue)						(iValue==ON?"ON":iValue==OFF?"OFF":"INVALID")

//1 shbaek: Device Func Attribute Mask
#define FUNC_ATTR_USE_NONE							0x0000
#define FUNC_ATTR_USE_REPORT							0x0001
#define FUNC_ATTR_USE_CONTROL							0x0002
#define FUNC_ATTR_USE_ALL								(FUNC_ATTR_USE_CONTROL|FUNC_ATTR_USE_REPORT)

#define FUNC_ATTR_CHECK_ALL(attr)						((attr & FUNC_ATTR_USE_ALL)==FUNC_ATTR_USE_ALL?TRUE:FALSE)
#define FUNC_ATTR_CHECK_REPORT(attr)					((attr & FUNC_ATTR_USE_REPORT)==FUNC_ATTR_USE_REPORT?TRUE:FALSE)
#define FUNC_ATTR_CHECK_CONTROL(attr)				((attr & FUNC_ATTR_USE_CONTROL)==FUNC_ATTR_USE_CONTROL?TRUE:FALSE)

#define GRIB_MAX_SIZE_BRIEF							64
#define GRIB_MAX_SIZE_SHORT							128
#define GRIB_MAX_SIZE_MIDDLE							256
#define GRIB_MAX_SIZE_LONG							512
#define GRIB_MAX_SIZE_DLONG							1024

#define GRIB_MAX_SIZE_BLE_ADDR_STR					17	//shbaek: 12(STRING)+5(:)
#define GRIB_MAX_SIZE_BLE_NAME_STR					24	//shbaek: BLE Module Name
#define GRIB_MAX_SIZE_TIME_STR						16	//shbaek: YYYYMMDD:HHMMSS
#define GRIB_MAX_SIZE_IP_STR							GRIB_MAX_SIZE_BRIEF
#define GRIB_MAX_SIZE_URI								SIZE_1K
#define GRIB_MAX_SIZE_AUTH_KEY						128 //shbaek: Just In Case ...

#define DEVICE_MAX_COUNT_FUNC							99
#define DEVICE_MAX_SIZE_ID							GRIB_MAX_SIZE_SHORT
#define DEVICE_MAX_SIZE_ADDR							GRIB_MAX_SIZE_BRIEF
#define DEVICE_MAX_SIZE_LOC							GRIB_MAX_SIZE_MIDDLE
#define DEVICE_MAX_SIZE_DESC							GRIB_MAX_SIZE_DLONG

#define DEVICE_MAX_SIZE_FUNC_NAME						GRIB_MAX_SIZE_SHORT
#define DEVICE_MAX_SIZE_EX_RSRCID						GRIB_MAX_SIZE_SHORT

#define GRIB_DEFAULT_AUTH_KEY							"GRIB_DEFAULT_KEY"
#define GRIB_1LINE_DASH			"# ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n"
#define GRIB_1LINE_SHARP			"# ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####\n"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: for Readability
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#ifndef MALLOC
#define MALLOC(size)									malloc(size)
#endif

#ifndef MEMSET
#define MEMSET(pMem, value, size)						memset((void *)pMem, value, size)
#endif

#ifndef FREE
#define FREE(pMem)										do{if(pMem!=NULL)free((void *)pMem);\
															pMem=NULL;}while(FALSE)
#endif

#ifndef MEMCPY
#define MEMCPY(pDes, pSrc, size)						memcpy(pDes, pSrc, size)
#endif

#ifndef STRCMP
#define STRCMP(str1, str2)								strcmp((const char *)str1, (const char *)str2)
#endif

#ifndef STRNCMP
#define STRNCMP(str1, str2, size)						strncmp((const char *)str1, (const char *)str2, (size_t)size)
#endif

#ifndef STRNCASECMP
#define STRNCASECMP(str1, str2, size)					strncasecmp((const char *)str1, (const char *)str2, (size_t)size)
#endif

#ifndef STRLEN
#define STRLEN(strSrc)									(strSrc==NULL?0:strlen((const char *)strSrc))
#endif

#ifndef STRDUP //shbaek: Return Allocated Another String Memory.
#define STRDUP(strSrc)									strdup((const char *)strSrc)
#endif

#ifndef STRTOK
#define STRTOK(strSrc, strToken)						strtok((char *)strSrc, (const char *)strToken)
#endif

#ifndef STRTOK_R
#define STRTOK_R(strSrc, strToken, pStr)				strtok_r((char *)strSrc, (const char *)strToken, (char **)pStr)
#endif

#ifndef STRSTR //shbaek: Return Pointer or NULL
#define STRSTR(strSrc, strSearch)						strstr((char *)strSrc, (const char *)strSearch)
#endif

#ifndef STRCHR
#define STRCHR(strSrc, charSearch)					strchr((char *)strSrc, charSearch)
#endif

#ifndef STRTRIM
#define STRTRIM(strSrc, charSearch)					trim((char *)strSrc)
#endif

#ifndef STRCAT
#define STRCAT(strDes, strSrc)						strcat((char *)strDes, (const char *)strSrc)
#endif

#ifndef STRINIT
#define STRINIT(strBuff, strLen)						MEMSET(strBuff, '\0', strLen)
#endif

#ifndef STRCPY
#define STRCPY(strDes, strSrc)						strcpy((char *)strDes, (const char *)strSrc)
#endif

#ifndef STRNCPY //shbaek: Copy strLen+1. String End is Always NULL
#define STRNCPY(strDes, strSrc, strLen)				strncpy((char *)strDes, (const char *)strSrc, (size_t)strLen)
#endif

#ifndef SPRINTF
#define SPRINTF(strBuff, strFormat, ...)				sprintf((char *)strBuff, (const char *)strFormat, __VA_ARGS__)
#endif

#ifndef SNPRINTF
#define SNPRINTF(strBuff, strLen, strFormat, ...)	snprintf((char *)strBuff, strLen, (const char *)strFormat, __VA_ARGS__)
#endif

#ifndef STRCASECMP //shbaek: Ignore Upper & Lower Case.
#define STRCASECMP(strCmpDes, strCmpSrc)				strcasecmp((const char *)strCmpDes, (const char *)strCmpSrc)
#endif

#ifndef ASPRINTF //shbaek: char **pStr2: Auto Allocation. Must be Free After Use.
#define ASPRINTF(pStr2, strFormat, ...)				asprintf(pStr2, strFormat, __VA_ARGS__)
#endif

#ifndef ATOI
#define ATOI(strNum)									atoi((const char *)strNum)
#endif

#ifndef ATOH32
#define ATOH32(strNum, pHexNum)						atoh32((const char *)strNum, (uint32 *)pHexNum)
#endif

#ifndef ATOH64
#define ATOH64(strNum, pHexNum)						atoh64((const char *)strNum, (uint64 *)pHexNum)
#endif

#ifndef ITOA
#define ITOA(iNum, strBuff, iRadix)					itoa(iNum, strBuff, iRadix)
#endif

#ifndef SLEEP
#define SLEEP(sec)										sleep(sec)
#endif

#ifndef LINUX_ERROR_NUM
#define LINUX_ERROR_NUM								errno
#endif

#ifndef LINUX_ERROR_STR
#define LINUX_ERROR_STR								strerror(errno)
#endif

inline void STRNSET(char* strDes, const char* strSrc, int BuffSize)
{
	if( !strDes || !strSrc || !BuffSize )return;

	STRINIT(strDes, BuffSize);
	if(STRLEN(strSrc)<BuffSize)STRNCPY(strDes, strSrc, STRLEN(strSrc));

	return;
}

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Type Define
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
typedef enum 
{//shbaek: Command Index
	GRIB_CMD_NAME		= 0, //shbaek: ./grib
	GRIB_CMD_MAIN		= 1, //shbaek: start, regi, debug, test, ...
	GRIB_CMD_SUB		= 2, 
	GRIB_CMD_ARG1		= 3,
	GRIB_CMD_ARG2		= 4,	
	GRIB_CMD_ARG3		= 5,
	GRIB_CMD_ARG4		= 6,
	GRIB_CMD_MAX
}Grib_CmdIndex;

typedef enum
{
	//shbaek: Provide Device's Interface Type
	DEVICE_IF_TYPE_NONE								= 0,
	DEVICE_IF_TYPE_BLE								= 1,
	DEVICE_IF_TYPE_ZIGBEE								= 2,
	DEVICE_IF_TYPE_ZWAVE								= 3,
	DEVICE_IF_TYPE_INTERNAL							= 4, //shbaek: No Have Semantic Descriptor
	DEVICE_IF_TYPE_EXTERNAL							= 5,
	DEVICE_IF_TYPE_MAX
}Grib_DeviceIfType;

typedef enum
{
	THREAD_STATUS_NONE								= 0,
	THREAD_STATUS_POLLING								= 1,
	THREAD_STATUS_USE_BLE								= 2,
	THREAD_STATUS_NEED_ANSWER							= 3,
	THREAD_STATUS_MAX
}Grib_ThreadStatus;

typedef enum
{
	BLE_ERROR_CODE_NONE								= 0,
    BLE_ERROR_CODE_GENERIC							= 1,
    BLE_ERROR_CODE_INVALID_COMMAND					= 2,
    BLE_ERROR_CODE_SENSOR								= 3,
    BLE_ERROR_CODE_BLE								= 4,
    BLE_ERROR_CODE_NOT_YET_IMPLEMENT					= 5,

	BLE_ERROR_CODE_BASE								= 100,
	BLE_ERROR_CODE_INVALID_PARAM						= BLE_ERROR_CODE_BASE+1,
	BLE_ERROR_CODE_CONNECT_FAIL						= BLE_ERROR_CODE_BASE+2,
	BLE_ERROR_CODE_SEND_FAIL							= BLE_ERROR_CODE_BASE+3,
	BLE_ERROR_CODE_RECV_FAIL							= BLE_ERROR_CODE_BASE+4,
	BLE_ERROR_CODE_INTERNAL							= BLE_ERROR_CODE_BASE+5,
	BLE_ERROR_CODE_INTERACTIVE						= BLE_ERROR_CODE_BASE+6,
	BLE_ERROR_CODE_GET_HANDLE							= BLE_ERROR_CODE_BASE+7,
	BLE_ERROR_CODE_READ_TIMEOUT						= BLE_ERROR_CODE_BASE+8,
	BLE_ERROR_CODE_CRITICAL							= BLE_ERROR_CODE_BASE+99,
	BLE_ERROR_CODE_MAX
}Grib_BleErrorCode;

typedef struct
{
	int  index;

	char addr[GRIB_MAX_SIZE_BLE_ADDR_STR+1];
	char name[GRIB_MAX_SIZE_SHORT];
	char memo[GRIB_MAX_SIZE_MIDDLE];

	unsigned int type;	//shbaek: Peer Address Type
}Grib_ScanDevInfo;

typedef struct tm TimeInfo;
typedef unsigned char byte;
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Feature
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define FEATURE_GRIB_BLE_EX							ON
#define FEATURE_GRIB_LOG								ON

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Constance Define - Feature Dependency
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define GRIB_DEFAULT_REPORT_CYCLE						30

#define GRIB_WAIT_BLE_REUSE_TIME						1
#define GRIB_CONTROL_FAIL_WAIT_TIME_SEC				1

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: About Log
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
// TODO: shbaek: NOT YET ...
#if (FEATURE_GRIB_LOG==ON)
#define GRIB_LOGD(...)									printf(__VA_ARGS__)
//#define GRIB_LOGE(...)								printf(__VA_ARGS__)

#else
#define GRIB_LOGD(...)									(GRIB_NOT_USED)
#endif

#define LOG_TAG_TEST									"# TEST"

#define LOG_TAG_DB										"# DB"


#endif
