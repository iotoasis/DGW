/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "grib_onem2m.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
char* gSiServerIp;
int   gSiServerPort;
char* gSiInName;
char* gSiCseName;

int  gDebugOneM2M;

//3 shbaek: Do Not Used ...
OneM2M_ReqParam gReqParam;
OneM2M_ResParam gResParam;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define __ONEM2M_UTIL_FUNC__
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

int Grib_SiSetServerConfig(void)
{
	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("LOAD CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	if(gSiInName==NULL) gSiInName = STRDUP(pConfigInfo->siInName);
	if(gSiCseName==NULL) gSiCseName = STRDUP(pConfigInfo->siCseName);

	if(gSiServerIp==NULL) gSiServerIp = STRDUP(pConfigInfo->siServerIP);
	gSiServerPort = pConfigInfo->siServerPort;

	gDebugOneM2M = 0<pConfigInfo->debugLevel?TRUE:FALSE;
//	gDebugOneM2M = TRUE; //shbaek: TEST DEBUG

	Grib_HttpSetDebug(gDebugOneM2M, pConfigInfo->tombStone);
	Grib_SmdSetDebug(gDebugOneM2M);

	GRIB_LOGD("# SERVER CONFIG: %s:%d/%s/%s\n", gSiServerIp, gSiServerPort, gSiInName, gSiCseName);

	return GRIB_SUCCESS;
}

int Grib_OneM2MResParser(OneM2M_ResParam *pResParam)
{
	int i = 0;
	int iLoopMax = 128;
	int iDBG = FALSE;

	char* strToken		= NULL;
	char* str1Line		= NULL;
	char* strTemp		= NULL;
	char* strSave		= NULL;

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
			str1Line = STRTOK_R(strResponse, strToken, &strSave);
		}
		else
		{
			str1Line = STRTOK_R(NULL, strToken, &strSave);
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
			if(strValueEnd==NULL)continue;
			strValueEnd[0] = NULL;

			STRNCPY(pResParam->xM2M_RsrcID, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] LAST VALUE:[%s]\n", i, pResParam->xM2M_RsrcID);
			continue;
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: [TBD] Find Only Parent ID Key
		strKey = "\"pi\" : \"";
		strTemp = STRSTR(str1Line, strKey);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strKey);
			if(iDBG)GRIB_LOGD("[%03d] KEY:[%s] TEMP VALUE:[%s]\n", i, strKey, strValue);

			strValueEnd = STRCHR(strValue, '"');
			if(strValueEnd==NULL)continue;
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

int Grib_OneM2MSendMsg(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	const int iDBG = FALSE;

	int iRes = GRIB_ERROR;
	Grib_HttpMsgInfo* pHttpMsg = NULL;

	//shbaek: Check Server Info
	if( STRLEN(gSiServerIp)==0 || (gSiServerPort==0) )
	{
		Grib_SiSetServerConfig();
	}

	pHttpMsg = (Grib_HttpMsgInfo*)MALLOC(sizeof(Grib_HttpMsgInfo));
	MEMSET(pHttpMsg, 0x00, sizeof(Grib_HttpMsgInfo));

	pHttpMsg->serverIP = gSiServerIp;
	pHttpMsg->serverPort = gSiServerPort;

	pHttpMsg->LABEL	 = pReqParam->xM2M_AeName;
	pHttpMsg->recvBuff = pResParam->http_RecvData;

	if(pReqParam->xM2M_ResourceType == ONEM2M_RESOURCE_TYPE_SEMANTIC_DESCRIPTOR)
	{//shbaek: Too Large Data ...
		pHttpMsg->sendBuff = pReqParam->http_SendDataEx;
		if(gDebugOneM2M)GRIB_LOGD("# %s-xM2M: LARGE DATA: %d\n", pHttpMsg->LABEL, STRLEN(pHttpMsg->sendBuff));
	}
	else
	{
		pHttpMsg->sendBuff = pReqParam->http_SendData;
	}

	do{
		iRes = Grib_HttpSendMsg(pHttpMsg);
		if(iRes <= 0)
		{
			STRINIT(pResParam->http_ResMsg, sizeof(pResParam->http_ResMsg));
			STRNCPY(pResParam->http_ResMsg, pHttpMsg->statusMsg, STRLEN(pHttpMsg->statusMsg));

			pResParam->http_ResNum = pHttpMsg->statusCode;

			goto ERROR;
		}
	}while(FALSE);

	iRes = Grib_HttpResParser(pHttpMsg);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: MSG PARSE ERROR !!!\n", pHttpMsg->LABEL);
		pResParam->http_ResNum = HTTP_STATUS_CODE_UNKNOWN;
		goto ERROR;
	}

	pResParam->http_ResNum = pHttpMsg->statusCode;

	STRINIT(pResParam->http_ResMsg, STRLEN(pResParam->http_ResMsg));
	STRNCPY(pResParam->http_ResMsg, pHttpMsg->statusMsg, STRLEN(pHttpMsg->statusMsg));

	if( (HTTP_STATUS_CODE_SUCCESS_BASE<=pHttpMsg->statusCode) && (pHttpMsg->statusCode<=HTTP_STATUS_CODE_SUCCESS_MAX) )
	{//shbaek: SUCCESS CASE
		FREE(pHttpMsg);
		return GRIB_DONE;
	}

ERROR:
	if(gDebugOneM2M)
	{
		//GRIB_LOGD("# %s-xM2M: %s [%d]\n", pHttpMsg->LABEL, pHttpMsg->statusMsg, pHttpMsg->statusCode);
		GRIB_LOGD("# %s-xM2M: RECV BUFF[%d]:\n%s\n", pHttpMsg->LABEL, STRLEN(pHttpMsg->recvBuff), pHttpMsg->recvBuff);
	}
	FREE(pHttpMsg);

	return GRIB_ERROR;
}

int Grib_GetAttrExpireTime(char* attrBuff, TimeInfo* pTime)
{
	const char* EXPIRE_TIME_STR_FORMAT = GRIB_STR_TIME_FORMAT;

	if(attrBuff == NULL)
	{
		GRIB_LOGD("# PARAM IS NULL ERROR !!!\n");
		return GRIB_ERROR;
	}

	STRINIT(attrBuff, ONEM2M_EXPIRE_TIME_STR_SIZE);

	if(pTime == NULL)
	{
		STRNCPY(attrBuff, ONEM2M_FIX_ATTR_ET, STRLEN(ONEM2M_FIX_ATTR_ET));
	}
	else
	{
		time_t curTimeSec = (time_t)0;
		time_t addTimeSec = (time_t)0;
		time_t exTimeSec  = (time_t)0;
		TimeInfo exTimeInfo;

		curTimeSec = time(NULL);

		addTimeSec += pTime->tm_year * GRIB_SEC_YEAR;
		addTimeSec += pTime->tm_mon  * GRIB_SEC_MON;
		addTimeSec += pTime->tm_mday * GRIB_SEC_DAY;
		addTimeSec += pTime->tm_hour * GRIB_SEC_HOUR;
		addTimeSec += pTime->tm_min  * GRIB_SEC_MIN;
		addTimeSec += pTime->tm_sec;

		exTimeSec = curTimeSec + addTimeSec;

		MEMSET(&exTimeInfo, 0x00, sizeof(exTimeInfo));
		localtime_r(&exTimeSec, &exTimeInfo);

		SNPRINTF(attrBuff, ONEM2M_EXPIRE_TIME_STR_SIZE, EXPIRE_TIME_STR_FORMAT, 
					exTimeInfo.tm_year+1900, exTimeInfo.tm_mon+1, exTimeInfo.tm_mday,
					exTimeInfo.tm_hour, exTimeInfo.tm_min, exTimeInfo.tm_sec);
	}

	if(gDebugOneM2M)
	{
		Grib_ShowCurrDateTime();
		GRIB_LOGD("# EXPIRE  TIME: %c[1;33m%s%c[0m\n", 27, attrBuff, 27);
	}

	return GRIB_DONE;
}

