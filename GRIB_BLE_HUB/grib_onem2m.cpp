/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "include/grib_define.h"
#include "include/grib_onem2m.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

char gServerIp[ONEM2M_MAX_SIZE_IP_STR+1];
int  gServerPort;
int  gSocketFD;

int  gDebugOneM2M;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define __HTTP_FUNC__
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_SetServerConfig(void)
{
	//3 shbaek: [TBD] Load Config File
	int iRes = GRIB_ERROR;
	Grib_ConfigInfo pConfigInfo;

	MEMSET(&pConfigInfo, 0x00, sizeof(Grib_ConfigInfo));

	iRes = Grib_LoadConfig(&pConfigInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# SERVCER CONFIG: LOAD CONFIG ERROR !!!\n");
		return iRes;
	}

	STRINIT(gServerIp, sizeof(gServerIp));
	STRNCPY(gServerIp, pConfigInfo.platformServerIP, STRLEN(pConfigInfo.platformServerIP));

	gServerPort = pConfigInfo.platformServerPort;

	gDebugOneM2M = pConfigInfo.debugOneM2M;

	GRIB_LOGD("# SERVER CONFIG: %s:%d\n", gServerIp, gServerPort);

	return GRIB_SUCCESS;
}

int Grib_HttpConnect(char* serverIP, int serverPort)
{
	int i 		= 0;
	int iFD		= GRIB_ERROR;
	int iRes	= GRIB_ERROR;

	int iSize	= 0;
	int iCount	= 0;
	int iError	= 0;

	int nonSocketStat = 0;
	int orgSocketStat = 0;

	fd_set  readSet;
	fd_set  writeSet;

	timeval recvTimeOut;
	timeval connTimeOut;
	struct sockaddr_in serverAddr;

	iFD = socket(AF_INET, SOCK_STREAM, GRIB_NOT_USED);
	if(iFD < 0)
	{
		GRIB_LOGD("# SOCKET OPEN FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}

	//shbaek: Get Socket Stat
	orgSocketStat = fcntl(iFD, F_GETFL, NULL);
	if(orgSocketStat < 0)
	{
		GRIB_LOGD("# GET FILE STAT FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		goto ERROR;
	}

	//shbaek: Set Non-Block Stat
	nonSocketStat = orgSocketStat | O_NONBLOCK;
	iRes = fcntl(iFD, F_SETFL, nonSocketStat);
	if(iRes < 0)
	{
		GRIB_LOGD("# SET NONBLOCK STAT FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		goto ERROR;
	}

	bzero(&serverAddr, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = inet_addr(serverIP);
	serverAddr.sin_port = htons(serverPort);

	//shbaek: Connect
	iRes = connect(iFD, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
	if(iRes < 0)
	{
		if(LINUX_ERROR_NUM != EINPROGRESS)
		{//shbaek: Connect Error
			GRIB_LOGD("# HTTP CONNECT FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
			goto ERROR;
		}
	}
	else
	{//shbaek: One Shot Connected -> Restore Socket Stat
		fcntl(iFD, F_SETFL, orgSocketStat);
		return iFD;
	}

    FD_ZERO(&readSet);
    FD_SET(iFD, &readSet);
    writeSet = readSet;
	connTimeOut.tv_sec  = HTTP_TIME_OUT_SEC_CONNECT;
	connTimeOut.tv_usec = GRIB_NOT_USED;

	//shbaek: Wait Read & Write Set
	iCount = select(iFD+1, &readSet, &writeSet, GRIB_NOT_USED, &connTimeOut);
	if(iCount == 0)
	{
		//shbaek: Time Out
		LINUX_ERROR_NUM = ETIMEDOUT;
		GRIB_LOGD("# SELECT TIME OUT: %d SEC\n", HTTP_TIME_OUT_SEC_CONNECT);
		goto ERROR;
	}

	//shbaek: Check Read & Write Set
    if( FD_ISSET(iFD, &readSet) || FD_ISSET(iFD, &writeSet) ) 
    {
    	iSize = sizeof(int);
		iCount = getsockopt(iFD, SOL_SOCKET, SO_ERROR, &iError, (socklen_t *)&iSize);
		if(iCount < 0)
		{
			GRIB_LOGD("# GET SOCKET OPTION FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
			goto ERROR;
		}
    } 
    else 
    {
		//shbaek: Nobody Touched
		GRIB_LOGD("# NOBODY TOCHED SOCKET\n");
        goto ERROR;
    }

	//shbaek: Restore Socket Stat
    fcntl(iFD, F_SETFL, orgSocketStat);
    if(iError) 
    { 
        LINUX_ERROR_NUM = iError; 
		GRIB_LOGD("# SET FILE STAT FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
        goto ERROR; 
    }

	//shbaek: Set Receive Time Out
	recvTimeOut.tv_sec  = HTTP_TIME_OUT_SEC_RECEIVE;
	recvTimeOut.tv_usec = GRIB_NOT_USED;
	setsockopt(iFD, SOL_SOCKET, SO_RCVTIMEO, (char*)&recvTimeOut, sizeof(timeval));

	return iFD;

ERROR:
	if(0 < iFD)close(iFD);
	return GRIB_ERROR;
}

int Grib_HttpSendMsg(char* pSendMsg, char* pRecvMsg)
{
	int iRes	= GRIB_ERROR;
	int iCount	= GRIB_ERROR;
	int iTotal	= GRIB_ERROR;
	int iDBG 	= gDebugOneM2M;

	int iFD 	= 0;

	int iTimeCheck = gDebugOneM2M;
	time_t sysTimer;
	struct tm *sysTime;

	//shbaek: Check Server Info
	if( (gServerIp==NULL) || (gServerPort==0) )
	{
		Grib_SetServerConfig();
	}

	//shbaek: Server Connect.
	iFD = Grib_HttpConnect(gServerIp, gServerPort);
	if(iFD == GRIB_ERROR)
	{
		GRIB_LOGD("SERVER CONNECT ERROR: %s:%d\n", gServerIp, gServerPort);
		goto FINAL;
	}

	if(STRLEN(pSendMsg) <= 0)
	{
		GRIB_LOGD("SEND MSG BUFFER EMPTY\n");
		goto FINAL;
	}

	iCount = send(iFD, pSendMsg, STRLEN(pSendMsg), GRIB_NOT_USED);
	if(iCount <= 0)
	{
		GRIB_LOGD("SEND FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		goto FINAL;
	}

	if(pRecvMsg == NULL)
	{
		//shbaek: Do Not Waiting for Response.
		close(iFD);
		return GRIB_DONE;
	}

	MEMSET(pRecvMsg, GRIB_INIT, ONEM2M_MAX_SIZE_RECV_MSG+1);
	iCount = -1;
	iTotal = 0;

	if(iTimeCheck)
	{
		sysTimer = time(NULL);
		sysTime  = localtime(&sysTimer);
		GRIB_LOGD("RECV TIME WAIT: %02d:%02d:%02d\n", sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
	}

	do{
		iCount = recv(iFD, (pRecvMsg+iTotal), (ONEM2M_MAX_SIZE_RECV_MSG-iTotal), GRIB_NOT_USED);
		if(iCount < 0)
		{
			GRIB_LOGD("RECV FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
			break;
		}
		else if(iCount == 0)
		{
			if(iDBG)GRIB_LOGD("RECV DONE\n");
			break;
		}
		else
		{
			iTotal += iCount;
			if(iDBG)GRIB_LOGD("RECV:%d TOTAL:%d\n", iCount, iTotal);
		}
	}while(iTotal < ONEM2M_MAX_SIZE_RECV_MSG);

	if(iTimeCheck)
	{
		sysTimer = time(NULL);
		sysTime  = localtime(&sysTimer);
		if(iDBG)GRIB_LOGD("RECV TIME DONE  : %02d:%02d:%02d\n", sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
	}

FINAL:
	if(0 < iFD)	close(iFD);
	else return GRIB_ERROR;

	return iTotal;
}

int Grib_HttpResParser(OneM2M_ResParam *pResParam)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iLoopMax = 128;
	int iDBG = FALSE;

	char* strToken		= NULL;
	char* str1Line		= NULL;
	char* strResponse	= NULL;

	char* strHttpVer	= NULL;
	char* strCodeNum	= NULL;
	char* strCodeMsg	= NULL;

	if( (pResParam==NULL) || (pResParam->http_RecvData==NULL) )
	{
		GRIB_LOGD("# PARAM IS NULL\n");
		return GRIB_ERROR;
	}

	//shbaek: Set Init.
	pResParam->http_ResNum = HTTP_STATUS_CODE_UNKNOWN;
	STRINIT(pResParam->http_ResMsg, sizeof(pResParam->http_ResMsg));

	strToken = GRIB_CRLN;
	strResponse = STRDUP(pResParam->http_RecvData);
	if(strResponse == NULL)
	{
		GRIB_LOGD("# RESPONSE COPY ERROR\n");
		goto FINAL;
	}

	str1Line = STRTOK(strResponse, strToken);
	if(str1Line == NULL)
	{
		GRIB_LOGD("# 1ST LINE IS NULL !!!\n");
		goto FINAL;
	}

	iRes = STRNCMP(str1Line, HTTP_VERSION_1P1, STRLEN(HTTP_VERSION_1P1));
	if(iRes != 0)
	{//shbaek: is not Same? Something Wrong
		GRIB_LOGD("# 1ST LINE IS WRONG: %s\n", str1Line);
		goto FINAL;
	}

	//shbaek: HTTP/1.1 200 OK
	//shbaek: HTTP/1.1 404 Not Found
	//shbaek: [1:Space] [3:Number Count]

	strHttpVer = str1Line;
	strCodeNum = strHttpVer+STRLEN(HTTP_VERSION_1P1)+1;
	strCodeMsg = strCodeNum+3+1;

	*(strCodeNum-1) = NULL;
	*(strCodeMsg-1) = NULL;

	pResParam->http_ResNum = ATOI(strCodeNum);

	STRINIT(pResParam->http_ResMsg, sizeof(pResParam->http_ResMsg));
	STRNCPY(pResParam->http_ResMsg, strCodeMsg, STRLEN(strCodeMsg));

	// TODO: shbaek: Check & Exception Handle?

FINAL:
	if(strResponse!=NULL)FREE(strResponse);

	return GRIB_DONE;
}


int Grib_OneM2MResParser(OneM2M_ResParam *pResParam)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iLoopMax = 128;
	int iDBG = FALSE;

	char* strToken		= NULL;
	char* str1Line		= NULL;
	char* strTemp		= NULL;

	char* strKey		= NULL;
	char* strValue		= NULL;
	
	char* strResponse	= NULL;

	if( (pResParam==NULL) || (pResParam->http_RecvData==NULL) )
	{
		GRIB_LOGD("# PARAM IS NULL\n");
		return GRIB_ERROR;
	}

	STRINIT(pResParam->xM2M_RsrcID, sizeof(pResParam->xM2M_RsrcID));
	STRINIT(pResParam->xM2M_PrntID, sizeof(pResParam->xM2M_PrntID));

	strToken = GRIB_CRLN;
	strResponse = STRDUP(pResParam->http_RecvData);
	if(strResponse == NULL)
	{
		GRIB_LOGD("# RESPONSE COPY ERROR\n");
		goto FINAL;
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
		if(iDBG)GRIB_LOGD("[%03d]%s\n", i, str1Line);
		if(str1Line == NULL)
		{
			if(iDBG)GRIB_LOGD("END LINE: %d\n", i);
			break;
		}

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: [TBD] Find Only Resource ID Key
		strKey = "\"ri\" : \"";
		strTemp = STRSTR(str1Line, strKey);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strKey);
			if(iDBG)GRIB_LOGD("[%03d] KEY:[%s] TEMP VALUE:[%s]\n", i, strKey, strValue);

			strValueEnd = STRCHR(strValue, '"');
			strValueEnd[0] = NULL;

			STRNCPY(pResParam->xM2M_RsrcID, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] LAST VALUE:[%s]\n", i, pResParam->xM2M_RsrcID);
			continue;
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: [TBD] Find Only Resource ID Key
		strKey = "\"pi\" : \"";
		strTemp = STRSTR(str1Line, strKey);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strKey);
			if(iDBG)GRIB_LOGD("[%03d] KEY:[%s] TEMP VALUE:[%s]\n", i, strKey, strValue);

			strValueEnd = STRCHR(strValue, '"');
			strValueEnd[0] = NULL;

			STRNCPY(pResParam->xM2M_PrntID, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] LAST VALUE:[%s]\n", i, pResParam->xM2M_PrntID);
			break;//3 shbaek: Search More?
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

	}while(i < iLoopMax);
	if(iDBG)GRIB_LOGD("===== ===== ===== ===== ===== ===== ===== ===== ===== =====\n");

FINAL:
	if(strResponse!=NULL)FREE(strResponse);

	return GRIB_DONE;
}

#define __ONEM2M_DEVICE_ID_FUNC__
//2 shbaek: NEED: xM2M_NM
int Grib_AppEntityCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	char  xM2M_AttrLBL[ONEM2M_MAX_SIZE_BRIEF] = {'\0',};
	char  xM2M_AttrAPN[ONEM2M_MAX_SIZE_BRIEF] = {'\0',};
	char  xM2M_AttrAPI[ONEM2M_MAX_SIZE_BRIEF] = {'\0',};
	char* xM2M_AttrRR = "FALSE";

	STRINIT(pReqParam->xM2M_Origin, sizeof(pReqParam->xM2M_Origin));
	STRNCPY(pReqParam->xM2M_Origin, pReqParam->xM2M_NM, STRLEN(pReqParam->xM2M_NM));
	pReqParam->xM2M_Origin[STRLEN(pReqParam->xM2M_NM)+1] = NULL;

	SNPRINTF(xM2M_AttrLBL, sizeof(xM2M_AttrLBL), "%s_AppEntity", pReqParam->xM2M_Origin);
	SNPRINTF(xM2M_AttrAPN, sizeof(xM2M_AttrAPN), "%s_AppName",	  pReqParam->xM2M_Origin);
	SNPRINTF(xM2M_AttrAPI, sizeof(xM2M_AttrAPI), "%s_AppID",     pReqParam->xM2M_Origin);

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqAppEntityCreate", pReqParam->xM2M_Origin);

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_APP_ENTITY_CREATE,
				xM2M_AttrLBL, xM2M_AttrAPN, xM2M_AttrAPI, gServerIp, gServerPort, xM2M_AttrRR);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_APP_ENTITY_CREATE,
				gServerIp, gServerPort, ONEM2M_RESOURCE_TYPE_APP_ENTITY, STRLEN(httpBody), 
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID, pReqParam->xM2M_NM);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	GRIB_LOGD("# APP ENTITY CREATE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# APP ENTITY CREATE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_CREATED)
	{//shbaek: Need Not Continue.
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{//shbaek: Already Exist is Not Error.
			GRIB_LOGD("# ALREADY EXIST %s\n", pReqParam->xM2M_NM);
			return GRIB_ERROR;
		}
		else
		{
			GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
			return GRIB_ERROR;	
		}
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}
	GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_Origin
int Grib_AppEntityRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqAppEntityRetrieve", pReqParam->xM2M_Origin);

	STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
	SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s", pReqParam->xM2M_Origin);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_APP_ENTITY_RETRIEVE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# APP ENTITY RETRIEVE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# APP ENTITY RETRIEVE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_OK)
	{//shbaek: Need Not Continue.
		GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
		return GRIB_ERROR;	
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}
	//GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	//GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_Origin
int Grib_AppEntityDelete(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqAppEntityDelete", pReqParam->xM2M_Origin);

	STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
	SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s", pReqParam->xM2M_Origin);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_APP_ENTITY_DELETE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# APP ENTITY DELETE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# APP ENTITY DELETE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_OK)
	{//shbaek: Need Not Continue.
		GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
		return GRIB_ERROR;	
	}

	return GRIB_SUCCESS;
}

#define __ONEM2M_CONTAINER_FUNC__

//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_NM
int Grib_ContainerCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	char  httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  xM2M_AttrLBL[ONEM2M_MAX_SIZE_URI] = {'\0',};
	char  xM2M_AttrET[ONEM2M_MAX_SIZE_BRIEF]  = {'\0',}; //shbaek: expirationTime

	unsigned long long int xM2M_AttrMNI = 0; //shbaek: maxNrOfInstances
	unsigned long long int xM2M_AttrMBS = 0; //shbaek: maxByteSize
	unsigned long long int xM2M_AttrMIA = 0; //shbaek: maxInstanceAge

	char* strDate = "20191231";
	char* strTime = "122359";
	char* pTemp = NULL;


	xM2M_AttrMNI = 100;
	xM2M_AttrMBS = 1024000ULL;
	xM2M_AttrMIA = 36000;

	STRNCPY(xM2M_AttrLBL, pReqParam->xM2M_URI, STRLEN(pReqParam->xM2M_URI));
	//SNPRINTF(xM2M_AttrLBL, sizeof(xM2M_AttrLBL), "%s_%s_Container", pReqParam->xM2M_Origin, pReqParam->xM2M_NM);
	SNPRINTF(xM2M_AttrET,  sizeof(xM2M_AttrET),  "%sT%s",        strDate, strTime);

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContainerCreate_%s", pReqParam->xM2M_Origin, pReqParam->xM2M_NM);

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_CONTAINER_CREATE,
				xM2M_AttrLBL, xM2M_AttrET, xM2M_AttrMNI, xM2M_AttrMBS, xM2M_AttrMIA);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_CREATE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, ONEM2M_RESOURCE_TYPE_CONTAINER, STRLEN(httpBody), 
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID, pReqParam->xM2M_NM);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	GRIB_LOGD("# CONTAINER CREATE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# CONTAINER CREATE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_CREATED)
	{//shbaek: Need Not Continue.
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{//shbaek: Already Exist is Not Error.
			GRIB_LOGD("# ALREADY EXIST %s\n", pReqParam->xM2M_NM);
			return GRIB_ERROR;
		}
		else
		{
			GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
			return GRIB_ERROR;	
		}
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}
	GRIB_LOGD("# RESOURCE ID : [%s]\n", pResParam->xM2M_RsrcID);
	GRIB_LOGD("# CONENT VALUE: [%s]\n", pResParam->xM2M_Content);
	
	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContainerRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContainerRetrieve", pReqParam->xM2M_Origin);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_RETRIEVE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# CONTAINER RETRIEVE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# CONTAINER RETRIEVE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_OK)
	{//shbaek: Need Not Continue.
		GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
		return GRIB_ERROR;	
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
		GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);
	}

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContainerDelete(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContainerDelete", pReqParam->xM2M_Origin);

	STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
	SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s", pReqParam->xM2M_Origin);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_DELETE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# CONTAINER DELETE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# CONTAINER DELETE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_OK)
	{//shbaek: Need Not Continue.
		GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
		return GRIB_ERROR;	
	}

	return GRIB_SUCCESS;
}

#define __ONEM2M_POLLING_FUNC__
//2 shbaek: NEED: xM2M_Origin
int Grib_PollingChannelCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	char  httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  xM2M_AttrLBL[ONEM2M_MAX_SIZE_BRIEF] = {NULL,};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqPollingChannelCreate", pReqParam->xM2M_Origin);

	STRINIT(pReqParam->xM2M_NM, sizeof(pReqParam->xM2M_NM));
	STRNCPY(pReqParam->xM2M_NM, ONEM2M_URI_CONTENT_POLLING_CHANNEL, STRLEN(ONEM2M_URI_CONTENT_POLLING_CHANNEL));

	SNPRINTF(xM2M_AttrLBL, sizeof(xM2M_AttrLBL), "%s_PollingChannel", pReqParam->xM2M_Origin);

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_POLLING_CHANNEL_CREATE, xM2M_AttrLBL);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_CREATE,
				pReqParam->xM2M_Origin, gServerIp, gServerPort, ONEM2M_RESOURCE_TYPE_POLLING_CHANNEL, STRLEN(httpBody), 
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID, pReqParam->xM2M_NM);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	GRIB_LOGD("# POLLING CHANNEL CREATE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# POLLING CHANNEL CREATE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_CREATED)
	{//shbaek: Need Not Continue.
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{//shbaek: Already Exist is Not Error.
			GRIB_LOGD("# ALREADY EXIST %s\n", pReqParam->xM2M_NM);
			return GRIB_ERROR;
		}
		else
		{
			GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
			return GRIB_ERROR;	
		}
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}
	//GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	//GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_Origin xM2M_Func
int Grib_SubsciptionCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	char  httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  xM2M_AttrLBL[ONEM2M_MAX_SIZE_BRIEF] = {'\0',};
	char  xM2M_AttrET[ONEM2M_MAX_SIZE_BRIEF]  = {'\0',};
	char  xM2M_AttrNU[ONEM2M_MAX_SIZE_URI] = {'\0',};

	char* strDate = "20191231";
	char* strTime = "122359";

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqSubsciptionCreate", pReqParam->xM2M_Origin);

	STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
	SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s/%s/%s", 
		pReqParam->xM2M_Origin, pReqParam->xM2M_Func, ONEM2M_URI_CONTENT_EXECUTE);

	STRINIT(pReqParam->xM2M_NM, sizeof(pReqParam->xM2M_NM));
	STRNCPY(pReqParam->xM2M_NM, ONEM2M_URI_CONTENT_SUBSCRIPTION, STRLEN(ONEM2M_URI_CONTENT_SUBSCRIPTION));

	SNPRINTF(xM2M_AttrLBL, sizeof(xM2M_AttrLBL), "%s_Subsciption", pReqParam->xM2M_Origin);
	SNPRINTF(xM2M_AttrET,  sizeof(xM2M_AttrET),  "%sT%s", strDate, strTime);
	SNPRINTF(xM2M_AttrNU, sizeof(xM2M_AttrNU), "http://%s:%d/herit-in/herit-cse/%s/pollingchannel/pcu",
		gServerIp, gServerPort, pReqParam->xM2M_Origin);

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_SUBSCRIPTION_CREATE, 
		xM2M_AttrLBL, xM2M_AttrET, xM2M_AttrNU);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_SUBSCRIPTION_CREATE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, ONEM2M_RESOURCE_TYPE_SUBSCRIPTION, STRLEN(httpBody), 
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID, pReqParam->xM2M_NM);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	GRIB_LOGD("# SUBSCRIPTION CREATE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
	GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# SUBSCRIPTION CREATE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_CREATED)
	{//shbaek: Need Not Continue.
		GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
		return GRIB_ERROR;	
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}
	//GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	//GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);

	return GRIB_SUCCESS;
}

#define __ONEM2M_CONTENT_INSTANCE_FUNC__

//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_CNF[If NULL, Set Default "text/plain:0"], xM2M_CON
int Grib_ContentInstanceCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  httpHead[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};
	char  httpBody[ONEM2M_MAX_SIZE_SEND_MSG/2] = {'\0',};

	char  xM2M_AttrLBL[ONEM2M_MAX_SIZE_BRIEF] = {NULL,};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContentInstanceCreate", pReqParam->xM2M_Origin);

	if(pReqParam->xM2M_CNF == NULL)
	{//shbaek: Default Type
		STRINIT(pReqParam->xM2M_CNF, sizeof(pReqParam->xM2M_CNF));
		SNPRINTF(pReqParam->xM2M_CNF, sizeof(pReqParam->xM2M_CNF), "%s:0", HTTP_CONTENT_TYPE_TEXT);
	}

	SNPRINTF(xM2M_AttrLBL, sizeof(xM2M_AttrLBL), "%s_ReportData", pReqParam->xM2M_Origin);

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_CONTENT_INSTANCE_CREATE,
				xM2M_AttrLBL, pReqParam->xM2M_CNF, pReqParam->xM2M_CON);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_CREATE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, ONEM2M_RESOURCE_TYPE_CONTENT_INSTANCE, STRLEN(httpBody), 
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# CONTENT INSTANCE CREATE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	if(pResParam == NULL)
	{
		GRIB_LOGD("# CONTENT INSTANCE CREATE: IGNORE RESPONSE\n");
		iRes = Grib_HttpSendMsg(pReqParam->http_SendData, NULL);
		return iRes;
	}

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# CONTENT INSTANCE CREATE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_CREATED)
	{//shbaek: Need Not Continue.
		GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
		return GRIB_ERROR;	
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}
	//GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	//GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContentInstanceRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;
	char httpHead[ONEM2M_MAX_SIZE_SEND_MSG] = {'\0',};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContentInstanceRetrieve", pReqParam->xM2M_Origin);

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_RETRIEVE,
				pReqParam->xM2M_URI, gServerIp, gServerPort, pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# CONTENT INST RETRIEVE[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# CONTENT INST RETRIEVE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# RECV MSG[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_OK)
	{//shbaek: Need Not Continue.
		GRIB_LOGD("# HTTP RESULT: %s[%d]\n", pResParam->http_ResMsg, pResParam->http_ResNum);
		return GRIB_ERROR;	
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}

	//GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	//GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);

	return GRIB_SUCCESS;
}

