/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_auth.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int gDebugAuth = FALSE;

char gAuthServerIp[GRIB_MAX_SIZE_IP_STR+1];
int  gAuthServerPort;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define __AUTH_FUNC__
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_AuthSetServerConfig(void)
{
	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# AUTH SERVCER CONFIG: LOAD CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	STRINIT(gAuthServerIp, sizeof(gAuthServerIp));
	STRNCPY(gAuthServerIp, pConfigInfo->authServerIP, STRLEN(pConfigInfo->authServerIP));

	gAuthServerPort = pConfigInfo->authServerPort;

	GRIB_LOGD("# AUTH SERVER CONFIG: %s:%d\n", gAuthServerIp, gAuthServerPort);

	return GRIB_SUCCESS;
}

int Grib_AuthGatewayRegi(char* gatewayID)
{
	int  iRes = GRIB_ERROR;
	int  iDBG = gDebugAuth;

	char  httpHead[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};

	Grib_HttpMsgInfo httpMsg;

	char sendBuff[HTTP_MAX_SIZE_SEND_MSG] = {'\0', };
	char recvBuff[HTTP_MAX_SIZE_RECV_MSG] = {'\0', };

	if(STRLEN(gatewayID) <= 0)
	{
		GRIB_LOGD("# AUTH GW REGI: IN-VALID PARAM !!!\n");
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# GATEWAY ID: %s\n", gatewayID);

	if( STRLEN(gAuthServerIp)==0 || (gAuthServerPort==0) )
	{
		Grib_AuthSetServerConfig();
	}

	SNPRINTF(httpBody, sizeof(httpBody), AUTH_BODY_FORMAT_GATEWAY_REGI, gatewayID);
	SNPRINTF(httpHead, sizeof(httpHead), AUTH_HEAD_FORMAT_GATEWAY_REGI, 
				gAuthServerIp, gAuthServerPort, STRLEN(httpBody));

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), "%s%s", httpHead, httpBody);

	MEMSET(&httpMsg, 0x00, sizeof(Grib_HttpMsgInfo));
	httpMsg.serverIP   = gAuthServerIp;
	httpMsg.serverPort = gAuthServerPort;
	httpMsg.LABEL= gatewayID;
	httpMsg.sendBuff = sendBuff;
	httpMsg.recvBuff = recvBuff;

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH GW REGI SEND[%d]:\n%s\n", STRLEN(sendBuff), sendBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(&httpMsg);
	if(iRes <= 0)
	{
		GRIB_LOGD("# HTTP MSG SEND ERROR !!!\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH GW REGI RECV[%d]:\n%s\n", STRLEN(recvBuff), recvBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpResParser(&httpMsg);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# HTTP MSG PARSE ERROR !!!\n");
		return iRes;
	}

	if(iDBG)
	{
		GRIB_LOGD("# REQ GATEWAY ID: %s\n", httpMsg.LABEL);
		GRIB_LOGD("# RES STATUS MSG: %s [%d]\n", httpMsg.statusMsg, httpMsg.statusCode);	
	}

	if(httpMsg.statusCode != HTTP_STATUS_CODE_OK)
	{
		return GRIB_ERROR;
	}

	return iRes;
}