int Grib_isAvailableExpireTime(char* xM2M_ExpireTime)
{
	int i = 0;
	time_t sysTimer;

	TimeInfo *sysTime;
	
	int iSeek = 0;
	char timeBuff[5] = {'\0', };

	//shbaek: Ex)20991130T163430
	if( (xM2M_ExpireTime==NULL) || (STRLEN(xM2M_ExpireTime)<15) )
	{
		GRIB_LOGD("# EXPIRE TIME INVALID ERROR !!!\n");
		return FALSE;
	}

	sysTimer = time(NULL);
	sysTime  = localtime(&sysTimer);

	if(gDebugOneM2M)GRIB_LOGD("# EXPIRE TIME: %s\n", xM2M_ExpireTime);

	//shbaek: YYYY
	STRINIT(timeBuff, sizeof(timeBuff));
	for(i=0; i<4; i++)timeBuff[i] = xM2M_ExpireTime[i];
	if((sysTime->tm_year+1900) < ATOI(timeBuff))return TRUE;
	else if(ATOI(timeBuff) < (sysTime->tm_year+1900))return FALSE;
	else iSeek = i;

	//shbaek: MM
	STRINIT(timeBuff, sizeof(timeBuff));
	for(i=0; i<2; i++)timeBuff[i] = xM2M_ExpireTime[iSeek+i];
	if((sysTime->tm_mon+1) < ATOI(timeBuff))return TRUE;
	else if(ATOI(timeBuff) < (sysTime->tm_mon+1))return FALSE;
	else iSeek += i;

	//shbaek: DD
	STRINIT(timeBuff, sizeof(timeBuff));
	for(i=0; i<2; i++)timeBuff[i] = xM2M_ExpireTime[iSeek+i];
	if((sysTime->tm_mday) < ATOI(timeBuff))return TRUE;
	else if(ATOI(timeBuff) < (sysTime->tm_mday))return FALSE;
	else iSeek += i;

	//shbaek: Skip 'T'
	iSeek++;

	//shbaek: HH
	STRINIT(timeBuff, sizeof(timeBuff));
	for(i=0; i<2; i++)timeBuff[i] = xM2M_ExpireTime[iSeek+i];
	if((sysTime->tm_hour) < ATOI(timeBuff))return TRUE;
	else if(ATOI(timeBuff) < (sysTime->tm_hour))return FALSE;
	else iSeek += i;

	//shbaek: MM
	STRINIT(timeBuff, sizeof(timeBuff));
	for(i=0; i<2; i++)timeBuff[i] = xM2M_ExpireTime[iSeek+i];
	if((sysTime->tm_min) < ATOI(timeBuff))return TRUE;
	else if(ATOI(timeBuff) < (sysTime->tm_min))return FALSE;
	else iSeek += i;
	iSeek += i;

	//shbaek: SS
	STRINIT(timeBuff, sizeof(timeBuff));
	for(i=0; i<2; i++)timeBuff[i] = xM2M_ExpireTime[iSeek+i];
	if((sysTime->tm_sec) < ATOI(timeBuff))return TRUE;
	else if(ATOI(timeBuff) < (sysTime->tm_sec))return FALSE;
	else iSeek += i;
	iSeek += i;

	return TRUE;
}

int Grib_CmdRequestParser(OneM2M_ResParam* pResParam)
{
	const char* FUNC = "CMD-PARSER";
	int iRes = GRIB_DONE;
	int iDBG = gDebugOneM2M;

	char* srcBuff = NULL;
	char* oriBuff = NULL;
	char* decBuff = NULL;

	if(pResParam == NULL)
	{
		Grib_ErrLog(FUNC, "PARAM IS NULL ERROR !!!");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# %s: CON TYPE : %s\n", FUNC, pResParam->xM2M_ContentInfo);
		GRIB_LOGD("# %s: CON VALUE: \n%s\n", FUNC, pResParam->xM2M_Content);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	decBuff = (char*)MALLOC(sizeof(pResParam->xM2M_Content));
	MEMSET(decBuff, 0x00, sizeof(pResParam->xM2M_Content));

	if(STRSTR(pResParam->xM2M_ContentInfo, ":0") != NULL)
	{//shbaek: Plain
		STRNCPY(decBuff, pResParam->xM2M_Content, STRLEN(pResParam->xM2M_Content));
	}
	else if(STRSTR(pResParam->xM2M_ContentInfo, ":1") != NULL)
	{//shbaek: Base64 Encoded
		iRes = Grib_Base64Decode(pResParam->xM2M_Content, decBuff, GRIB_NOT_USED);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# %s: BASE64 DECODING ERROR !!!\n", FUNC);
			return GRIB_ERROR;
		}
	}
	else
	{//shbaek: Not Support
		GRIB_LOGD("# %s: NOT SUPPORTED TYPE [%d]: \n%s\n", FUNC, STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		return GRIB_ERROR;
	}

	if(!STRNCASECMP(pResParam->xM2M_ContentInfo, HTTP_CONTENT_TYPE_TEXT, STRLEN(HTTP_CONTENT_TYPE_TEXT)))
	{//shbaek: Do Not Parsing
		STRINIT(pResParam->cmdReq_ExecID, sizeof(pResParam->cmdReq_ExecID));
		STRNCPY(pResParam->cmdReq_ExecID, pResParam->xM2M_RsrcID, STRLEN(pResParam->xM2M_RsrcID));
		if(iDBG)GRIB_LOGD("# %s: EXEC_ID: %s\n", FUNC, pResParam->cmdReq_ExecID);

		STRINIT(pResParam->xM2M_Content, sizeof(pResParam->xM2M_Content));
		STRNCPY(pResParam->xM2M_Content, decBuff, STRLEN(decBuff));
		if(iDBG)GRIB_LOGD("# %s: CMD DATA: %s\n", FUNC, pResParam->xM2M_Content);
	}

	if(!STRNCASECMP(pResParam->xM2M_ContentInfo, HTTP_CONTENT_TYPE_JSON, STRLEN(HTTP_CONTENT_TYPE_JSON)))
	{//shbaek: Need Parsing Json.
		char* strKey = NULL;
		char* strTemp = NULL;
		char* strValue = NULL;

		oriBuff = srcBuff = Grib_TrimAll(decBuff);
		//if(iDBG)GRIB_LOGD("# %s: BASE64 DEC CON VALUE: %s\n", FUNC, decBuff);

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: [TBD] Find Execute ID
		strKey = "\"exec_id\":\"";
		strTemp = STRSTR(srcBuff, strKey);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strKey);
			//if(iDBG)GRIB_LOGD("# %s: KEY:[%s] TEMP VALUE:[%s]\n", FUNC, strKey, strValue);

			strValueEnd = STRCHR(strValue, '"');
			if(strValueEnd != NULL)
			{
				strValueEnd[0] = NULL;

				STRINIT(pResParam->cmdReq_ExecID, sizeof(pResParam->cmdReq_ExecID));
				STRNCPY(pResParam->cmdReq_ExecID, strValue, STRLEN(strValue));

				srcBuff = ++strValueEnd; //shbaek: Move String Point
				if(iDBG)GRIB_LOGD("# %s: EXEC_ID: %s\n", FUNC, pResParam->cmdReq_ExecID);
			}
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: [TBD] Find Control Command Data
		strKey = "\"data\":\"";
		strTemp = STRSTR(srcBuff, strKey);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strKey);
			//if(iDBG)GRIB_LOGD("# %s: KEY:[%s] TEMP VALUE:[%s]\n", FUNC, strKey, strValue);

			strValueEnd = STRCHR(strValue, '"');
			if(strValueEnd != NULL)
			{
				strValueEnd[0] = NULL;

				STRINIT(pResParam->xM2M_Content, sizeof(pResParam->xM2M_Content));
				STRNCPY(pResParam->xM2M_Content, strValue, STRLEN(strValue));
				
				if(iDBG)GRIB_LOGD("# %s: CMD DATA: %s\n", FUNC, pResParam->xM2M_Content);
			}
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

		
	}

	FREE(oriBuff);
	FREE(decBuff);

	return iRes;
}

