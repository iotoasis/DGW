#include "grib_system.h"
#include "FreeRTOS.h"
#include <task.h>
#include <task_common.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>



/*

    shbaek: [USER-MOD] User Function's Name & Attribute

           Function Name -> OneM2M's Container Name.

*/
GribFuncInfo gFuncInfo[DEVICE_FUNC_COUNT] =
{
	{FUNC_NAME,       FUNC_ATTR_USE_REPORT},
};

//shbaek: Command & Handler
GribCmdHandler gCmdHandler[] =
{
    //shbeak: GRIB Command & Handler
    {"GET+DEVID?",   getDeviceID},
    {"SET+DEVID=",   setDeviceID},
    {"GET+LOC?",     getDeviceLoc},
    {"SET+LOC=",     setDeviceLoc},
    {"GET+DESC?",    getDeviceDesc},
    {"GET+FCOUNT?",  getFuncCount},
    {"GET+FNAME?",   getFuncName},
    {"GET+FATTR?",   getFuncAttr},
    {"GET+CYCLE?",   getReportCycle},
    {"BLE+NAME?",    getBleName},
    {"BLE+NAME=",    setBleName},
    {"SET+LOG=",     setLogFlag},
    {"MY+REBOOT",    myReboot},

    /*  shbeak: [USER-MOD] User Command & Handler Matching Table.
                Remember, Command Naming Rule:
                GET+"Func Name" or SET+"Func Name"
                If Have Control Attr, Must Need SET+"Func Name"
                If Have Report Attr, Must Need GET+"Func Name"
                and If Have All Attr, Must Need Both.
    */
    {"GET+" FUNC_NAME "?",     myReport},

    //shbeak: Last Point. Do Not Modify
    {NULL,           NULL}
};

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: GRIB FUNCTION
//# dhyim: Modified For RTOS
//# ########## ########## ########## ########## ########## ########## ########## ##########
int getDeviceID(GribFuncParam* pParam)
{   //shbaek: [TO-DO] NOT YET IMPLEMENTED.
	snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%s", DEVICE_ID);
	return RES_OK;
}

int setDeviceID(GribFuncParam* pParam)
{   //shbaek: [TO-DO] NOT YET IMPLEMENTED.
	snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%s:%d", RES2STRING(RES_ERROR), GRIB_ERROR_NOT_YET_IMPLEMENT);
	return RES_OK;
}

int getDeviceLoc(GribFuncParam* pParam)
{   //shbaek: [TO-DO] NOT YET IMPLEMENTED.
	snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%s", DEVICE_LOC);
	return RES_OK;
}

int setDeviceLoc(GribFuncParam* pParam)
{   //shbaek: [TO-DO] NOT YET IMPLEMENTED.
	snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%s:%d", RES2STRING(RES_ERROR), GRIB_ERROR_NOT_YET_IMPLEMENT);
	return RES_OK;
}

int getDeviceDesc(GribFuncParam* pParam)
{   //shbaek: [TO-DO] NOT YET IMPLEMENTED.
	snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%s", DEVICE_DESC);
	return RES_OK;
}

int getFuncCount(GribFuncParam* pParam)
{
	snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%d", DEVICE_FUNC_COUNT);
	return RES_OK;
}

int getFuncName(GribFuncParam* pParam)
{
	int i = 0;

	for (i = 0; i < DEVICE_FUNC_COUNT; i++)
	{
		if (i != 0)
		{
			strncat(pParam->sendMsg, ",", 1);
		}
		strncat(pParam->sendMsg, gFuncInfo[i].funcName, strlen(gFuncInfo[i].funcName));
	}

	return RES_OK;
}

int getFuncAttr(GribFuncParam* pParam)
{
	int i = 0;
	char pBuff[10] = {'\0',};

	for (i = 0; i < DEVICE_FUNC_COUNT; i++)
	{
		if (i != 0)
		{
			strncat(pParam->sendMsg, ",", 1);
		}

		memset(pBuff, '\0', sizeof(pBuff));
		itoa(gFuncInfo[i].funcAttr, pBuff, 10);
		strncat(pParam->sendMsg, pBuff, strlen(pBuff));
	}

	return RES_OK;
}

int getReportCycle(GribFuncParam* pParam)
{
	snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%d", REPORT_CYCLE_TIME);
	return RES_OK;
}

int myReboot(GribFuncParam* pParam)
{
// 	Serial.println("# REBOOT");					// Only For Debug, Requires another serial port
// 	Serial.println();
	myBleReset();

	vTaskDelay(1000 / portTICK_RATE_MS);	
 	asm volatile ("  jmp 0");

	return RES_OK;
}

int myBleMsg(char* bleSendMsg, char* bleRecvMsg)
{
	int   i = 0;
	int   iError = RES_OK;
	int   iMsgLen = 0;
	int	  btUartReceive=0;

	iMsgLen = strlen(bleSendMsg);

// 	if(gDebug){									// Only For Debug, Requires another serial port
// 		Serial.print(F("# BLE SEND MSG["));
// 		Serial.print(strlen(bleSendMsg));
// 		Serial.print("]: ");
// 		Serial.print(bleSendMsg);
// 	}
	uart_send_data(bleSendMsg, strlen(bleSendMsg));
	
	memset(bleRecvMsg, '\0', MAX_SIZE_BT_BUFF);
	
	while(i<10)
	{
		if(uart_receive_data(bleRecvMsg)) break;
		vTaskDelay(100UL / portTICK_RATE_MS);
		i++;
	}
	if(i>=10) return 1;
	

	return iError;
}

// int myBleState(void)							// Only For Debug, Requires another serial port
// {
// 	int iState = LOW;
// 	iState = digitalRead(BT_STATE);
// 
// // 	if(gDebug){
// // 		Serial.print("# BT_STATE: ");
// // 		Serial.println(iState);
// // 	}
// 
// 	return iState;
// }

