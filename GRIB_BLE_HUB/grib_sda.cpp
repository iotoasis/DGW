/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_sda.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int gDebugSda = FALSE;

char gSdaServerIp[GRIB_MAX_SIZE_IP_STR+1];
int  gSdaServerPort;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define __SDA_FUNC__
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
void Grib_SdaSetDebug(int iDebug)
{
	if(iDebug == TRUE)
	{
		gDebugSda = TRUE;
		GRIB_LOGD("# SDA DEBUG: ON\n");
	}

	return;
}

int Grib_SdaSetServerConfig(void)
{
	//3 shbaek: [TBD] Load Config File
	int iRes = GRIB_ERROR;
	Grib_ConfigInfo pConfigInfo;

	MEMSET(&pConfigInfo, 0x00, sizeof(Grib_ConfigInfo));

	iRes = Grib_LoadConfig(&pConfigInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# SDA SERVCER CONFIG: LOAD CONFIG ERROR !!!\n");
		return iRes;
	}

	STRINIT(gSdaServerIp, sizeof(gSdaServerIp));
	STRNCPY(gSdaServerIp, pConfigInfo.sdaServerIP, STRLEN(pConfigInfo.sdaServerIP));

	gSdaServerPort = pConfigInfo.sdaServerPort;

	GRIB_LOGD("# SDA SERVER CONFIG: %s:%d\n", gSdaServerIp, gSdaServerPort);

	return GRIB_SUCCESS;
}

int Grib_SdaDeviceInfoParser(char* recvBuff, char* deviceInfo)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iLoopMax = SIZE_1K;
	int iDBG = FALSE;

	char* strToken		= NULL;
	char* str1Line		= NULL;
	char* strTemp		= NULL;

	char* strKey		= NULL;
	char* strValue		= NULL;
	
	char* strResponse	= NULL;

	if( (recvBuff==NULL) || (deviceInfo==NULL) )
	{
		GRIB_LOGD("# PARAMETER IS NULL !!!\n");
		return GRIB_ERROR;
	}

	STRINIT(deviceInfo, sizeof(deviceInfo));

	strToken = GRIB_CRLN;
	strResponse = STRDUP(recvBuff);
	if(strResponse == NULL)
	{
		GRIB_LOGD("# RECV BUFF COPY ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("===== ===== ===== ===== ===== ===== ===== ===== ===== =====\n");
	do{
		//shbaek: Cut 1 Line
		if(i==0)
		{
			str1Line = STRTOK(strResponse, strToken);
		}
		else
		{
			str1Line = STRTOK(NULL, strToken);
		}

		i++;
		//if(iDBG)GRIB_LOGD("[%03d]%s\n", i, str1Line);
		if(str1Line == NULL)
		{
			if(iDBG)GRIB_LOGD("END LINE: %d\n", i);
			break;
		}

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: [TBD] Find Only Device Information
		strKey = "\"device_information\":\"";
		strTemp = STRSTR(str1Line, strKey);
		if(strTemp != NULL)
		{
			const char* strEndTag = "</rdf:RDF>\\n";
			char *strValueEnd = NULL;

			if(iDBG)GRIB_LOGD("[%03d] KEY[%d]:%s\n", i, STRLEN(strKey), strKey);
			if(iDBG)GRIB_LOGD("[%03d] TEMP VALUE[%d]:%s\n", i, STRLEN(strTemp), strTemp);

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strKey);
			if(iDBG)GRIB_LOGD("[%03d] START VALUE[%d]:%s\n", i, STRLEN(strValue), strValue);
			if(iDBG)GRIB_LOGD("[%03d] END TAG[%d]:%s\n", i, STRLEN(strEndTag), strEndTag);

			strValueEnd = STRSTR(strValue, strEndTag);
			if(strValueEnd == NULL)
			{//shbaek: Not Found End Tag
				GRIB_LOGD("[%03d] NOT FOUND TAG TAG !!!\n", i);
				break;
			}
			strValueEnd[STRLEN(strEndTag)] = NULL;

			STRNCPY(deviceInfo, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] LAST VALUE:[%s]\n", i, deviceInfo);
			break;//3 shbaek: Search More? break -> continue
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
	}while(i < iLoopMax);
	if(iDBG)GRIB_LOGD("===== ===== ===== ===== ===== ===== ===== ===== ===== =====\n");

FINAL:
	if(strResponse!=NULL)FREE(strResponse);

	return GRIB_DONE;
}

int Grib_SdaGetDeviceInfo(char* deviceID, char* deviceInfo)
{
	int iDBG = gDebugSda;
	int iRes = GRIB_ERROR;
	
	Grib_HttpMsgInfo httpMsg;

	char sendBuff[HTTP_MAX_SIZE_SEND_MSG]	= {'\0', };
	char recvBuff[SDA_MAX_DEVICE_INFO] 		= {'\0', };

	if( (deviceID==NULL) || (deviceInfo==NULL) )
	{
		GRIB_LOGD("# PARAMETER IS NULL !!!\n");
		return GRIB_ERROR;
	}

	if( STRLEN(gSdaServerIp)==0 || (gSdaServerPort==0) )
	{
		Grib_SdaSetServerConfig();
	}

	STRINIT(sendBuff, sizeof(sendBuff));
	STRINIT(recvBuff, sizeof(recvBuff));

	SNPRINTF(sendBuff, sizeof(sendBuff), SDA_GET_DEVICE_INFO_FORMAT_HTTP, 
		deviceID, gSdaServerIp, gSdaServerPort);

	MEMSET(&httpMsg, 0x00, sizeof(Grib_HttpMsgInfo));
	httpMsg.serverIP   = gSdaServerIp;
	httpMsg.serverPort = gSdaServerPort;
	httpMsg.LABEL= deviceID;
	httpMsg.sendBuff = sendBuff;
	httpMsg.recvBuff = recvBuff;

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# SDA GET DEV INFO SEND[%d]:\n%s", STRLEN(sendBuff), sendBuff);
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
		GRIB_LOGD("# SDA GET DEV INFO RECV[%d]:\n%s", STRLEN(recvBuff), recvBuff);
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
		GRIB_LOGD("# SDA REQ DEVICE ID : %s\n", httpMsg.LABEL);
		GRIB_LOGD("# SDA RES STATUS MSG: %s [%d]\n", httpMsg.statusMsg, httpMsg.statusCode);	
	}

	if(httpMsg.statusCode != HTTP_STATUS_CODE_OK)
	{
		return GRIB_ERROR;
	}

	iRes = Grib_SdaDeviceInfoParser(httpMsg.recvBuff, deviceInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# DEVICE INFO PARSE ERROR !!!\n");
		return iRes;
	}

	if(iDBG)GRIB_LOGD("# SDA DEVICE INFO[%d]:\n%s\n", STRLEN(deviceInfo), deviceInfo);

	return 	iRes;
}