//int Grib_CmdResponseCreate(char* cmdID, char* cmdData, char* cmdBuff)
int Grib_CmdResponseCreate(OneM2M_ReqParam* pReqParam, OneM2M_ResParam* pResParam, char* cmdResData)
{
	const char* FUNC = "CMD-PARSER";
	int iRes = GRIB_DONE;
	int iDBG = gDebugOneM2M;

	const char* CMD_RES_FORMAT = 	"{" \
									" \"exec_id\":\"%s\"," \
									" \"exec_result\":\"%s\" " \
									"}";

	char CMD_RES_BUFF[GRIB_MAX_SIZE_DLONG] = {'\0', };

	if( (pReqParam==NULL) || (pResParam==NULL) || (cmdResData==NULL) )
	{
		Grib_ErrLog(FUNC, "PARAM IS NULL ERROR !!!");
		return GRIB_ERROR;
	}

	SNPRINTF(CMD_RES_BUFF, sizeof(CMD_RES_BUFF), CMD_RES_FORMAT, pResParam->cmdReq_ExecID, cmdResData);
	if(iDBG)GRIB_LOGD("# %s-xCMD: SRC: %s\n", pReqParam->xM2M_AeName, CMD_RES_BUFF);

	STRINIT(pReqParam->xM2M_CNF, sizeof(pReqParam->xM2M_CNF));
	SPRINTF(pReqParam->xM2M_CNF, "%s:%d", HTTP_CONTENT_TYPE_JSON, HTTP_ENC_TYPE_BASE64);

	STRINIT(pReqParam->xM2M_CON, sizeof(pReqParam->xM2M_CON));
	Grib_Base64Encode(CMD_RES_BUFF, pReqParam->xM2M_CON, GRIB_NOT_USED);
	if(iDBG)GRIB_LOGD("# %s-xCMD: ENC: %s\n", pReqParam->xM2M_AeName, pReqParam->xM2M_CON);

	return iRes;
}


