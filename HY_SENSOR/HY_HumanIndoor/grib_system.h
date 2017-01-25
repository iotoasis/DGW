#ifndef __GRIB_SYSTEM_H__
#define __GRIB_SYSTEM_H__

// #include <SoftwareSerial.h>
#include "grib_config.h"
#include <stddef.h>

//# ########## ########## ########## ########## ########## ########## ########## ##########
//shbaek: CONSTANT VALUE
//# ########## ########## ########## ########## ########## ########## ########## ##########

#define MAX_SIZE_BT_BUFF                256
#define MAX_SIZE_DEVICE_ID              256

#define RES_OK                          0
#define RES_ERROR                       1

#define ANALOG_MIN                      0
#define ANALOG_MAX                      1024

#define GRIB_BLE_MSG_END_SYMBOL         "\n"

#define RES2STRING(x)                  x==0?"OK":"ERROR"
#define BOOL2STRING(x)                 x==false?"FALSE":"TRUE"
#define ATTR2STRING(x)                 x==FUNC_ATTR_USE_REPORT?"REPORT ONLY": x==FUNC_ATTR_USE_CONTROL?"CONTROL ONLY" : x==FUNC_ATTR_USE_ALL?"REPORT & CONTROL":"INVALID FUNC ATTR"

#define FUNC_ATTR_USE_REPORT            0x0001
#define FUNC_ATTR_USE_CONTROL           0x0002
#define FUNC_ATTR_USE_ALL               (FUNC_ATTR_USE_CONTROL|FUNC_ATTR_USE_REPORT)

#define FUNC_ATTR_CHECK_ALL(attr)     ((attr & FUNC_ATTR_USE_ALL)==FUNC_ATTR_USE_ALL?TRUE:FALSE)
#define FUNC_ATTR_CHECK_REPORT(attr)  ((attr & FUNC_ATTR_USE_REPORT)==FUNC_ATTR_USE_REPORT?TRUE:FALSE)
#define FUNC_ATTR_CHECK_CONTROL(attr) ((attr & FUNC_ATTR_USE_CONTROL)==FUNC_ATTR_USE_CONTROL?TRUE:FALSE)

#define FUNC_ATTR_STRING_ALL(attr)     ((attr & FUNC_ATTR_USE_ALL)==FUNC_ATTR_USE_ALL?TRUE:FALSE)
#define FUNC_ATTR_STRING_REPORT(attr)  ((attr & FUNC_ATTR_USE_REPORT)==FUNC_ATTR_USE_REPORT?TRUE:FALSE)
#define FUNC_ATTR_STRING_CONTROL(attr) ((attr & FUNC_ATTR_USE_CONTROL)==FUNC_ATTR_USE_CONTROL?TRUE:FALSE)

//# ########## ########## ########## ########## ########## ########## ########## ##########
//shbaek: TYPE DEFINE
//# ########## ########## ########## ########## ########## ########## ########## ##########
typedef enum
{
    GRIB_ERROR_NONE                 = 0,
    GRIB_ERROR_GENERIC              = 1,
    GRIB_ERROR_INVALID_COMMAND      = 2,
    GRIB_ERROR_SENSOR               = 3,
    GRIB_ERROR_BLE                  = 4,
    GRIB_ERROR_NOT_YET_IMPLEMENT    = 5,
    GRIB_ERROR_MAX
} GribErrorType;

typedef struct
{
    char* funcName;
    int   funcAttr;
} GribFuncInfo;

typedef struct
{
    char* recvMsg;
    char* sendMsg;
} GribFuncParam;

typedef int (*GribFunc) (GribFuncParam* pParam);

typedef struct
{
    char* cmdName;
    GribFunc pHandle;
} GribCmdHandler;

extern int gDebug;
//# ########## ########## ########## ########## ########## ########## ########## ##########
//shbaek: PROTOTYPE
//dhyim: Modified For RTOS
//# ########## ########## ########## ########## ########## ########## ########## ##########
GribFunc findHandler(char* recvMsg);

void mySetup(void);
int myLoop(void);

int getDeviceID(GribFuncParam* pParam);
int setDeviceID(GribFuncParam* pParam);
int getDeviceLoc(GribFuncParam* pParam);
int setDeviceLoc(GribFuncParam* pParam);
int getDeviceDesc(GribFuncParam* pParam);
int getFuncCount(GribFuncParam* pParam);
int getFuncName(GribFuncParam* pParam);
int getFuncAttr(GribFuncParam* pParam);
int getReportCycle(GribFuncParam* pParam);
int myBleMsg(char* bleSendMsg, char* bleRecvMsg);
int myBleReset(void);
int myBleState(void);
int getBleAddr(char* pAddr);
int getBleName(GribFuncParam* pParam);
int setBleName(GribFuncParam* pParam);
int setLogFlag(GribFuncParam* pParam);
int myReboot(GribFuncParam* pParam);
int setMyLed(int onoff);

int myReport(GribFuncParam* pParam);
void mySetup(void);
#endif
