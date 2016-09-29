#ifndef __GRIB_BLE_H__
#define __GRIB_BLE_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


#include "grib_define.h"
#include "grib_onem2m.h"
#include "grib_thread.h"
#include "grib_util.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#define BLE_MAX_SIZE_SEND_MSG								256
#define BLE_MAX_SIZE_RECV_MSG								256

#define BLE_FILE_PATH_BLECOMM_PROGRAM					"./libs/ble_extend/attrib/gatttool"

#define BLE_FILE_PATH_PYTHON_PROGRAM						"/usr/bin/python"
#define BLE_FILE_PATH_PYTHON_SCRIPT						"libs/ble_python/blecomm.py"

#define BLE_FILE_PATH_PIPE_ROOT							"pipe"
#define BLE_FILE_PATH_LOG_ROOT							"log"

#define BLE_RESPONSE_STR_OK								"OK"
#define BLE_RESPONSE_STR_ERROR							"ERROR"

#define BLE_FILE_NAME_PIPE_TEMP							"TEMP_PIPE_FILE"

#define BLE_CMD_GET_DEVICE_ID								"GET+DEVID?"
#define BLE_CMD_SET_DEVICE_ID								"SET+DEVID=%s"
#define BLE_CMD_GET_DEVICE_IF								"GET+DEVIF?"
#define BLE_CMD_GET_DEVICE_LOC							"GET+LOC?"
#define BLE_CMD_SET_DEVICE_LOC							"SET+LOC=%s"
#define BLE_CMD_GET_DEVICE_DESC							"GET+DESC?"
#define BLE_CMD_GET_REPORT_CYCLE							"GET+CYCLE?"

#define BLE_CMD_GET_FUNC_DATA								"GET+%s?"
#define BLE_CMD_SET_FUNC_DATA								"SET+%s=%s"
#define BLE_CMD_GET_FUNC_NAME								"GET+FNAME?"
#define BLE_CMD_GET_FUNC_ATTR								"GET+FATTR?"
#define BLE_CMD_GET_FUNC_COUNT							"GET+FCOUNT?"


typedef struct
{
	const char*	bleAddr;
	const char*	blePipe;
	const char*	bleSendMsg;
	const char*	bleRecvMsg;
	const char*	bleErrorMsg;
}Grib_BleLogInfo;


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: HCI TOOL DEPENDENCY
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define FLAGS_AD_TYPE 0x01
#define FLAGS_LIMITED_MODE_BIT 0x01
#define FLAGS_GENERAL_MODE_BIT 0x02

#define EIR_FLAGS                   0x01  /* flags */
#define EIR_UUID16_SOME             0x02  /* 16-bit UUID, more available */
#define EIR_UUID16_ALL              0x03  /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME             0x04  /* 32-bit UUID, more available */
#define EIR_UUID32_ALL              0x05  /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME            0x06  /* 128-bit UUID, more available */
#define EIR_UUID128_ALL             0x07  /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT              0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           0x09  /* complete local name */
#define EIR_TX_POWER                0x0A  /* transmit power level */
#define EIR_DEVICE_ID               0x10  /* device ID */

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_BleSendMsg(char* deviceAddr, char *pipeFileName, char* sendBuff, char* recvBuff);

int Grib_BleGetDeviceID(char* deviceAddr, char* recvBuff);
int Grib_BleSetDeviceID(char* deviceAddr, char* deviceID, char* recvBuff);
int Grib_BleGetDeviceIF(char* deviceAddr, char* deviceID, char* recvBuff);
int Grib_BleGetDeviceLoc(char* deviceAddr, char* deviceID, char* recvBuff);
int Grib_BleSetDeviceLoc(char* deviceAddr, char* deviceID, char* deviceLoc, char* recvBuff);
int Grib_BleGetDeviceDesc(char* deviceAddr, char* deviceID, char* recvBuff);

int Grib_BleGetFuncCount(char* deviceAddr, char* deviceID, char* recvBuff);
int Grib_BleGetFuncName(char* deviceAddr, char* deviceID, char* recvBuff);
int Grib_BleGetFuncAttr(char* deviceAddr, char* deviceID, char* recvBuff);
int Grib_BleGetReportCycle(char* deviceAddr, char* deviceID, char* recvBuff);
int Grib_BleGetFuncData(char* deviceAddr, char* deviceID, char* funcName, char* recvBuff);
int Grib_BleSetFuncData(char* deviceAddr, char* deviceID, char* funcName, char* content, char* recvBuff);

int Grib_BleConfig(void);
int Grib_BleDetourInit(void);
int Grib_BleDeviceInit(void);
int Grib_BleCleanAll(void);
int Grib_BleDeviceScan(int iScanCount);
int Grib_BleDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo);



#endif