#define __ONEM2M_DEVICE_ID_FUNC__
//2 shbaek: NEED: xM2M_NM
int Grib_AppEntityCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_BODY] = {'\0',};

	char  AUTO_LABEL[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  xM2M_AttrAPN[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char  xM2M_AttrAPI[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char* xM2M_AttrRR = ONEM2M_FIX_ATTR_RR;

	char  xM2M_AttrET[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char* xM2M_AttrRN = NULL;

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

/*
	STRINIT(pReqParam->xM2M_Origin, sizeof(pReqParam->xM2M_Origin));
	STRNCPY(pReqParam->xM2M_Origin, pReqParam->xM2M_NM, STRLEN(pReqParam->xM2M_NM));
	pReqParam->xM2M_Origin[STRLEN(pReqParam->xM2M_NM)+1] = NULL;
*/

	STRINIT(pReqParam->xM2M_AeName, sizeof(pReqParam->xM2M_AeName));
	STRNCPY(pReqParam->xM2M_AeName, pReqParam->xM2M_NM, STRLEN(pReqParam->xM2M_NM));
	pReqParam->xM2M_AeName[STRLEN(pReqParam->xM2M_NM)+1] = NULL;

	STRINIT(AUTO_LABEL, sizeof(AUTO_LABEL));
	if(STRLEN(pReqParam->xM2M_AttrLBL) <= 1)
	{
		SNPRINTF(AUTO_LABEL, sizeof(AUTO_LABEL), "%s_AppEntityLabel", pReqParam->xM2M_AeName);
	}
	else
	{
		STRNCPY(AUTO_LABEL, pReqParam->xM2M_AttrLBL, sizeof(AUTO_LABEL));
	}

	SNPRINTF(xM2M_AttrAPN, sizeof(xM2M_AttrAPN), "%s_AppName",	  pReqParam->xM2M_AeName);
	SNPRINTF(xM2M_AttrAPI, sizeof(xM2M_AttrAPI), "%s_AppID",     pReqParam->xM2M_AeName);

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqAppEntityCreate", pReqParam->xM2M_AeName);

	Grib_GetAttrExpireTime(xM2M_AttrET, GRIB_NOT_USED);
	xM2M_AttrRN = (char *)pReqParam->xM2M_NM; //shbaek: "X-M2M-NM" Change to "rn" on v2.0

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_APP_ENTITY_CREATE,
				AUTO_LABEL, xM2M_AttrAPN, xM2M_AttrAPI, gSiServerIp, gSiServerPort, xM2M_AttrRR,
				xM2M_AttrRN, xM2M_AttrET);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_APP_ENTITY_CREATE,
				gSiInName, gSiCseName, gSiServerIp, gSiServerPort, 
				ONEM2M_RESOURCE_TYPE_APP_ENTITY, STRLEN(httpBody),
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				//3 shbaek: Origin Value Must be Use xM2M_NM(AE Name) -> Do Not Use Hub Name
				pReqParam->xM2M_NM, pReqParam->xM2M_ReqID);//shbaek: xM2M_Origin -> xM2M_NM

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# APP ENTITY CREATE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{
			GRIB_LOGD("# %s-xM2M: %c[1;33mAPP ENTITY ALREADY EXIST ...%c[0m\n", pReqParam->xM2M_AeName, 27, 27);
		}
		else
		{
			GRIB_LOGD("# %s-xM2M: %c[1;31mCREATE AE FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
				27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		}
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# APP ENTITY CREATE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
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

//2 shbaek: NEED: xM2M_Origin
int Grib_AppEntityRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqAppEntityRetrieve", pReqParam->xM2M_AeName);

	STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
	SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s", pReqParam->xM2M_AeName);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_APP_ENTITY_RETRIEVE,
				gSiInName, gSiCseName, pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# APP ENTITY RETRIEVE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: %c[1;31mRETRIEVE AE FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
			27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# APP ENTITY RETRIEVE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
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

//2 shbaek: NEED: xM2M_Origin
int Grib_AppEntityDelete(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqAppEntityDelete", pReqParam->xM2M_AeName);

	STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
	SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s", pReqParam->xM2M_AeName);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_APP_ENTITY_DELETE,
				gSiInName, gSiCseName, pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# APP ENTITY DELETE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# APP ENTITY DELETE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# APP ENTITY DELETE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	return GRIB_SUCCESS;
}

#define __ONEM2M_CONTAINER_FUNC__

//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_NM
int Grib_ContainerCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	char  httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_BODY] = {'\0',};

	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	unsigned long long int xM2M_AttrMNI = 0; //shbaek: maxNrOfInstances
	unsigned long long int xM2M_AttrMBS = 0; //shbaek: maxByteSize
	unsigned long long int xM2M_AttrMIA = 0; //shbaek: maxInstanceAge

	char  AUTO_LABEL[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  xM2M_AttrET[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char* xM2M_AttrRN = NULL;

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	xM2M_AttrMNI = ONEM2M_FIX_ATTR_MNI;
	xM2M_AttrMBS = ONEM2M_FIX_ATTR_MBS;
	xM2M_AttrMIA = ONEM2M_FIX_ATTR_MIA;

	if(STRLEN(pReqParam->xM2M_AttrLBL) <= 1)
	{
		STRINIT(AUTO_LABEL, sizeof(AUTO_LABEL));
		SNPRINTF(AUTO_LABEL, sizeof(AUTO_LABEL), "%s_ContainerLabel", pReqParam->xM2M_NM);
	}

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContainerCreate", pReqParam->xM2M_AeName);

	Grib_GetAttrExpireTime(xM2M_AttrET, GRIB_NOT_USED);
	xM2M_AttrRN = (char *)pReqParam->xM2M_NM; //shbaek: "X-M2M-NM" Change to "rn" on v2.0

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_CONTAINER_CREATE,
				AUTO_LABEL, xM2M_AttrMNI, xM2M_AttrMBS, xM2M_AttrMIA,
				xM2M_AttrRN, xM2M_AttrET);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_CREATE,
				gSiInName, gSiCseName, pReqParam->xM2M_URI, gSiServerIp, gSiServerPort,
				ONEM2M_RESOURCE_TYPE_CONTAINER, STRLEN(httpBody), 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTAINER CREATE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{

			GRIB_LOGD("# %s-xM2M: %c[1;33m%s CNT ALREADY EXIST ...%c[0m\n", pReqParam->xM2M_AeName, 
				27, pReqParam->xM2M_NM, 27);
		}
		else
		{
			GRIB_LOGD("# %s-xM2M: %c[1;31mCREATE %s CNT FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
				27, pReqParam->xM2M_NM, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		}

		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTAINER CREATE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);
		GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	}

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContainerRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContainerRetrieve", pReqParam->xM2M_AeName);

#ifdef FEATURE_CAS
	if(STRLEN(pReqParam->authKey)==0)STRNCPY(pReqParam->authKey, GRIB_DEFAULT_AUTH_KEY, STRLEN(GRIB_DEFAULT_AUTH_KEY));
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_RETRIEVE, gSiInName, gSiCseName,
				pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTAINER RETRIEVE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: %c[1;31mRETRIEVE CNT FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
			27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTAINER RETRIEVE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# RESPONSE PARSING ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);
		GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
	}

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContainerDelete(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;

	char httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContainerDelete", pReqParam->xM2M_AeName);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_DELETE, gSiInName, gSiCseName,
				pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTAINER DELETE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# CONTAINER DELETE ERROR\n");
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTAINER DELETE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	return GRIB_SUCCESS;
}

#define __ONEM2M_POLLING_FUNC__
//2 shbaek: NEED: xM2M_Origin
int Grib_PollingChannelCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	char  httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_BODY] = {'\0',};

	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  AUTO_LABEL[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  xM2M_AttrET[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char* xM2M_AttrRN = NULL;

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqPollingChannelCreate", pReqParam->xM2M_AeName);

	STRINIT(pReqParam->xM2M_NM, sizeof(pReqParam->xM2M_NM));
	STRNCPY(pReqParam->xM2M_NM, ONEM2M_URI_CONTENT_POLLING_CHANNEL, STRLEN(ONEM2M_URI_CONTENT_POLLING_CHANNEL));

	if(STRLEN(pReqParam->xM2M_AttrLBL) <= 1)
	{
		STRINIT(AUTO_LABEL, sizeof(AUTO_LABEL));
		SNPRINTF(AUTO_LABEL, sizeof(AUTO_LABEL), "%s_PollingChannelLabel", pReqParam->xM2M_AeName);
	}

	Grib_GetAttrExpireTime(xM2M_AttrET, GRIB_NOT_USED);
	xM2M_AttrRN = (char *)pReqParam->xM2M_NM; //shbaek: "X-M2M-NM" Change to "rn" on v2.0

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_POLLING_CHANNEL_CREATE, AUTO_LABEL,
			xM2M_AttrRN, xM2M_AttrET);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTAINER_CREATE, gSiInName, gSiCseName,
				pReqParam->xM2M_AeName, gSiServerIp, gSiServerPort, ONEM2M_RESOURCE_TYPE_POLLING_CHANNEL, STRLEN(httpBody),
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# POLLING CHANNEL CREATE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{
			GRIB_LOGD("# %s-xM2M: %c[1;33mPOLLING CHANNEL ALREADY EXIST ...%c[0m\n", pReqParam->xM2M_AeName, 27, 27);
		}
		else
		{
			GRIB_LOGD("# %s-xM2M: %c[1;31mCREATE PCH FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
				27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		}

		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# POLLING CHANNEL CREATE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
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

//2 shbaek: NEED: xM2M_Origin xM2M_Func
int Grib_SubsciptionCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	char  httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_BODY] = {'\0',};

	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  AUTO_LABEL[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  xM2M_AttrENC[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char  xM2M_AttrNU[GRIB_MAX_SIZE_URI] = {'\0',};

	char  xM2M_AttrET[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char* xM2M_AttrRN = NULL;

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	if(iDBG)
	{
		GRIB_LOGD("# SUBSCRIPTION CREATE ...\n");
	}

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqSubsciptionCreate", pReqParam->xM2M_AeName);
	if(iDBG)GRIB_LOGD("# %s: REQ ID: %s\n", pReqParam->xM2M_AeName, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
	SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s/%s/%s", 
		pReqParam->xM2M_AeName, pReqParam->xM2M_Func, ONEM2M_URI_CONTENT_EXECUTE);
	if(iDBG)GRIB_LOGD("# SUBSCRIPTION : xM2M_URI: %s\n", pReqParam->xM2M_URI);

	STRINIT(pReqParam->xM2M_NM, sizeof(pReqParam->xM2M_NM));
	STRNCPY(pReqParam->xM2M_NM, ONEM2M_URI_CONTENT_SUBSCRIPTION, STRLEN(ONEM2M_URI_CONTENT_SUBSCRIPTION));
	if(iDBG)GRIB_LOGD("# SUBSCRIPTION : xM2M_NM: %s\n", pReqParam->xM2M_NM);

	if(STRLEN(pReqParam->xM2M_AttrLBL) <= 1)
	{
		STRINIT(AUTO_LABEL, sizeof(AUTO_LABEL));
		SNPRINTF(AUTO_LABEL, sizeof(AUTO_LABEL), "%s_SubscriptionLabel", pReqParam->xM2M_Func);
		if(iDBG)GRIB_LOGD("# SUBSCRIPTION : AUTO_LABEL: %s\n", AUTO_LABEL);
	}
	
	STRNCPY(xM2M_AttrENC, ONEM2M_FIX_ATTR_ENC, STRLEN(ONEM2M_FIX_ATTR_ENC));
	if(iDBG)GRIB_LOGD("# SUBSCRIPTION : ENC: %s\n", xM2M_AttrENC);

	SNPRINTF(xM2M_AttrNU, sizeof(xM2M_AttrNU), ONEM2M_FIX_ATTR_NU_FORMAT, 
		gSiServerIp, gSiServerPort, gSiInName, gSiCseName, pReqParam->xM2M_AeName);
	if(iDBG)GRIB_LOGD("# SUBSCRIPTION : NU: %s\n", xM2M_AttrNU);

	Grib_GetAttrExpireTime(xM2M_AttrET, GRIB_NOT_USED);
	xM2M_AttrRN = (char *)pReqParam->xM2M_NM; //shbaek: "X-M2M-NM" Change to "rn" on v2.0
	if(iDBG)GRIB_LOGD("# SUBSCRIPTION : RN: %s\n", xM2M_AttrRN);

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_SUBSCRIPTION_CREATE, 
		AUTO_LABEL, xM2M_AttrENC, xM2M_AttrNU, xM2M_AttrRN, xM2M_AttrET);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_SUBSCRIPTION_CREATE, gSiInName, gSiCseName,
				pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, ONEM2M_RESOURCE_TYPE_SUBSCRIPTION, STRLEN(httpBody), 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# SUBSCRIPTION CREATE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{
			GRIB_LOGD("# %s-xM2M: %c[1;33mSUBSCRIPTION ALREADY EXIST ...%c[0m\n", pReqParam->xM2M_AeName, 27, 27);
		}
		else
		{
			GRIB_LOGD("# %s-xM2M: %c[1;31mCREATE SUB FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
				27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		}
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# SUBSCRIPTION CREATE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
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

#define __ONEM2M_CONTENT_INSTANCE_FUNC__

//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_CNF[If NULL, Set Default "text/plain:0"], xM2M_CON
int Grib_ContentInstanceCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};
	char  httpBody[HTTP_MAX_SIZE_BODY] = {'\0',};

	char  AUTO_LABEL[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  xM2M_AttrET[GRIB_MAX_SIZE_BRIEF] = {'\0',};

	TimeInfo pTime;

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	if(STRLEN(pReqParam->xM2M_ReqID) <= 1)
	{
		STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
		SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContentInstanceCreate", pReqParam->xM2M_AeName);
	}

	if(STRLEN(pReqParam->xM2M_CNF) <= 1)
	{//shbaek: Default Type
		STRINIT(pReqParam->xM2M_CNF, sizeof(pReqParam->xM2M_CNF));
		SNPRINTF(pReqParam->xM2M_CNF, sizeof(pReqParam->xM2M_CNF), "%s:%d", HTTP_CONTENT_TYPE_TEXT, HTTP_ENC_TYPE_NONE);
	}

	STRINIT(AUTO_LABEL, sizeof(AUTO_LABEL));
	if(STRLEN(pReqParam->xM2M_AttrLBL) <= 1)
	{
		SNPRINTF(AUTO_LABEL, sizeof(AUTO_LABEL), "%s_ContentInstanceLabel", pReqParam->xM2M_AeName);
	}
	else
	{
		STRNCPY(AUTO_LABEL, pReqParam->xM2M_AttrLBL, sizeof(AUTO_LABEL));
	}

	//shbaek: Add for Instance Expire Time(=1 Day) by Herit.
	MEMSET(&pTime, 0x00, sizeof(TimeInfo));
	pTime.tm_mday = 1;

	Grib_GetAttrExpireTime(xM2M_AttrET, &pTime);

	SNPRINTF(httpBody, sizeof(httpBody), ONEM2M_BODY_FORMAT_CONTENT_INSTANCE_CREATE,
				AUTO_LABEL, xM2M_AttrET, pReqParam->xM2M_CNF, pReqParam->xM2M_CON);

#ifdef FEATURE_CAS
	if(STRLEN(pReqParam->authKey)==0)STRNCPY(pReqParam->authKey, GRIB_DEFAULT_AUTH_KEY, STRLEN(GRIB_DEFAULT_AUTH_KEY));
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_CREATE, gSiInName, gSiCseName,
				pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, ONEM2M_RESOURCE_TYPE_CONTENT_INSTANCE, STRLEN(httpBody), 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s%s", httpHead, httpBody);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTENT INSTANCE CREATE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: %c[1;31mCREATE CIN FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
			27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTENT INSTANCE CREATE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: RESPONSE PARSING ERROR\n", pReqParam->xM2M_AeName);
		return GRIB_ERROR;
	}

	if(FALSE)
	{
		GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
		GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);
	}

	return GRIB_SUCCESS;
}