int Grib_AuthDeviceRegi(char* deviceID, char* devicePW)
{
	int  iRes = GRIB_ERROR;
	int  iDBG = gDebugAuth;

	char  httpHead[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};

	Grib_HttpMsgInfo httpMsg;

	char sendBuff[HTTP_MAX_SIZE_SEND_MSG] = {'\0', };
	char recvBuff[HTTP_MAX_SIZE_RECV_MSG] = {'\0', };

	if(STRLEN(deviceID) <= 0)
	{
		GRIB_LOGD("# AUTH DEVICE REGI: IN-VALID PARAM !!!\n");
		return GRIB_ERROR;
	}

	if(STRLEN(devicePW) <= 0)
	{
		GRIB_LOGD("# AUTH DEVICE REGI: SET DEFAULT PASSWORD !!!\n");
		devicePW = AUTH_DEFAULT_DEVICE_PW;
	}

	if(iDBG)GRIB_LOGD("# DEVICE ID: %s\n", deviceID);

	if( STRLEN(gAuthServerIp)==0 || (gAuthServerPort==0) )
	{
		Grib_AuthSetServerConfig();
	}

	SNPRINTF(httpBody, sizeof(httpBody), AUTH_BODY_FORMAT_DEVICE_REGI, deviceID, devicePW);
	SNPRINTF(httpHead, sizeof(httpHead), AUTH_HEAD_FORMAT_DEVICE_REGI, 
				gAuthServerIp, gAuthServerPort, STRLEN(httpBody));

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), "%s%s", httpHead, httpBody);

	MEMSET(&httpMsg, 0x00, sizeof(Grib_HttpMsgInfo));
	httpMsg.serverIP   = gAuthServerIp;
	httpMsg.serverPort = gAuthServerPort;
	httpMsg.LABEL= deviceID;
	httpMsg.sendBuff = sendBuff;
	httpMsg.recvBuff = recvBuff;

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH DEVICE REGI SEND[%d]:\n%s\n", STRLEN(sendBuff), sendBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(&httpMsg);
	if(iRes <= 0)
	{
		GRIB_LOGD("# HTTP MSG SEND ERROR !!!\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH DEVICE REGI RECV[%d]:\n%s\n", STRLEN(recvBuff), recvBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpResParser(&httpMsg);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# HTTP MSG PARSE ERROR !!!\n");
		return iRes;
	}

	if(iDBG)
	{
		GRIB_LOGD("# REQ DEVICE ID : %s\n", httpMsg.LABEL);
		GRIB_LOGD("# RES STATUS MSG: %s [%d]\n", httpMsg.statusMsg, httpMsg.statusCode);	
	}

	if(httpMsg.statusCode != HTTP_STATUS_CODE_OK)
	{
		return GRIB_ERROR;
	}

	return iRes;
}

int Grib_AuthDeviceInfo(char* deviceID, char* deviceInfo)
{
	int  iRes = GRIB_ERROR;
	int  iDBG = gDebugAuth;

	char  httpHead[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};

	Grib_HttpMsgInfo httpMsg;

	char sendBuff[HTTP_MAX_SIZE_SEND_MSG] = {'\0', };
	char recvBuff[HTTP_MAX_SIZE_RECV_MSG] = {'\0', };

	if(STRLEN(deviceID) <= 0)
	{
		GRIB_LOGD("# AUTH DEVICE INFO: IN-VALID PARAM !!!\n");
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# DEVICE ID: %s\n", deviceID);

	if( STRLEN(gAuthServerIp)==0 || (gAuthServerPort==0) )
	{
		Grib_AuthSetServerConfig();
	}

	SNPRINTF(httpBody, sizeof(httpBody), AUTH_BODY_FORMAT_DEVICE_INFO, deviceID);
	SNPRINTF(httpHead, sizeof(httpHead), AUTH_HEAD_FORMAT_DEVICE_INFO, 
				gAuthServerIp, gAuthServerPort, STRLEN(httpBody));

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), "%s%s", httpHead, httpBody);

	MEMSET(&httpMsg, 0x00, sizeof(Grib_HttpMsgInfo));
	httpMsg.serverIP   = gAuthServerIp;
	httpMsg.serverPort = gAuthServerPort;
	httpMsg.LABEL= deviceID;
	httpMsg.sendBuff = sendBuff;
	httpMsg.recvBuff = recvBuff;

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH DEVICE INFO SEND[%d]:\n%s\n", STRLEN(sendBuff), sendBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(&httpMsg);
	if(iRes <= 0)
	{
		GRIB_LOGD("# HTTP MSG SEND ERROR !!!\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH DEVICE INFO RECV[%d]:\n%s\n", STRLEN(recvBuff), recvBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpResParser(&httpMsg);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# HTTP MSG PARSE ERROR !!!\n");
		return iRes;
	}

	if(iDBG)
	{
		GRIB_LOGD("# REQ DEVICE ID : %s\n", httpMsg.LABEL);
		GRIB_LOGD("# RES STATUS MSG: %s [%d]\n", httpMsg.statusMsg, httpMsg.statusCode);	
	}

	if(httpMsg.statusCode != HTTP_STATUS_CODE_OK)
	{
		return GRIB_ERROR;
	}

	if(deviceInfo != NULL)
	{
		MEMSET(deviceInfo, 0x00, sizeof(deviceInfo));
		MEMCPY(deviceInfo, recvBuff, STRLEN(recvBuff));
		deviceInfo[STRLEN(recvBuff)] = '\0';
	}

	return iRes;
}

