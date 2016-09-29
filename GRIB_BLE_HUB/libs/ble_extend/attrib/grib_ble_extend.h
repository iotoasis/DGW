#ifndef __GRIB_BLE_EXTEND_H__
#define __GRIB_BLE_EXTEND_H__
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <btio/btio.h>

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Type Define
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
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
#define __FILE_PATH__		__FILE__
#endif

#ifndef __FILE_NAME__
#define __FILE_NAME__ 		(strrchr(__FILE__, '/') ? strrchr(__FILE__, '/')+1 : __FILE__)
#endif

#ifndef __FUNC__
#define __FUNC__			__func__
#endif

#ifndef __FUNC_NAME__
#define __FUNC_NAME__		__func__
#endif

#define BLE_HEADER_SIZE  				3
#define BLE_MSG_MAX_SIZE  				20
#define BLE_MSG_END_SYMBOL  			'\n'

#define GRIB_LOG_CR_LF					"\r\n"
#define GRIB_LOG_DIR					"log"

#define GRIB_ERROR_STR_BUSY			"Device or resource busy"

#define BLE_EX_SIZE_ADDR				32
#define BLE_EX_SIZE_PIPE_PATH			1024
#define BLE_EX_SIZE_SEND_MSG			256
#define BLE_EX_SIZE_RECV_MSG			256

typedef enum
{
	BLE_ERROR_CODE_NONE				= 0,
	BLE_ERROR_CODE_BASE				= 100,
	BLE_ERROR_CODE_INVALID_PARAM		= BLE_ERROR_CODE_BASE+1,
	BLE_ERROR_CODE_CONNECT_FAIL		= BLE_ERROR_CODE_BASE+2,
	BLE_ERROR_CODE_SEND_FAIL			= BLE_ERROR_CODE_BASE+3,
	BLE_ERROR_CODE_RECV_FAIL			= BLE_ERROR_CODE_BASE+4,
	BLE_ERROR_CODE_INTERNAL			= BLE_ERROR_CODE_BASE+5,
	BLE_ERROR_CODE_INTERACTIVE		= BLE_ERROR_CODE_BASE+6,
	BLE_ERROR_CODE_CRITICAL			= BLE_ERROR_CODE_BASE+99,
	BLE_ERROR_CODE_MAX
}Grib_BleErrorCode;

typedef enum
{
	BLE_STATUS_ERROR					=-1,
	BLE_STATUS_NONE					= 0,
	BLE_STATUS_INIT					= 1,
	BLE_STATUS_CONNECT				= 2,
	BLE_STATUS_LISTEN					= 3,
	BLE_STATUS_OPERATE				= 4,
	BLE_STATUS_DONE					= 5,
	BLE_STATUS_DISCONNECT				= 6,
	BLE_STATUS_MAX
}Grib_BleStatus;

typedef enum
{
	ARG_INDEX_MAIN_CMD				= 0,
	ARG_INDEX_BLE_ADDR				= 1,
	ARG_INDEX_PIPE_PATH				= 2,
	ARG_INDEX_SEND_MSG				= 3,
	ARG_INDEX_OPTION					= 4,
	ARG_INDEX_MAX
}BleCommArgIndex;


typedef struct
{
	int		isDebug;
	int		pipeWrite;
	char	pAddr[BLE_EX_SIZE_ADDR];
	char	pPipePath[BLE_EX_SIZE_PIPE_PATH];
	char	pSendBuff[BLE_EX_SIZE_SEND_MSG];
	char	pRecvBuff[BLE_EX_SIZE_RECV_MSG];
	char	pOption[BLE_EX_SIZE_SEND_MSG];
	
}BleCommArg;

typedef struct
{
    int* pNaTime;
    int* pStatus;
} GribTimerParam;

typedef int (*GribFunc) (GribTimerParam* pParam);

typedef struct
{
    char* cmdName;
    GribFunc pHandle;
} GribCmdHandler;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int  str2hex(char* srcBuff, char* hexBuff);

int  Grib_CheckCriticalMsg(GError* pErrorMsg);
int	 Grib_BleCommWritePipe(char* pipeFile, char* pipeMsg);
int	 Grib_BleCommLoopQuit(GMainLoop* pEventLoop, GError* pErrorMsg, Grib_BleErrorCode iErrorCode);
void Grib_BleCommSetArg(BleCommArg* pBleCommArg);

#endif