//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContentInstanceRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugOneM2M;
	char httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqContentInstanceRetrieve", pReqParam->xM2M_AeName);

#ifdef FEATURE_CAS
	if(STRLEN(pReqParam->authKey)==0)STRNCPY(pReqParam->authKey, GRIB_DEFAULT_AUTH_KEY, STRLEN(GRIB_DEFAULT_AUTH_KEY));
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_RETRIEVE, gSiInName, gSiCseName,
				pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTENT INST RETRIEVE SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: %c[1;31mRETRIEVE CIN FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
			27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		return GRIB_ERROR;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# CONTENT INST RETRIEVE RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
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

#define __ONEM2M_LONG_POLLING_FUNC__

int Grib_LongPollingResParser(OneM2M_ResParam *pResParam)
{
	int i = 0;
	int iLoopMax = 128;
	int iDBG = FALSE;

	char* strToken		= NULL;
	char* str1Line		= NULL;
	char* strTemp		= NULL;
	char* strSave		= NULL;

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

	if(iDBG)GRIB_LOGD(GRIB_1LINE_SHARP);
	do{
		//shbaek: Cut 1 Line
		if(i==0)
		{
//			str1Line = STRTOK(strResponse, strToken);
			str1Line = STRTOK_R(strResponse, strToken, &strSave);
		}
		else
		{
//			str1Line = STRTOK(NULL, strToken);
			str1Line = STRTOK_R(NULL, strToken, &strSave);
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
			if(strValueEnd==NULL) continue;
			strValueEnd[0] = NULL;
			
			STRINIT(pResParam->xM2M_RsrcID, sizeof(pResParam->xM2M_RsrcID));
			STRNCPY(pResParam->xM2M_RsrcID, strValue, STRLEN(strValue));
			
			if(iDBG)GRIB_LOGD("[%03d] RID:[%s]\n", i, pResParam->xM2M_RsrcID);
			continue;//1 shbaek: Next Line
		}

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
			if(strValueEnd==NULL) continue;
			strValueEnd[0] = NULL;

			STRINIT(pResParam->xM2M_PrntID, sizeof(pResParam->xM2M_PrntID));
			STRNCPY(pResParam->xM2M_PrntID, strValue, STRLEN(strValue));

			if(iDBG)GRIB_LOGD("[%03d] PID:[%s]\n", i, pResParam->xM2M_PrntID);
			continue;//1 shbaek: Next Line
		}


		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: Find Create Time Tag
		strTagStart = "<ct>";
		strTagEnd	= "</ct>";
		strTemp = STRSTR(str1Line, strTagStart);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strTagStart);
			if(iDBG)GRIB_LOGD("[%03d] TAG:[%s] TEMP VALUE:[%s]\n", i, strTagStart, strValue);

			strValueEnd = STRSTR(str1Line, strTagEnd);
			if(strValueEnd==NULL) continue;
			strValueEnd[0] = NULL;

			STRINIT(pResParam->xM2M_CreateTime, sizeof(pResParam->xM2M_CreateTime));
			STRNCPY(pResParam->xM2M_CreateTime, strValue, STRLEN(strValue));

			if(iDBG)GRIB_LOGD("[%03d] CT:[%s]\n", i, pResParam->xM2M_CreateTime);
			continue;//1 shbaek: Next Line
		}

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: Find Expire Time Tag
		strTagStart = "<et>";
		strTagEnd	= "</et>";
		strTemp = STRSTR(str1Line, strTagStart);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strTagStart);
			if(iDBG)GRIB_LOGD("[%03d] TAG:[%s] TEMP VALUE:[%s]\n", i, strTagStart, strValue);

			strValueEnd = STRSTR(str1Line, strTagEnd);
			if(strValueEnd==NULL) continue;
			strValueEnd[0] = NULL;

			STRINIT(pResParam->xM2M_ExpireTime, sizeof(pResParam->xM2M_ExpireTime));
			STRNCPY(pResParam->xM2M_ExpireTime, strValue, STRLEN(strValue));

			if(iDBG)GRIB_LOGD("[%03d] ET:[%s]\n", i, pResParam->xM2M_ExpireTime);
			continue;//1 shbaek: Next Line
		}

		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		//shbaek: Find Content Info Tag
		strTagStart = "<cnf>";
		strTagEnd	= "</cnf>";
		strTemp = STRSTR(str1Line, strTagStart);
		if(strTemp != NULL)
		{
			char *strValueEnd = NULL;

			//shbaek: Copy Value
			strValue = strTemp+STRLEN(strTagStart);
			if(iDBG)GRIB_LOGD("[%03d] TAG:[%s] TEMP VALUE:[%s]\n", i, strTagStart, strValue);

			strValueEnd = STRSTR(str1Line, strTagEnd);
			if(strValueEnd==NULL) continue;
			strValueEnd[0] = NULL;

			STRINIT(pResParam->xM2M_ContentInfo, sizeof(pResParam->xM2M_ContentInfo));
			STRNCPY(pResParam->xM2M_ContentInfo, strValue, STRLEN(strValue));

			if(iDBG)GRIB_LOGD("[%03d] CNF:[%s]\n", i, pResParam->xM2M_ContentInfo);
			continue;//1 shbaek: Next Line
		}

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
			if(strValueEnd==NULL) continue;
			strValueEnd[0] = NULL;

			STRINIT(pResParam->xM2M_Content, sizeof(pResParam->xM2M_Content));
			STRNCPY(pResParam->xM2M_Content, strValue, STRLEN(strValue));

			if(iDBG)GRIB_LOGD("[%03d] CON:[%s]\n", i, pResParam->xM2M_Content);
			break;//2 shbaek: Search More?
		}
		//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

	}while(i < iLoopMax);
	if(iDBG)GRIB_LOGD(GRIB_1LINE_SHARP);