#define __ONEM2M_LONG_POLLING_FUNC__

int Grib_LongPollingResParser(OneM2M_ResParam *pResParam)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iLoopMax = 128;
	int iDBG = FALSE;

	char* strToken		= NULL;
	char* str1Line		= NULL;
	char* strTemp		= NULL;

	char* strTagStart	= NULL;
	char* strTagEnd	= NULL;
	char* strValue		= NULL;

	char* strResponse	= NULL;

	if( (pResParam==NULL) || (pResParam->http_RecvData==NULL) )
	{
		GRIB_LOGD("# PARAM IS NULL\n");
		return GRIB_ERROR;
	}

	strToken = GRIB_CRLN;
	strResponse = STRDUP(pResParam->http_RecvData);
	if(strResponse == NULL)
	{
		GRIB_LOGD("# RESPONSE COPY ERROR\n");
		goto FINAL;
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
		if(iDBG)GRIB_LOGD("[%03d]%s\n", i, str1Line);
		if(str1Line == NULL)
		{
			if(iDBG)GRIB_LOGD("END LINE: %d\n", i);
			break;
		}

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: Find Resource ID Tag
		strTagStart = "<ri>";
		strTagEnd	= "</ri>";
		strTemp = STRSTR(str1Line, strTagStart);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strTagStart);
			if(iDBG)GRIB_LOGD("[%03d] TAG:[%s] TEMP VALUE:[%s]\n", i, strTagStart, strValue);

			strValueEnd = STRSTR(str1Line, strTagEnd);
			strValueEnd[0] = NULL;
			
			STRINIT(pResParam->xM2M_RsrcID, sizeof(pResParam->xM2M_RsrcID));
			STRNCPY(pResParam->xM2M_RsrcID, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] RID:[%s]\n", i, pResParam->xM2M_RsrcID);
			continue;//3 shbaek: Next Line
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: Find Parents ID Tag
		strTagStart = "<pi>";
		strTagEnd	= "</pi>";
		strTemp = STRSTR(str1Line, strTagStart);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strTagStart);
			if(iDBG)GRIB_LOGD("[%03d] TAG:[%s] TEMP VALUE:[%s]\n", i, strTagStart, strValue);

			strValueEnd = STRSTR(str1Line, strTagEnd);
			strValueEnd[0] = NULL;
			
			STRINIT(pResParam->xM2M_PrntID, sizeof(pResParam->xM2M_PrntID));
			STRNCPY(pResParam->xM2M_PrntID, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] PID:[%s]\n", i, pResParam->xM2M_PrntID);
			continue;//3 shbaek: Next Line
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: Find Content Tag
		strTagStart = "<con>";
		strTagEnd	= "</con>";
		strTemp = STRSTR(str1Line, strTagStart);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strTagStart);
			if(iDBG)GRIB_LOGD("[%03d] TAG:[%s] TEMP VALUE:[%s]\n", i, strTagStart, strValue);

			strValueEnd = STRSTR(str1Line, strTagEnd);
			strValueEnd[0] = NULL;

			STRINIT(pResParam->xM2M_Content, sizeof(pResParam->xM2M_Content));
			STRNCPY(pResParam->xM2M_Content, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] CON:[%s]\n", i, pResParam->xM2M_Content);
			break;//3 shbaek: Search More?
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####



	}while(i < iLoopMax);
	if(iDBG)GRIB_LOGD("===== ===== ===== ===== ===== ===== ===== ===== ===== =====\n");