int Grib_AuthDeviceDeRegi(char* deviceID)
{
	int  iRes = GRIB_ERROR;
	int  iDBG = TRUE;

	char  httpHead[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_SEND_MSG/2] = {'\0',};

	Grib_HttpMsgInfo httpMsg;

	char sendBuff[HTTP_MAX_SIZE_SEND_MSG] = {'\0', };
	char recvBuff[HTTP_MAX_SIZE_RECV_MSG] = {'\0', };

	if(STRLEN(deviceID) <= 0)
	{
		GRIB_LOGD("# AUTH DEVICE DE-REGI: IN-VALID PARAM !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# DEVICE ID: %s\n", deviceID);

	if( STRLEN(gAuthServerIp)==0 || (gAuthServerPort==0) )
	{
		Grib_AuthSetServerConfig();
	}

	SNPRINTF(httpBody, sizeof(httpBody), AUTH_BODY_FORMAT_DEVICE_DEREGI, deviceID);
	SNPRINTF(httpHead, sizeof(httpHead), AUTH_HEAD_FORMAT_DEVICE_DEREGI, 
				gAuthServerIp, gAuthServerPort, STRLEN(httpBody));

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), "%s%s", httpHead, httpBody);

	MEMSET(&httpMsg, 0x00, sizeof(Grib_HttpMsgInfo));
	httpMsg.serverIP   = gAuthServerIp;
	httpMsg.serverPort = gAuthServerPort;
	httpMsg.LABEL= deviceID;
	httpMsg.sendBuff = sendBuff;
	httpMsg.recvBuff = recvBuff;

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH DEVICE DE-REGI SEND[%d]:\n%s\n", STRLEN(sendBuff), sendBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(&httpMsg);
	if(iRes <= 0)
	{
		GRIB_LOGD("# HTTP MSG SEND ERROR !!!\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# AUTH DEVICE DE-REGI RECV[%d]:\n%s\n", STRLEN(recvBuff), recvBuff);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpResParser(&httpMsg);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# HTTP MSG PARSE ERROR !!!\n");
		return iRes;
	}

	if(iDBG)
	{
		GRIB_LOGD("# REQ DEVICE ID : %s\n", httpMsg.LABEL);
		GRIB_LOGD("# RES STATUS MSG: %s [%d]\n", httpMsg.statusMsg, httpMsg.statusCode);	
	}

	if(httpMsg.statusCode != HTTP_STATUS_CODE_OK)
	{
		return GRIB_ERROR;
	}

	return iRes;
}

int Grib_AuthGetPW(char* deviceID, char* devicePW)
{
	int  i = 0;
	int  iRes = GRIB_ERROR;
	int  iDBG = gDebugAuth;

	char recvBuff[HTTP_MAX_SIZE_RECV_MSG] = {'\0', };
	char pswdBuff[SIZE_1K] = {'\0', };

	char* strResponse = NULL;
	char* strTagStart = NULL;
	char* strTagEnd = NULL;
	char* strValue = NULL;
	char* strTemp = NULL;

	if(STRLEN(deviceID) <= 0)
	{
		GRIB_LOGD("# AUTH GET PASSWD: IN-VALID PARAM !!!\n");
		return GRIB_ERROR;
	}

	iRes = Grib_AuthDeviceInfo(deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# AUTH GET PASSWD: GET DEVICE INFO ERROR !!!\n");
		return iRes;
	}

	strResponse = STRDUP(recvBuff);

	strTagStart = "\\\"DEV_PWD\\\" : \\\"";
	strTagEnd	= "\\\" }\" }";
	strTemp = STRSTR(strResponse, strTagStart);
	if(strTemp != NULL)
	{
		char *strValueEnd = NULL;

		//shbaek: Copy Value
		strValue = strTemp+STRLEN(strTagStart);

		strValueEnd = STRSTR(strResponse, strTagEnd);
		strValueEnd[0] = '\0';
		
		STRINIT(pswdBuff, sizeof(pswdBuff));
		STRNCPY(pswdBuff, strValue, STRLEN(strValue));		
	}

	if(iDBG)
	{
		GRIB_LOGD("\n");
		GRIB_LOGD("# AUTH GET DEVICE: %s\n", deviceID);
		GRIB_LOGD("# AUTH GET PASSWD: %s\n", strValue);
	}

	if(devicePW != GRIB_NOT_USED)
	{
		STRINIT(devicePW, sizeof(devicePW));
		STRNCPY(devicePW, pswdBuff, STRLEN(pswdBuff));
		devicePW[STRLEN(pswdBuff)] = '\0';
	}

	return iRes;
}