FINAL:
	if(strResponse!=NULL)FREE(strResponse);

	return GRIB_DONE;
}

//2 shbaek: NEED: xM2M_Origin, [xM2M_URI: If NULL, Auto Set]
int Grib_LongPolling(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;
	char* httpHead = NULL;
	int   sizeHead = 0;

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqLongPolling", pReqParam->xM2M_AeName);

	if(STRLEN(pReqParam->xM2M_URI) < STRLEN(pReqParam->xM2M_AeName))
	{
		STRINIT(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI));
		SNPRINTF(pReqParam->xM2M_URI, sizeof(pReqParam->xM2M_URI), "%s/%s/%s", 
			pReqParam->xM2M_AeName, ONEM2M_URI_CONTENT_POLLING_CHANNEL, ONEM2M_URI_CONTENT_PCU);

		if(iDBG)GRIB_LOGD("# LONG POLLING URI: %s\n", pReqParam->xM2M_URI);
	}

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);

	if(iDBG)
	{
		GRIB_LOGD("# LONG POLLING BASE64 SRC: %s\n", pAuthBase64Src);
		GRIB_LOGD("# LONG POLLING BASE64 ENC: %s\n", pAuthBase64Enc);
	}
#endif
	sizeHead = 	STRLEN(ONEM2M_HEAD_FORMAT_LONG_POLLING)+STRLEN(gSiInName)+STRLEN(gSiCseName)+
				STRLEN(pReqParam->xM2M_URI)+STRLEN(gSiServerIp)+4+
#ifdef FEATURE_CAS
				STRLEN(pAuthBase64Enc)+
#endif
				STRLEN(pReqParam->xM2M_Origin)+STRLEN(pReqParam->xM2M_ReqID);
	if(iDBG)GRIB_LOGD("# %s-xM2M: LONG POLLING HEAD SIZE: %d\n", pReqParam->xM2M_AeName, sizeHead);

	httpHead = (char*) MALLOC(sizeHead);
	MEMSET(httpHead, NULL, sizeHead);

	SNPRINTF(httpHead, sizeHead, ONEM2M_HEAD_FORMAT_LONG_POLLING, gSiInName, gSiCseName,
				pReqParam->xM2M_URI, gSiServerIp, gSiServerPort, 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	STRINIT(pReqParam->http_SendData, sizeof(pReqParam->http_SendData));
	STRNCPY(pReqParam->http_SendData, httpHead, STRLEN(httpHead));
//	SNPRINTF(pReqParam->http_SendData, sizeof(pReqParam->http_SendData), "%s", httpHead);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# LONG POLLING SEND[%d]:\n%s", STRLEN(pReqParam->http_SendData), pReqParam->http_SendData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		if(pResParam->http_ResNum != HTTP_STATUS_CODE_REQUEST_TIME_OUT)
		{
			GRIB_LOGD("# %s-xM2M: %c[1;31mLONG POLLING FAIL: %s[%d]%c[0m\n", pReqParam->xM2M_AeName, 
				27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		}

		goto FINAL;
	}

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# LONG POLLING RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_LongPollingResParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: RESPONSE PARSING ERROR\n", pReqParam->xM2M_AeName);
		goto FINAL;
	}

	//shbaek: Add Command Result.
	iRes = Grib_CmdRequestParser(pResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: CMD REQUEST PARSING ERROR\n", pReqParam->xM2M_AeName);
		goto FINAL;
	}	

	if(iDBG)
	{
		GRIB_LOGD("# RESOURCE ID: [%s]\n", pResParam->xM2M_RsrcID);
		GRIB_LOGD("# PARENTS  ID: [%s]\n", pResParam->xM2M_PrntID);
		GRIB_LOGD("# CONTENT    : [%s]\n", pResParam->xM2M_Content);
	}

FINAL:
	FREE(httpHead);
	return iRes;
}