FINAL:
	if(strResponse!=NULL)FREE(strResponse);

	return GRIB_DONE;
}

//2 shbaek: NEED: xM2M_Origin
int Grib_LongPolling(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;
	char httpHead[ONEM2M_MAX_SIZE_SEND_MSG] = {'\0',};

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqLongPolling", pReqParam->xM2M_Origin);

	if(STRLEN(pReqParam->xM2M_URI) < STRLEN(pReqParam->xM2M_Origin))
	{
		STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
		SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s/%s/%s", 
			pReqParam->xM2M_Origin, ONEM2M_URI_CONTENT_POLLING_CHANNEL, ONEM2M_URI_CONTENT_PCU);

		GRIB_LOGD("# LONG POLLING URI: %s\n", pReqParam->xM2M_URI);
	}

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_LONG_POLLING,
				pReqParam->xM2M_URI, gServerIp, gServerPort, pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
		GRIB_LOGD("# LONG POLLING[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD("----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----\n");
	}

	iRes = Grib_HttpSendMsg(pReqParam->http_SendData, pResParam->http_RecvData);
	if(iRes <= 0)
	{
		GRIB_LOGD("# LONG POLLING ERROR\n");
		return GRIB_ERROR;
	}

	Grib_HttpResParser(pResParam);
	if(pResParam->http_ResNum != HTTP_STATUS_CODE_OK)
	{//shbaek: Need Not Continue.
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_REQUEST_TIME_OUT)
		{
			if(iDBG)GRIB_LOGD("# %s-xM2M: LONG POLLING TIME OUT: %s[%d]\n", pReqParam->xM2M_Origin, pResParam->http_ResMsg, pResParam->http_ResNum);
		}
		else
		{
			GRIB_LOGD("# %s-xM2M: LONG POLLING HTTP RESULT: %s[%d]\n", pReqParam->xM2M_Origin, pResParam->http_ResMsg, pResParam->http_ResNum);
		}
		return GRIB_ERROR;	
	}

	iRes = Grib_LongPollingResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}
	if(iDBG)
	{
		GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
		GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);
		GRIB_LOGD("# CONTENT    : [%s]\n", pResParam->xM2M_Content);
	}
	return GRIB_SUCCESS;
}