int myBleReset(void)
{
	char* bleSendMsg = "AT+RESET\r\n";
	char  bleRecvMsg[MAX_SIZE_BT_BUFF] = {'\0', };

// 	if(gDebug){									// Only For Debug, Requires another serial port
// 		Serial.println(F("# BLE RESET"));
// 	}

	myBleMsg(bleSendMsg, bleRecvMsg);

	return RES_OK;
}

int getBleAddr(char* pAddr)
{
	int   iLen = 0;
	int   iError = RES_OK;

	char* pTemp = NULL;
	char* bleSendMsg = "AT+ADDR?\r\n";
	char  bleRecvMsg[MAX_SIZE_BT_BUFF] = {'\0', };

	myBleMsg(bleSendMsg, bleRecvMsg);

	pTemp = &strchr(bleRecvMsg, '=')[1];
	iLen  = strlen(pTemp);

	if (iLen < MAX_SIZE_BT_BUFF)pTemp[iLen] = '\0';
	if ( (pTemp[iLen - 1] == '\r') || (pTemp[iLen - 1] == '\n') )pTemp[iLen - 1] = '\0';
	if ( (pTemp[iLen - 2] == '\r') || (pTemp[iLen - 2] == '\n') )pTemp[iLen - 2] = '\0';

	memset(pAddr, '\0', strlen(bleRecvMsg) + 1);
	strncpy(pAddr, pTemp, iLen);

// 	if(gDebug){									// Only For Debug, Requires another serial port
// 		Serial.print(F("# BLE ADDR: "));
// 		Serial.println(pAddr);
// 	}

	return iError;
}

int getBleName(GribFuncParam* pParam)
{
	int   iLen = 0;
	int   iError = RES_OK;

	char* pName = NULL;
	char* bleSendMsg = "AT+NAME\r\n";
	char  bleRecvMsg[MAX_SIZE_BT_BUFF] = {'\0', };

	myBleMsg(bleSendMsg, bleRecvMsg);

	//strncpy(bleRecvMsg, pParam->sendMsg, strlen(pParam->sendMsg));
	pName = &strchr(bleRecvMsg, '=')[1];
	iLen  = strlen(pName);
	if (iLen < MAX_SIZE_BT_BUFF)pName[iLen] = '\0';
	if ( (pName[iLen - 1] == '\r') || (pName[iLen - 1] == '\n') )pName[iLen - 1] = '\0';
	if ( (pName[iLen - 2] == '\r') || (pName[iLen - 2] == '\n') )pName[iLen - 2] = '\0';

	memset(pParam->sendMsg, '\0', strlen(bleRecvMsg) + 1);
	strncpy(pParam->sendMsg, pName, iLen);

// 	if(gDebug){									// Only For Debug, Requires another serial port
// 		Serial.print(F("# BLE NAME: "));
// 		Serial.println(pName);
// 	}

	return iError;
}

int setBleName(GribFuncParam* pParam)
{
	int   iError = RES_OK;

	char* pName = NULL;
	char  bleSendMsg[MAX_SIZE_BT_BUFF] = {'\0', };

	pName = &strchr(pParam->recvMsg, '=')[1];
	snprintf(bleSendMsg, sizeof(bleSendMsg), "AT+NAME%s\r\n", pName);

	myBleMsg(bleSendMsg, pParam->sendMsg);

	return iError;
}

int setLogFlag(GribFuncParam* pParam)			// Only For Debug, Requires another serial port
{
	int   iError = RES_ERROR;
// 	char  boolChar = '\0';
// 	char* pMsg = NULL;
// 
// 	boolChar = strchr(pParam->recvMsg, '=')[1];
// 	//Serial.print("# SET YOUR LOG FLAG: ");
// 	//Serial.println(boolChar);
// 
// 	memset(pParam->sendMsg, '\0', sizeof(pParam->sendMsg));
// 
// 	if (boolChar == '0')
// 	{
//  		gDebug = false;	
// 		iError = RES_OK;
// 		pMsg = "LOG FLAG DOWN";
// 	}
// 	else if (boolChar == '1')
// 	{
//  		gDebug = true;
// 		iError = RES_OK;
// 		pMsg = "LOG FLAG UP";
// 	}
// 	else
// 	{
// 		iError = RES_ERROR;
// 		pMsg = "INVALID FLAG VALUE (VALID: 0 or 1)";
// 	}
// 
// 	strncpy(pParam->sendMsg, pMsg, strlen(pMsg));
	return iError;
}

// int setMyLed(int onoff)						// Just No LED.
// {
// 	if (onoff == false)
// 	{
// 		digitalWrite(MYLED, LOW);
// 	}
// 	else
// 	{
// 		digitalWrite(MYLED, HIGH);
// 	}
// 
// 	return RES_OK;
// }

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: FIND HANDLER
//# ########## ########## ########## ########## ########## ########## ########## ##########
GribFunc findHandler(char* recvMsg)
{
	int i = 0;
	char* pCmd = NULL;

	do {
		pCmd = gCmdHandler[i].cmdName;

		if (strncasecmp(recvMsg, pCmd, strlen(pCmd)) == 0)
		{
// 			if(gDebug){							// Only For Debug, Requires another serial port
// 				Serial.println(F("# FIND HANDLER"));
// 			}
			return gCmdHandler[i].pHandle;
		}
		i++;
	} while ( gCmdHandler[i].cmdName != NULL );

// 	if(gDebug){									// Only For Debug, Requires another serial port
// 		Serial.print(F("# NO MATCHING COMMAND: "));
// 		Serial.println(recvMsg);
// 	}

	return NULL;
}