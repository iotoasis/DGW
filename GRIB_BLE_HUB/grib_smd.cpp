/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_smd.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int gDebugSmd = FALSE;

char gSmdServerIp[GRIB_MAX_SIZE_IP_STR+1];
int  gSmdServerPort;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define __SMD_FUNC__
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
void Grib_SmdSetDebug(int iDebug)
{
	if(iDebug == TRUE)
	{
		gDebugSmd = TRUE;
		GRIB_LOGD("# SMD DEBUG: ON\n");
	}

	return;
}

int Grib_SmdSetServerConfig(void)
{
	int iRes = GRIB_ERROR;

	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("LOAD CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	STRINIT(gSmdServerIp, sizeof(gSmdServerIp));
	STRNCPY(gSmdServerIp, pConfigInfo->smdServerIP, STRLEN(pConfigInfo->smdServerIP));

	gSmdServerPort = pConfigInfo->smdServerPort;

	GRIB_LOGD("# SMD SERVER CONFIG: %s:%d\n", gSmdServerIp, gSmdServerPort);

	return GRIB_SUCCESS;
}

int Grib_SmdDeviceInfoParser(char* recvBuff, char* smdBuff)
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

	if( (recvBuff==NULL) || (smdBuff==NULL) )
	{
		GRIB_LOGD("# PARAMETER IS NULL !!!\n");
		return GRIB_ERROR;
	}

	STRINIT(smdBuff, sizeof(smdBuff));

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

			STRNCPY(smdBuff, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] LAST VALUE:[%s]\n", i, smdBuff);
			break;//3 shbaek: Search More? break -> continue
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
	}while(i < iLoopMax);
	if(iDBG)GRIB_LOGD("===== ===== ===== ===== ===== ===== ===== ===== ===== =====\n");

FINAL:
	if(strResponse!=NULL)FREE(strResponse);

	return GRIB_DONE;
}

int Grib_SmdGetDeviceInfo(char* deviceID, char* smdBuff)
{
	int iDBG = gDebugSmd;
	int iRes = GRIB_ERROR;
	
	Grib_HttpMsgInfo httpMsg;

	char sendBuff[HTTP_MAX_SIZE_SEND_MSG]	= {'\0', };
	char recvBuff[HTTP_MAX_SIZE_RECV_MSG] 	= {'\0', };

	if( (deviceID==NULL) || (smdBuff==NULL) )
	{
		GRIB_LOGD("# PARAMETER IS NULL !!!\n");
		return GRIB_ERROR;
	}

	if( STRLEN(gSmdServerIp)==0 || (gSmdServerPort==0) )
	{
		Grib_SmdSetServerConfig();
	}

	STRINIT(sendBuff, sizeof(sendBuff));
	STRINIT(recvBuff, sizeof(recvBuff));

	SNPRINTF(sendBuff, sizeof(sendBuff), SMD_GET_DEVICE_INFO_FORMAT_HTTP, 
		deviceID, gSmdServerIp, gSmdServerPort);

	MEMSET(&httpMsg, 0x00, sizeof(Grib_HttpMsgInfo));
	httpMsg.serverIP   = gSmdServerIp;
	httpMsg.serverPort = gSmdServerPort;
	httpMsg.LABEL= deviceID;
	httpMsg.sendBuff = sendBuff;
	httpMsg.recvBuff = recvBuff;

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# SMD GET DEV INFO SEND[%d]:\n%s", STRLEN(sendBuff), sendBuff);
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
		GRIB_LOGD("# SMD GET DEV INFO RECV[%d]:\n%s", STRLEN(recvBuff), recvBuff);
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
		GRIB_LOGD("# SMD REQ DEVICE ID : %s\n", httpMsg.LABEL);
		GRIB_LOGD("# SMD RES STATUS MSG: %s [%d]\n", httpMsg.statusMsg, httpMsg.statusCode);	
	}

	if(httpMsg.statusCode != HTTP_STATUS_CODE_OK)
	{
		return GRIB_ERROR;
	}

	iRes = Grib_SmdDeviceInfoParser(httpMsg.recvBuff, smdBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# DEVICE INFO PARSE ERROR !!!\n");
		return iRes;
	}

	if(iDBG)GRIB_LOGD("# SMD DEVICE INFO[%d]:\n%s\n", STRLEN(smdBuff), smdBuff);

	return 	iRes;
}