#define __ONEM2M_ETC_FUNC__

int Grib_CreateOneM2MTree(Grib_DbRowDeviceInfo* pRowDeviceInfo)
{
	int i		= 0;
	int iRes	= GRIB_ERROR;
	int useExe	= FALSE;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	if( (pRowDeviceInfo==NULL) || (pRowDeviceInfo->deviceID==NULL))
	{
		GRIB_LOGD("# PARAMETER IS NULL\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# CREATE-TREE: %s\n", pRowDeviceInfo->deviceID);

	MEMSET(&reqParam, GRIB_INIT, sizeof(OneM2M_ReqParam));
	MEMSET(&reqParam, GRIB_INIT, sizeof(OneM2M_ResParam));

	//1 shbaek: 1.App Entity
	//shbaek: Your Device ID
	STRINIT(&reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
	STRNCPY(&reqParam.xM2M_NM, pRowDeviceInfo->deviceID, STRLEN(pRowDeviceInfo->deviceID));
	//shbaek: Create Device ID Container
	iRes = Grib_AppEntityCreate(&reqParam, &resParam);
	if(iRes == GRIB_ERROR)
	{
		if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{//shbaek: Already Exist is Not Error.
			// TODO: shbaek: What Can I Do ... ?
		}
		else
		{
			goto ERROR;
		}
	}

	//1 shbaek: 2-1.Polling Channel
	//shbaek: Create Polling Channel Container(need xM2M_Origin)
	iRes = Grib_PollingChannelCreate(&reqParam, &resParam);
	if(iRes == GRIB_ERROR)
	{
		if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{//shbaek: Already Exist is Not Error.
			// TODO: shbaek: What Can I Do ... ?
		}
		else
		{
			goto ERROR;
		}
	}

	for(i=0; i<pRowDeviceInfo->deviceFuncCount; i++)
	{
		Grib_DbRowDeviceFunc* pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];
		char* pFuncName = pRowDeviceFunc->funcName;

		//1 shbaek: 2-2.Function
		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(&reqParam.xM2M_URI, reqParam.xM2M_Origin, STRLEN(reqParam.xM2M_Origin));
		STRINIT(&reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(&reqParam.xM2M_NM, pFuncName, STRLEN(pFuncName));
		iRes = Grib_ContainerCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
			{//shbaek: Already Exist is Not Error.
				STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
				SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s", reqParam.xM2M_Origin, pFuncName);
				Grib_ContainerRetrieve(&reqParam, &resParam);
			}
			else
			{
				goto ERROR;
			}
		}

		//1 shbaek: 3-1.Execute
		//shbaek: Set URI -> in/cse/"Device ID"/"Func"
		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s", reqParam.xM2M_Origin, pFuncName);
		STRINIT(&reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(&reqParam.xM2M_NM, ONEM2M_URI_CONTENT_EXECUTE, STRLEN(ONEM2M_URI_CONTENT_EXECUTE));
		//shbaek: Create Execute Container
		iRes = Grib_ContainerCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
			{//shbaek: Already Exist is Not Error.
				STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
				SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", reqParam.xM2M_Origin, pFuncName, reqParam.xM2M_NM);
				Grib_ContainerRetrieve(&reqParam, &resParam);
			}
			else
			{
				goto ERROR;
			}
		}

		//2 shbaek: NEED EXECUTE's RESOURCE ID
		STRINIT(pRowDeviceFunc->exRsrcID, sizeof(pRowDeviceFunc->exRsrcID));
		STRNCPY(pRowDeviceFunc->exRsrcID, resParam.xM2M_RsrcID, STRLEN(resParam.xM2M_RsrcID));
		GRIB_LOGD("# %s EXECUTE RESOURCE ID: %s\n", pFuncName, pRowDeviceFunc->exRsrcID);

		//1 shbaek: 3-2.Status
		//shbaek: Set URI -> in/cse/"Device ID"/"Func"
		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s", reqParam.xM2M_Origin, pFuncName);
		STRINIT(&reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(&reqParam.xM2M_NM, ONEM2M_URI_CONTENT_STATUS, STRLEN(ONEM2M_URI_CONTENT_STATUS));
		//shbaek: Create Status Container
		iRes = Grib_ContainerCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
			{//shbaek: Already Exist is Not Error.
				STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
				SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", reqParam.xM2M_Origin, pFuncName, reqParam.xM2M_NM);
				Grib_ContainerRetrieve(&reqParam, &resParam);
			}
			else
			{
				goto ERROR;
			}
		}

		//1 shbaek: 3-3.Subscription
		STRINIT(&reqParam.xM2M_Func, sizeof(reqParam.xM2M_Func));
		STRNCPY(&reqParam.xM2M_Func, pFuncName, STRLEN(pFuncName));
		//shbaek: Create Subscription(need xM2M_Origin, xM2M_Func)
		iRes = Grib_SubsciptionCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
			{//shbaek: Already Exist is Not Error.
				// TODO: shbaek: What Can I Do ... ?
			}
			else
			{
				goto ERROR;
			}
		}

	}

	return GRIB_DONE;

ERROR:
	GRIB_LOGD("# CREATE ONE M2M TREE ERROR\n");
	return GRIB_ERROR;
}

