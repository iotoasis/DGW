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

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "grib_define.h"
#include "grib_config.h"


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define BLE_MAX_SIZE_ADDR									17	//shbaek: 12(STRING)+5(:)
#define BLE_MAX_SIZE_HANDLE								4	//shbaek: 0x1234
#define BLE_GATTTOOL_TIMEOUT								5	//shbaek: 5 Sec

#define BLE_MAX_SIZE_SEND_MSG								256
#define BLE_MAX_SIZE_RECV_MSG								256

#define BLE_GRIB_HCI_FILE_NAME							"grib_hci"
#define BLE_GRIB_HCI_MENU_INIT							"init"

#define BLE_FILE_PATH_GATTTOOL							"./libs/ble_extend/attrib/gatttool"
#define BLE_FILE_PATH_GATTTOOL_EX							"./libs/bluez/attrib/gatttool"

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


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: GATTTOOL Dependency Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define BLE_GATT_OPT_READ									"--char-read"
#define BLE_GATT_OPT_WRITE_ONLY							"--char-write"
#define BLE_GATT_OPT_WRITE_REQ							"--char-write-req"
#define BLE_GATT_OPT_LISTEN								"--listen"

#define BLE_GATT_OPT_HANDLE								"--handle"
#define BLE_GATT_OPT_VALUE								"--value"
#define BLE_GATT_OPT_DEVICE								"--device"
#define BLE_GATT_OPT_PEER_PUBLIC							"--addr-type=public"
#define BLE_GATT_OPT_PEER_RANDOM							"--addr-type=random"





/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Type Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

typedef struct
{
	char	addr[BLE_MAX_SIZE_ADDR+1];
	char	handle[BLE_MAX_SIZE_HANDLE+1];
	char*	sendMsg;
	char*	recvMsg;

	char*	pipe;
	char*	label;
}Grib_BleDeviceInfo;

typedef struct
{
	const char*	bleAddr;
	const char*	blePipe;
	const char*	bleSendMsg;
	const char*	bleRecvMsg;
	const char*	bleErrorMsg;
}Grib_BleLogInfo;

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
int Grib_BleCleanAll(void);
int Grib_BleGetDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo);

int Grib_BleSendOnly(Grib_BleDeviceInfo* pBleDevice);
int Grib_BleSendReq(Grib_BleDeviceInfo* pBleDevice);

#endif