#define __ONEM2M_SEMANTIC_FUNC__
//2 shbaek: NEED: xM2M_Origin
int Grib_SemanticDescriptorUpload(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam)
{
	int   iRes = GRIB_ERROR;
	int   iDBG = gDebugOneM2M;

	char  httpHead[HTTP_MAX_SIZE_HEAD] = {'\0',};

	char  AUTO_LABEL[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  xM2M_AttrDCRP[GRIB_MAX_SIZE_BRIEF] = {'\0',};

	char  xM2M_AttrET[GRIB_MAX_SIZE_BRIEF] = {'\0',};
	char* xM2M_AttrRN = NULL;

	char* smdBuff = NULL;
	char* xM2M_AttrDSP = NULL;
	char* httpBody = NULL;
	char* httpSendMsg = NULL;
	int   httpSendMsgByte = 0;

#ifdef FEATURE_CAS
	char  pAuthBase64Src[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
	char  pAuthBase64Enc[GRIB_MAX_SIZE_MIDDLE] = {'\0',};
#endif

	//shbaek: Semantic Descriptor
	smdBuff 	 = (char *)MALLOC(HTTP_MAX_SIZE_BODY);
	xM2M_AttrDSP = (char *)MALLOC(HTTP_MAX_SIZE_BODY);

	MEMSET(smdBuff, 0x00, HTTP_MAX_SIZE_BODY);
	MEMSET(xM2M_AttrDSP, 0x00, HTTP_MAX_SIZE_BODY);

	iRes = Grib_SmdGetDeviceInfo(pReqParam->xM2M_AeName, smdBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s-xM2M: %c[1;31mGET DEVICE INFO FAIL: %s%c[0m\n", pReqParam->xM2M_AeName, 27, smdBuff, 27);
		return GRIB_ERROR;
	}
	
	Grib_Base64Encode(smdBuff, xM2M_AttrDSP, GRIB_NOT_USED);
	if(smdBuff)FREE(smdBuff);

	pReqParam->xM2M_ResourceType = ONEM2M_RESOURCE_TYPE_SEMANTIC_DESCRIPTOR;

	xM2M_AttrRN = ONEM2M_URI_CONTENT_SEM_DEC;
	if(iDBG)GRIB_LOGD("# RNAME: %s\n", xM2M_AttrRN);

	if(STRLEN(pReqParam->xM2M_AttrLBL) <= 1)
	{
		STRINIT(AUTO_LABEL, sizeof(AUTO_LABEL));
		SNPRINTF(AUTO_LABEL, sizeof(AUTO_LABEL), "%s_SemanticDescriptorLabel", pReqParam->xM2M_AeName);
	}

	SNPRINTF(xM2M_AttrDCRP, sizeof(xM2M_AttrDCRP), "%s:%d", HTTP_CONTENT_TYPE_RDF_XML, HTTP_ENC_TYPE_BASE64);
	if(iDBG)GRIB_LOGD("# DCRP: %s\n", xM2M_AttrDCRP);

	STRINIT(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID));
	SNPRINTF(pReqParam->xM2M_ReqID, sizeof(pReqParam->xM2M_ReqID), "%s_ReqSemanticDescriptorUpload", pReqParam->xM2M_AeName);
	if(iDBG)GRIB_LOGD("# REQ ID: %s\n", pReqParam->xM2M_ReqID);

	Grib_GetAttrExpireTime(xM2M_AttrET, GRIB_NOT_USED);
	if(iDBG)GRIB_LOGD("# Ex TIME: %s\n", xM2M_AttrET);

	//shbaek: HTTP Body
	httpBody = (char *)MALLOC(HTTP_MAX_SIZE_BODY+STRLEN(xM2M_AttrDSP)+1);
	MEMSET(httpBody, 0x00, HTTP_MAX_SIZE_BODY+STRLEN(xM2M_AttrDSP)+1);
	SNPRINTF(httpBody, HTTP_MAX_SIZE_BODY+STRLEN(xM2M_AttrDSP)+1, ONEM2M_BODY_FORMAT_SEMANTIC_DESCRIPTOR_UPLOAD,
				AUTO_LABEL, xM2M_AttrRN, xM2M_AttrET, xM2M_AttrDCRP, xM2M_AttrDSP);
	if(xM2M_AttrDSP)FREE(xM2M_AttrDSP);

#ifdef FEATURE_CAS
	SNPRINTF(pAuthBase64Src, sizeof(pAuthBase64Src), "%s:%s", pReqParam->xM2M_AeName, pReqParam->authKey);
	Grib_Base64Encode(pAuthBase64Src, pAuthBase64Enc, GRIB_NOT_USED);
#endif

	SNPRINTF(httpHead, sizeof(httpHead), ONEM2M_HEAD_FORMAT_SEMANTIC_DESCRIPTOR_UPLOAD, gSiInName, gSiCseName,
				pReqParam->xM2M_AeName, gSiServerIp, gSiServerPort, ONEM2M_RESOURCE_TYPE_SEMANTIC_DESCRIPTOR, STRLEN(httpBody), 
#ifdef FEATURE_CAS
				pAuthBase64Enc,
#endif
				pReqParam->xM2M_Origin, pReqParam->xM2M_ReqID);

	//shbaek: Send Message 
	httpSendMsgByte = STRLEN(httpHead)+STRLEN(httpBody)+1;
	httpSendMsg		= (char*)MALLOC(httpSendMsgByte);
	STRINIT(httpSendMsg, httpSendMsgByte);
	SNPRINTF(httpSendMsg, httpSendMsgByte, "%s%s", httpHead, httpBody);

	if(httpBody)FREE(httpBody);
	pReqParam->http_SendDataEx = httpSendMsg;

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# SEMANTIC DESCRIPTOR SEND[%d]:\n%s", STRLEN(pReqParam->http_SendDataEx), pReqParam->http_SendDataEx);
		GRIB_LOGD(GRIB_1LINE_DASH);
	}

	iRes = Grib_OneM2MSendMsg(pReqParam, pResParam);
	if(iRes != GRIB_DONE)
	{
		if(pResParam->http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{
			GRIB_LOGD("# %s-xM2M: %c[1;33mSEMANTIC DESCRIPTOR ALREADY EXIST ...%c[0m\n", pReqParam->xM2M_AeName, 27, 27);
		}
		else
		{
			GRIB_LOGD("# %s-xM2M: %c[1;31mCREATE SMD FAIL: %s [%d]%c[0m\n", pReqParam->xM2M_AeName, 
				27, pResParam->http_ResMsg, pResParam->http_ResNum, 27);
		}

		if(httpSendMsg)FREE(httpSendMsg);
		return GRIB_ERROR;
	}
	if(httpSendMsg)FREE(httpSendMsg);

	if(iDBG)
	{
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# SEMANTIC DESCRIPTOR RECV[%d]:\n%s\n", STRLEN(pResParam->http_RecvData), pResParam->http_RecvData);
		GRIB_LOGD(GRIB_1LINE_DASH);
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

	return iRes;
}


#define __ONEM2M_ETC_FUNC__

int Grib_CreateOneM2MTree(Grib_DbRowDeviceInfo* pRowDeviceInfo, char* pAuthKey)
{
	int i		= 0;
	int iRes	= GRIB_ERROR;
	int iDBG   	= gDebugOneM2M;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	Grib_ConfigInfo* pConfigInfo = NULL;

	if( (pRowDeviceInfo==NULL) || (pRowDeviceInfo->deviceID==NULL))
	{
		GRIB_LOGD("# PARAMETER IS NULL\n");
		return GRIB_ERROR;
	}

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# CREATE-TREE: %s\n", pRowDeviceInfo->deviceID);

	MEMSET(&reqParam, GRIB_INIT, sizeof(OneM2M_ReqParam));
	MEMSET(&resParam, GRIB_INIT, sizeof(OneM2M_ResParam));

#ifdef FEATURE_CAS
	reqParam.authKey = pAuthKey;
#endif

	STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

	//1 shbaek: 1.App Entity
	//shbaek: Your Device ID
	STRINIT(reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
	STRNCPY(reqParam.xM2M_NM, pRowDeviceInfo->deviceID, STRLEN(pRowDeviceInfo->deviceID));
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
	MEMSET(&resParam, 0x00, sizeof(resParam));
	//shbaek: Create Polling Channel Container(need xM2M_AeName)
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
		MEMSET(&resParam, 0x00, sizeof(resParam));
		STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(reqParam.xM2M_URI, reqParam.xM2M_AeName, STRLEN(reqParam.xM2M_AeName));
		STRINIT(reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(reqParam.xM2M_NM, pFuncName, STRLEN(pFuncName));
		iRes = Grib_ContainerCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
			{//shbaek: Already Exist is Not Error.
#if __CACHE_RI_TABLE__
				STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
				SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s", reqParam.xM2M_AeName, pFuncName);
				Grib_ContainerRetrieve(&reqParam, &resParam);
#endif
			}
			else
			{
				goto ERROR;
			}
		}

		if(FUNC_ATTR_CHECK_REPORT(pRowDeviceFunc->funcAttr))
		{//shbaek: Use Report Status.
			//1 shbaek: 3-1.Status
			//shbaek: Set URI -> in/cse/"Device ID"/"Func"
			MEMSET(&resParam, 0x00, sizeof(resParam));
			STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
			SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s", reqParam.xM2M_AeName, pFuncName);
			STRINIT(reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
			STRNCPY(reqParam.xM2M_NM, ONEM2M_URI_CONTENT_STATUS, STRLEN(ONEM2M_URI_CONTENT_STATUS));
			//shbaek: Create Status Container
			iRes = Grib_ContainerCreate(&reqParam, &resParam);
			if(iRes == GRIB_ERROR)
			{
				if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
				{//shbaek: Already Exist is Not Error.

#if __CACHE_RI_TABLE__
					STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
					SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", reqParam.xM2M_AeName, pFuncName, reqParam.xM2M_NM);
					Grib_ContainerRetrieve(&reqParam, &resParam);
#endif
				}
				else
				{
					goto ERROR;
				}
			}
		}//shbaek: Use Report Status.

		if(FUNC_ATTR_CHECK_CONTROL(pRowDeviceFunc->funcAttr))
		{//shbaek: Use Control Command.

			//1 shbaek: 3-2. RESULT -> ICBMS 3rd
			//shbaek: Set URI -> in/cse/"Device ID"/"Func"
			MEMSET(&resParam, 0x00, sizeof(resParam));
			STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
			SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s", reqParam.xM2M_AeName, pFuncName);
			STRINIT(reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
			STRNCPY(reqParam.xM2M_NM, ONEM2M_URI_CONTENT_RESULT, STRLEN(ONEM2M_URI_CONTENT_RESULT));
			//shbaek: Create Result Container
			iRes = Grib_ContainerCreate(&reqParam, &resParam);
			if(iRes == GRIB_ERROR)
			{
				if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
				{//shbaek: Already Exist is Not Error.

				}
				else
				{
					goto ERROR;
				}
			}

			//1 shbaek: 3-3.Execute
			//shbaek: Set URI -> in/cse/"Device ID"/"Func"
			MEMSET(&resParam, 0x00, sizeof(resParam));
			STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
			SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s", reqParam.xM2M_AeName, pFuncName);
			STRINIT(reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
			STRNCPY(reqParam.xM2M_NM, ONEM2M_URI_CONTENT_EXECUTE, STRLEN(ONEM2M_URI_CONTENT_EXECUTE));
			//shbaek: Create Execute Container
			iRes = Grib_ContainerCreate(&reqParam, &resParam);
			if(iRes == GRIB_ERROR)
			{
				if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
				{//shbaek: Already Exist is Not Error.
#if __CACHE_RI_TABLE__
					MEMSET(&resParam, 0x00, sizeof(resParam));
					STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
					SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", reqParam.xM2M_AeName, pFuncName, reqParam.xM2M_NM);
					Grib_ContainerRetrieve(&reqParam, &resParam);
#endif
				}
				else
				{
					goto ERROR;
				}
			}

#if __CACHE_RI_TABLE__
			//2 shbaek: NEED EXECUTE's RESOURCE ID
			STRINIT(pRowDeviceFunc->exRsrcID, sizeof(pRowDeviceFunc->exRsrcID));
			STRNCPY(pRowDeviceFunc->exRsrcID, resParam.xM2M_RsrcID, STRLEN(resParam.xM2M_RsrcID));
			if(iDBG)GRIB_LOGD("# %s: %s EXECUTE RESOURCE ID: %s\n", pRowDeviceInfo->deviceID, pFuncName, pRowDeviceFunc->exRsrcID);
#endif

			//1 shbaek: 3-3-1.Subscription
			MEMSET(&resParam, 0x00, sizeof(resParam));
			STRINIT(reqParam.xM2M_Func, sizeof(reqParam.xM2M_Func));
			STRNCPY(reqParam.xM2M_Func, pFuncName, STRLEN(pFuncName));
			//shbaek: Create Subscription(need xM2M_AeName, xM2M_Func)
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
		}//shbaek: Use Control Command.

	}

	if(pRowDeviceInfo->deviceInterface == DEVICE_IF_TYPE_INTERNAL)
	{//3 shbaek: No Have Semantic Descriptor Type
		if(iDBG)GRIB_LOGD("# %s TREE: NO HAVE SEMANTIC DESCRIPTOR ...\n", pRowDeviceInfo->deviceID);
		return GRIB_DONE;
	}

	//1 shbaek: 2-3.Semantic Descriptor
#if 1//__NOT_USED__
	//shbaek: Upload Semantic Descriptor(need xM2M_AeName)
	iRes = Grib_SemanticDescriptorUpload(&reqParam, &resParam);
	if(iRes != GRIB_DONE)
	{
		if(resParam.http_ResNum == HTTP_STATUS_CODE_CONFLICT)
		{//shbaek: Already Exist is Not Error.
			// TODO: shbaek: What Can I Do ... ?
		}
		else
		{
			GRIB_LOGD("# %s TREE: SEMANTIC DESCRIPTOR ERROR !!!\n", pRowDeviceInfo->deviceID);
			//goto ERROR;
		}
	}
#else
	GRIB_LOGD("# %c[1;33mSkip Semantic Descriptor ...%c[0m\n", 27, 27);
#endif

	return GRIB_DONE;

ERROR:
	GRIB_LOGD("# CREATE ONE M2M TREE ERROR\n");
	return GRIB_ERROR;
}

int Grib_UpdateHubInfo(char* pAuthKey)
{
	int  iRes = GRIB_ERROR;
	char pIpAddr[GRIB_MAX_SIZE_IP_STR] = {'\0', };

	Grib_ConfigInfo* pConfigInfo = NULL;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

#ifdef FEATURE_CAS
	if(STRLEN(pAuthKey)<=1)
	{
		GRIB_LOGD("# AUTH INFO NULL ERROR !!!\n");
		return GRIB_ERROR;
	}
#endif

	STRINIT(pIpAddr, sizeof(pIpAddr));

	MEMSET(&reqParam, 0x00, sizeof(reqParam));
	MEMSET(&resParam, 0x00, sizeof(resParam));

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("LOAD CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# %s VERSION: %c[1;33m%s%c[0m\n", pConfigInfo->hubID, 27, GRIB_HUB_VERSION, 27);

	iRes = Grib_GetIPAddr(pIpAddr);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# UPDATE HUB INFO: GET HOST NAME ERROR !!!\n");
	}
	GRIB_LOGD("# %s IP ADDR: %c[1;33m%s%c[0m\n", pConfigInfo->hubID, 27, pIpAddr, 27);

	if(STRLEN(pIpAddr) == 0)
	{
		STRNCPY(pIpAddr, "0.0.0.0", STRLEN("0.0.0.0"));
	}

	STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

	STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
	STRNCPY(reqParam.xM2M_AeName, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

	STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
	SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", 
		reqParam.xM2M_AeName, ONEM2M_URI_CONTENT_HUB, ONEM2M_URI_CONTENT_STATUS);

	STRINIT(reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON));
	SNPRINTF(reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON), ONEM2M_FORMAT_CONTENT_VALUE_HUB_INFO, 
		GRIB_HUB_VERSION, pConfigInfo->hubID, pIpAddr);

#ifdef FEATURE_CAS
	reqParam.authKey = pAuthKey;
#endif

	iRes = Grib_ContentInstanceCreate(&reqParam, &resParam);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("# UPDATE HUB INFO: CREATE INSTANCE ERROR !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# UPDATE HUB INFO: DONE\n");

	return GRIB_SUCCESS;

}

int Grib_UpdateDeviceInfo(Grib_DbAll* pDbAll, char* pAuthKey)
{
	int  i = 0;
	int  iRes = GRIB_ERROR;
	Grib_ConfigInfo* pConfigInfo = NULL;

	if(pDbAll == NULL)
	{
		GRIB_LOGD("# DEVICE INFO NULL ERROR !!!\n");
		return GRIB_ERROR;
	}

#ifdef FEATURE_CAS
	if(STRLEN(pAuthKey)<=1)
	{
		GRIB_LOGD("# AUTH INFO NULL ERROR !!!\n");
		return GRIB_ERROR;
	}
#endif

	MEMSET(&gReqParam, 0x00, sizeof(gReqParam));
	MEMSET(&gResParam, 0x00, sizeof(gResParam));

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("LOAD CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}
	GRIB_LOGD("# UPDATE DEVICE COUNT: %d EA\n", pDbAll->deviceCount);

	STRINIT(gReqParam.xM2M_Origin, sizeof(gReqParam.xM2M_Origin));
	STRNCPY(gReqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

	STRINIT(gReqParam.xM2M_AeName, sizeof(gReqParam.xM2M_AeName));
	STRNCPY(gReqParam.xM2M_AeName, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

	STRINIT(gReqParam.xM2M_URI, sizeof(gReqParam.xM2M_URI));
	SNPRINTF(gReqParam.xM2M_URI, sizeof(gReqParam.xM2M_URI), "%s/%s/%s", 
		gReqParam.xM2M_AeName, ONEM2M_URI_CONTENT_DEVICE, ONEM2M_URI_CONTENT_STATUS);

	STRINIT(gReqParam.xM2M_CON, sizeof(gReqParam.xM2M_CON));

	if(pDbAll->deviceCount<=0)
	{
		STRNCPY(gReqParam.xM2M_CON, "NO_DEVICE", STRLEN("NO_DEVICE"));
	}

	for(i=0; i<pDbAll->deviceCount; i++)
	{
		if(i != 0)
		{
			STRCAT(gReqParam.xM2M_CON, ", ");
		}

		STRCAT(gReqParam.xM2M_CON, pDbAll->ppRowDeviceInfo[i]->deviceID);
/*
		STRCAT(gReqParam.xM2M_CON, "(");
		STRCAT(gReqParam.xM2M_CON, pDbAll->ppRowDeviceInfo[i]->deviceAddr);
		STRCAT(gReqParam.xM2M_CON, ")");
*/
	}

#ifdef FEATURE_CAS
	gReqParam.authKey = pAuthKey;
#endif

	iRes = Grib_ContentInstanceCreate(&gReqParam, &gResParam);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("# UPDATE DEVICE INFO: CREATE INSTANCE ERROR !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# UPDATE DEVICE INFO: DONE\n");

	return GRIB_SUCCESS;

}