int Grib_UpdateHubInfo(void)
{
	int  iRes = GRIB_ERROR;
	char pIpAddr[128] = {'\0', };

	Grib_ConfigInfo pConfigInfo;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	STRINIT(pIpAddr, sizeof(pIpAddr));

	MEMSET(&pConfigInfo, 0x00, sizeof(Grib_ConfigInfo));
	MEMSET(&reqParam, 0x00, sizeof(OneM2M_ReqParam));
	MEMSET(&reqParam, 0x00, sizeof(OneM2M_ResParam));

	GRIB_LOGD("# UPDATE HUB INFO: START\n");

	GRIB_LOGD("# UPDATE HUB INFO: HUB VERSION: %s\n", GRIB_HUB_VERSION);

	iRes = Grib_LoadConfig(&pConfigInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# UPDATE HUB INFO: LOAD CONFIG ERROR !!!\n");
		return iRes;
	}
	GRIB_LOGD("# UPDATE HUB INFO: HUB ID: %s\n", pConfigInfo.hubID);

	iRes = Grib_GetIPAddr(pIpAddr);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# UPDATE HUB INFO: GET HOST NAME ERROR !!!\n");
	}
	GRIB_LOGD("# UPDATE HUB INFO: HUB IP: %s\n", pIpAddr);

	if(STRLEN(pIpAddr) == 0)
	{
		STRNCPY(pIpAddr, "0.0.0.0", STRLEN("0.0.0.0"));
	}

	STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	STRNCPY(reqParam.xM2M_Origin, pConfigInfo.hubID, STRLEN(pConfigInfo.hubID));

	STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
	SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", 
		reqParam.xM2M_Origin, ONEM2M_URI_CONTENT_HUB, ONEM2M_URI_CONTENT_STATUS);

	STRINIT(&reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON));
	SNPRINTF(reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON), ONEM2M_FORMAT_CONTENT_VALUE_HUB_INFO, 
		GRIB_HUB_VERSION, pConfigInfo.hubID, pIpAddr);

	iRes = Grib_ContentInstanceCreate(&reqParam, &resParam);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("# UPDATE HUB INFO: CREATE INSTANCE ERROR !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# UPDATE HUB INFO: DONE\n");

	return GRIB_SUCCESS;

}

int Grib_UpdateDeviceInfo(Grib_DbAll *pDbAll)
{
	int  i = 0;
	int  iRes = GRIB_ERROR;
	Grib_ConfigInfo pConfigInfo;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	if(pDbAll == NULL)
	{
		return GRIB_ERROR;
	}

	MEMSET(&reqParam, 0x00, sizeof(OneM2M_ReqParam));
	MEMSET(&reqParam, 0x00, sizeof(OneM2M_ResParam));

	GRIB_LOGD("# UPDATE DEVICE INFO: START\n");

	iRes = Grib_LoadConfig(&pConfigInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# UPDATE DEVICE INFO: LOAD CONFIG ERROR !!!\n");
		return iRes;
	}
	GRIB_LOGD("# UPDATE DEVICE INFO: HUB ID: %s\n", pConfigInfo.hubID);

	STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	STRNCPY(reqParam.xM2M_Origin, pConfigInfo.hubID, STRLEN(pConfigInfo.hubID));

	STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
	SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", 
		reqParam.xM2M_Origin, ONEM2M_URI_CONTENT_DEVICE, ONEM2M_URI_CONTENT_STATUS);

	STRINIT(&reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON));

	for(i=0; i<pDbAll->deviceCount; i++)
	{
		if(i==0)
		{
			STRCAT(reqParam.xM2M_CON, pDbAll->ppRowDeviceInfo[i]->deviceID);
		}
		else
		{
			STRCAT(reqParam.xM2M_CON, ", ");
			STRCAT(reqParam.xM2M_CON, pDbAll->ppRowDeviceInfo[i]->deviceID);		
		}
	}

	iRes = Grib_ContentInstanceCreate(&reqParam, &resParam);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("# UPDATE DEVICE INFO: CREATE INSTANCE ERROR !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# UPDATE DEVICE INFO: DONE\n");

	return GRIB_SUCCESS;

}

