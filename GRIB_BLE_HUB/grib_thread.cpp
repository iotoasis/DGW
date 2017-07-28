/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_thread.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int gDebugThread;
char* gHubID;

Grib_DbAll* 	  		gDevInfoAll;
Grib_DbCacheRiAll*		gCacheRiAll;

Grib_HubThreadInfo*		gHubThread;
Grib_HubThreadInfo*		gResetThread;
Grib_HubThreadInfo*		gWatchDogThread;
Grib_DeviceThreadInfo** gBleThreadList;

#define TEST_USE_ONLY_BLE		FALSE

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_SetThreadConfig(void)
{
	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("GET CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	gDebugThread = pConfigInfo->debugLevel;
	GRIB_LOGD("# SET THREAD DEBUG: %s\n", GRIB_BOOL_TO_STR(gDebugThread));

	if(gHubID == NULL)
	{
		gHubID = STRDUP(pConfigInfo->hubID);
		GRIB_LOGD("# READ HUB ID: %s\n", gHubID);
	}

	return GRIB_DONE;
}

int Grib_DeviceCheckAttr(Grib_DbRowDeviceInfo* pRowDeviceInfo)
{
	int i = 0;
	int iDBG = gDebugThread;
	int iAttr = 0;
	Grib_DbRowDeviceFunc* pRowDeviceFunc;

	for(i=0; i<pRowDeviceInfo->deviceFuncCount; i++)
	{
		pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];

		iAttr |= pRowDeviceFunc->funcAttr;
		if(iDBG)GRIB_LOGD("# %s-CHECK: %s [%s][%d]\n", 
			pRowDeviceInfo->deviceID, pRowDeviceFunc->funcName, Grib_FuncAttrToStr(pRowDeviceFunc->funcAttr), pRowDeviceFunc->funcAttr);
	}

	if(iDBG)GRIB_LOGD("# %s-CHECK: HAVE [%s][%d]\n", 
		pRowDeviceInfo->deviceID, Grib_FuncAttrToStr(iAttr), iAttr);
	
	return iAttr;
}

#define __GRIB_CACHE_RI_FUNC__

int Grib_MatchFuncName(char* rid, char* funcName)
{
	const char* FUNC_TAG = "MATCH-FNAME";

	int i = 0;
	int iRes = GRIB_DONE;
	int iDBG = gDebugThread;
	Grib_DbRowCacheRi rowCacheRi;
	Grib_DbRowCacheRi* pRowCacheRi = NULL;

	if(STRLEN(rid) == 0)
	{
		GRIB_LOGD("# %s: IN-VALID RESOURCE ID !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# %s: RESOURCE ID: %s\n", FUNC_TAG, rid);

	if(funcName == NULL)
	{
		GRIB_LOGD("# %s: IN-VALID BUFFER !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	if(gCacheRiAll == NULL)
	{
		gCacheRiAll = (Grib_DbCacheRiAll*)MALLOC(sizeof(Grib_DbCacheRiAll));
		MEMSET(gCacheRiAll, GRIB_INIT, sizeof(Grib_DbCacheRiAll));

		iRes = Grib_DbGetCacheRiAll(gCacheRiAll);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# %s: CACHE TABLE GET ERROR !!!\n", FUNC_TAG);
			return iRes;
		}

		GRIB_LOGD("# %s: GET CACHE TABLE DONE ...\n", FUNC_TAG);
	}

	for(i=0; i<gCacheRiAll->cacheCount; i++)
	{
		pRowCacheRi = gCacheRiAll->ppRowCacheRi[i];
		if( !STRNCMP(pRowCacheRi->rid, rid, STRLEN(rid)) )
		{//shbaek: Matching Resource ID
			STRINIT(funcName, sizeof(funcName));
			STRNCPY(funcName, pRowCacheRi->rname, STRLEN(pRowCacheRi->rname));
		}
	}

	if(iDBG)GRIB_LOGD("# %s: FUNC NAME  : %s\n", FUNC_TAG, funcName);

	return iRes;
}

int Grib_FindFuncName(char* rid, char* funcName)
{
	const char* FUNC_TAG = "FIND-FNAME";

	int iRes = GRIB_DONE;
	int iDBG = gDebugThread;
	Grib_DbRowCacheRi rowCacheRi;

	if(STRLEN(rid) == 0)
	{
		GRIB_LOGD("# %s: IN-VALID RESOURCE ID !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# %s: RESOURCE ID: %s\n", FUNC_TAG, rid);

	if(funcName == NULL)
	{
		GRIB_LOGD("# %s: IN-VALID BUFFER !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}
	
	MEMSET(&rowCacheRi, 0x00, sizeof(Grib_DbRowCacheRi));

	STRINIT(rowCacheRi.rid, sizeof(rowCacheRi.rid));
	STRNCPY(rowCacheRi.rid, rid, STRLEN(rid));

	iRes = Grib_DbGetCacheInfo(&rowCacheRi);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: %s GET INFO IS ERROR !!!\n", FUNC_TAG, rid);
		return GRIB_ERROR;
	}

	STRINIT(funcName, sizeof(funcName));
	STRNCPY(funcName, rowCacheRi.rname, STRLEN(rowCacheRi.rname));

	if(iDBG)
	{
		GRIB_LOGD("# %s: RESOURCE ID   : %s\n", FUNC_TAG, rowCacheRi.rid);
		GRIB_LOGD("# %s: RESOURCE NAME : %s\n", FUNC_TAG, rowCacheRi.rname);
		GRIB_LOGD("# %s: RESOURCE URI  : %s\n", FUNC_TAG, rowCacheRi.uri);
	}

	return iRes;
}
int Grib_MakeCacheRi(Grib_DbAll* pDbAll, Grib_DeviceThreadInfo** pDevThreadList)
{
	const char* FUNC_TAG = "MAKE-CACHE-RI";

	int i = 0;
	int x = 0;
	int iRes = GRIB_ERROR;
	int DEVICE_MAX_COUNT = 0;
	int FUNC_COUNT = 0;

	char* DEVICE_ID = NULL;

	Grib_ConfigInfo* pConfigInfo = NULL;

	Grib_DbRowDeviceInfo* pRowDeviceInfo = NULL;
	Grib_DbRowDeviceFunc* pRowDeviceFunc = NULL;
	Grib_DbRowCacheRi rowCacheRi;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

#if (TEST_USE_ONLY_BLE)
	GRIB_LOGD("# MAKE CACHE TABLE: %c[1;33mTEST_USE_ONLY_BLE !!!%c[0m\n", 27, 27);
	return GRIB_DONE;
#endif

	GRIB_LOGD("# %s: MAKE CACHE TABLE ...\n", FUNC_TAG);

	iRes = Grib_DbDelCacheRiAll();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: CLEAN CACHE TABLE ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	if( (pDbAll==NULL) || (pDevThreadList==NULL) )
	{
		GRIB_LOGD("# %s: PARAM IN NULL ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# %s: GET CONFIG ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	MEMSET(&reqParam, GRIB_INIT, sizeof(OneM2M_ReqParam));

	DEVICE_MAX_COUNT = pDbAll->deviceCount;
	GRIB_LOGD("# %s: TOTAL DEVICE COUNT: %d\n", FUNC_TAG, DEVICE_MAX_COUNT);

	STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	SNPRINTF(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin), "GRIB/%s", pConfigInfo->hubID);

	for(i=0; i<DEVICE_MAX_COUNT; i++)
	{
		pRowDeviceInfo = pDevThreadList[i]->pRowDeviceInfo;
		FUNC_COUNT = pRowDeviceInfo->deviceFuncCount;
		DEVICE_ID = pRowDeviceInfo->deviceID;

#ifdef FEATURE_CAS
		reqParam.authKey = pDevThreadList[i]->authKey;
#endif

		for(x=0; x<FUNC_COUNT; x++)
		{
			pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[x];

			if(FUNC_ATTR_CHECK_CONTROL(pRowDeviceFunc->funcAttr))
			{
				MEMSET(&resParam, GRIB_INIT, sizeof(OneM2M_ResParam));

				STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
				STRNCPY(reqParam.xM2M_AeName, DEVICE_ID, STRLEN(DEVICE_ID));	

				STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
				SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", DEVICE_ID, 
																		pRowDeviceFunc->funcName,
																		ONEM2M_URI_CONTENT_EXECUTE);
				iRes = Grib_ContainerRetrieve(&reqParam, &resParam);
				if(iRes != GRIB_DONE)
				{
					GRIB_LOGD("# %s: %s-%s RETRIEVE ERROR: %s[%d]\n", FUNC_TAG, DEVICE_ID, pRowDeviceFunc->funcName,
																		resParam.http_ResMsg, resParam.http_ResNum);
					return GRIB_ERROR;
				}

				MEMSET(&rowCacheRi, 0x00, sizeof(Grib_DbRowCacheRi));

				STRINIT(rowCacheRi.rname, sizeof(rowCacheRi.rname));
				STRNCPY(rowCacheRi.rname, pRowDeviceFunc->funcName, STRLEN(pRowDeviceFunc->funcName));	

				STRINIT(rowCacheRi.uri, sizeof(rowCacheRi.uri));
				STRNCPY(rowCacheRi.uri, reqParam.xM2M_URI, STRLEN(reqParam.xM2M_URI));	

				rowCacheRi.rtype = ONEM2M_RESOURCE_TYPE_CONTAINER;

				STRINIT(rowCacheRi.rid, sizeof(rowCacheRi.rid));
				STRNCPY(rowCacheRi.rid, resParam.xM2M_RsrcID, STRLEN(resParam.xM2M_RsrcID));	

				STRINIT(rowCacheRi.rid, sizeof(rowCacheRi.rid));
				STRNCPY(rowCacheRi.rid, resParam.xM2M_RsrcID, STRLEN(resParam.xM2M_RsrcID));	

				STRINIT(rowCacheRi.pid, sizeof(rowCacheRi.pid));
				STRNCPY(rowCacheRi.pid, resParam.xM2M_PrntID, STRLEN(resParam.xM2M_PrntID));

				iRes = Grib_DbSetCacheRi(&rowCacheRi);
				if(iRes != GRIB_DONE)
				{
					GRIB_LOGD("# %s: %s-%s DB SET ERROR !!!\n", FUNC_TAG, DEVICE_ID, pRowDeviceFunc->funcName);
					return GRIB_ERROR;
				}

			}
			else
			{//shbaek: Not Used Control ...
				continue;
			}

		}//shbaek: Func
	}//shbaek: Device

	return GRIB_DONE;
}

#define __GRIB_SYSTEM_THREAD_FUNC__
void* Grib_ResetThread(void* threadArg)
{
	int iDBG = FALSE;//gDebugThread;
	int intervalTime = 24;
	
	time_t sysTimer;
	struct tm *sysTime;

	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# RESET-THREAD: GET CONFIG ERROR !!!\n");
		return NULL;
	}

	if(pConfigInfo->resetTimeHour == 0)
	{
		GRIB_LOGD("# RESET-THREAD: SWITCH OFF !!!\n");
		return NULL;
	}

	if( (0<pConfigInfo->resetTimeHour) && (pConfigInfo->resetTimeHour<=24) )
	{
		intervalTime = pConfigInfo->resetTimeHour;		
	}
	GRIB_LOGD("# RESET-THREAD: INTERVAL TIME: %d\n", intervalTime);

	while(TRUE)
	{
		sysTimer = time(NULL);
		sysTime  = localtime(&sysTimer);
		if(iDBG)GRIB_LOGD("# RESET-THREAD: %02d:%02d:%02d\n", sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
		SLEEP(10);

		if( ((sysTime->tm_hour%intervalTime)==0) && (sysTime->tm_min==0) )
		{
			char rebootMsg[SIZE_1K] = {'\0', };

			SNPRINTF(rebootMsg, sizeof(rebootMsg), "Time is Up ...\n"
				"Reset Time Hour: %d\n", pConfigInfo->resetTimeHour);
			systemReboot(30, rebootMsg);
		}
	}

	return threadArg;
}

void* Grib_WatchDogThread(void* threadArg)
{
	const char* FUNC = "WATCH-DOG";
	int iDBG = FALSE;//gDebugThread;
	
	time_t sysTimer;
	struct tm *sysTime;

	const int  WATCHDOG_WAIT_TIME_SEC = 60 * 3;
	const int  MAX_CONTINUAL_ERROR_COUNT = 10;

	int i = 0;
	int TOTAL_DEVICE_COUNT = 0;
	int bleErrorDeviceCount = 0;
	int notWorkDeviceCount = 0;
	int watchdogStatus = WATCHDOG_STATUS_NONE;
	Grib_DeviceThreadInfo** threadList;

	GRIB_LOGD("# %s: START ...\n", FUNC);

	threadList = gBleThreadList;
	TOTAL_DEVICE_COUNT = gDevInfoAll->deviceCount;

	if(threadList == NULL)
	{
		GRIB_LOGD("# %s: THREAD LIST IS NULL\n", FUNC);
		return threadArg;
	}

	if(TOTAL_DEVICE_COUNT < 1)
	{
		GRIB_LOGD("# %s: NO DEVICE ...\n", FUNC);
		return threadArg;
	}

	while(TRUE)
	{

		for(i=1; i<=WATCHDOG_WAIT_TIME_SEC; i++)
		{
			sysTimer = time(NULL);
			sysTime  = localtime(&sysTimer);

			if(i%30 == 0)GRIB_LOGD("# %s: %c[1;33mWAIT %d SEC [%02d:%02d:%02d]%c[0m\n", FUNC,
				27, i, sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec, 27);

			SLEEP(1);
		}

		bleErrorDeviceCount = 0;
		notWorkDeviceCount = 0;

		for(i=0; i<TOTAL_DEVICE_COUNT; i++)
		{
			pthread_mutex_lock(&threadList[i]->threadMutex);
			watchdogStatus = threadList[i]->watchdogStatus;
			pthread_mutex_unlock(&threadList[i]->threadMutex);

			//shbaek: STATUS CHECK
			if(watchdogStatus == WATCHDOG_STATUS_DONE)
			{//shbaek: Working ...
				pthread_mutex_lock(&threadList[i]->threadMutex);
				threadList[i]->watchdogStatus = WATCHDOG_STATUS_NONE;
				pthread_mutex_unlock(&threadList[i]->threadMutex);
			}
			else
			{//shbaek: Not Working !!!
				GRIB_LOGD("# %s: %c[1;33m%s NOT WORKING !!!%c[0m\n", FUNC, 27, threadList[i]->pRowDeviceInfo->deviceID, 27);
				notWorkDeviceCount++;
				bleErrorDeviceCount++;
				continue;
			}

			//shbaek: BLE ERROR CHECK
			if(MAX_CONTINUAL_ERROR_COUNT < threadList[i]->bleContinualError)
			{//shbaek: Device Continual Error !!!
				GRIB_LOGD("# %s: %c[1;33m%s CONTINUAL ERROR COUNT: %d%c[0m\n", FUNC, 
					27, threadList[i]->pRowDeviceInfo->deviceID, threadList[i]->bleContinualError, 27);
				bleErrorDeviceCount++;
			}

		}

		if(bleErrorDeviceCount == TOTAL_DEVICE_COUNT)
		{//3 shbaek: All Device Continual Error !!!
			const char* pCommand	= "./" GRIB_PROGRAM_REBOOT " timer 30 \"Ble Continual Error !!!\"";
			char  pBuffer[SIZE_1K] = {'\0', };

			systemCommand(pCommand, pBuffer, sizeof(pBuffer));
		}

		if(notWorkDeviceCount == TOTAL_DEVICE_COUNT)
		{//3 shbaek: All Device Runaway !!!
			const char* pCommand	= "./" GRIB_PROGRAM_REBOOT " timer 30 \"All Device Runaway !!!\"";
			char  pBuffer[SIZE_1K] = {'\0', };

			systemCommand(pCommand, pBuffer, sizeof(pBuffer));
		}

	}

	return threadArg;
}

int Grib_SystemThreadStart(void)
{
	int iRes = GRIB_ERROR;

#if __NOT_USED__
	//3 shbaek: Reset Thread Init & Create
	GRIB_LOGD("# RESET THREAD CREATE ...\n");
	gResetThread = (Grib_HubThreadInfo*)MALLOC(sizeof(Grib_HubThreadInfo));
	MEMSET(gResetThread, 0x00, sizeof(Grib_HubThreadInfo));

	pthread_attr_init(&gResetThread->threadAttr);
	pthread_cond_init(&gResetThread->threadCond, GRIB_NOT_USED);
	pthread_mutex_init(&gResetThread->threadMutex, GRIB_NOT_USED);
	pthread_create(&gResetThread->threadID, &gResetThread->threadAttr, Grib_ResetThread, (void *)gResetThread);
	SLEEP(1);
#endif

	//3 shbaek: Watch Dog Thread Init & Create
	GRIB_LOGD("# WATCH DOG THREAD CREATE ...\n");
	gWatchDogThread = (Grib_HubThreadInfo*)MALLOC(sizeof(Grib_HubThreadInfo));
	MEMSET(gWatchDogThread, 0x00, sizeof(Grib_HubThreadInfo));

	pthread_attr_init(&gWatchDogThread->threadAttr);
	pthread_cond_init(&gWatchDogThread->threadCond, GRIB_NOT_USED);
	pthread_mutex_init(&gWatchDogThread->threadMutex, GRIB_NOT_USED);
	pthread_create(&gWatchDogThread->threadID, &gWatchDogThread->threadAttr, Grib_WatchDogThread, (void *)gWatchDogThread);
	SLEEP(1);

	return iRes;
}


#define __GRIB_BLE_THREAD_FUNC__

void* Grib_HubThread(void* threadArg)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugThread;
	int needRegi = FALSE;

	Grib_HubThreadInfo *pThreadInfo;

	char* deviceID = NULL;
	char* cmdConValue = NULL;

#ifdef FEATURE_CAS
	char pAuthKey[GRIB_MAX_SIZE_AUTH_KEY] = {'\0', };
#else
	char* pAuthKey = NULL;
#endif

	OneM2M_ReqParam hubReqParam;
	OneM2M_ResParam hubResParam;

	if( (gHubID==NULL) || (STRLEN(gHubID)<=1) )
	{
		GRIB_LOGD("# %ld HUB ID IS NULL", pthread_self());

		iRes = Grib_SetThreadConfig();
		if(iRes != GRIB_DONE)
		{
			return NULL;
		}
	}

#if (TEST_USE_ONLY_BLE)
	GRIB_LOGD("# HUB-THREAD: %c[1;33mTEST_USE_ONLY_BLE !!!%c[0m\n", 27, 27);
	return threadArg;
#endif

	if(threadArg == NULL)
	{
		GRIB_LOGD("# %s: THREAD INFO IS NULL !!!\n", gHubID);
		return NULL;
	}

	//shbaek: Pre-Pare
	pThreadInfo	= (Grib_HubThreadInfo *)threadArg;
	deviceID	= gHubID;

	MEMSET(&hubReqParam, GRIB_INIT, sizeof(OneM2M_ReqParam));
	MEMSET(&hubResParam, GRIB_INIT, sizeof(OneM2M_ResParam));

	STRINIT(hubReqParam.xM2M_Origin, sizeof(hubReqParam.xM2M_Origin));
	SNPRINTF(hubReqParam.xM2M_Origin, sizeof(hubReqParam.xM2M_Origin), "GRIB/%s", gHubID);

	STRINIT(hubReqParam.xM2M_AeName, sizeof(hubReqParam.xM2M_AeName));
	STRNCPY(hubReqParam.xM2M_AeName, gHubID, STRLEN(gHubID));		

#ifdef FEATURE_CAS
	iRes = Grib_CasGetAuthKey(deviceID, pAuthKey);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: GET AUTH KEY ERROR !!!\n\n", deviceID);
		return threadArg;
	}
	//GRIB_LOGD("# %s: AUTH KEY: %s\n", deviceID, pAuthKey);
	hubReqParam.authKey = pAuthKey;
#endif

	GRIB_LOGD("# %s: HUB THREAD START ...\n", deviceID);

	STRINIT(hubReqParam.xM2M_URI, sizeof(hubReqParam.xM2M_URI));
	SNPRINTF(hubReqParam.xM2M_URI, sizeof(hubReqParam.xM2M_URI), "%s/%s/%s", 
		gHubID, ONEM2M_URI_CONTENT_HUB, ONEM2M_URI_CONTENT_STATUS);
	iRes = Grib_ContainerRetrieve(&hubReqParam, &hubResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: [URI: %s] [MSG: %s (%d)]\n", deviceID, hubReqParam.xM2M_URI, hubResParam.http_ResMsg, hubResParam.http_ResNum);

		if(hubResParam.http_ResNum == HTTP_STATUS_CODE_NOT_FOUND)
		{//shbaek: Need Create Tree ...
			needRegi = TRUE;
		}
	}

	STRINIT(hubReqParam.xM2M_URI, sizeof(hubReqParam.xM2M_URI));
	SNPRINTF(hubReqParam.xM2M_URI, sizeof(hubReqParam.xM2M_URI), "%s/%s/%s", 
		gHubID, ONEM2M_URI_CONTENT_DEVICE, ONEM2M_URI_CONTENT_STATUS);
	iRes = Grib_ContainerRetrieve(&hubReqParam, &hubResParam);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: [URI: %s] [MSG: %s (%d)]\n", deviceID, hubReqParam.xM2M_URI, hubResParam.http_ResMsg, hubResParam.http_ResNum);

		if(hubResParam.http_ResNum == HTTP_STATUS_CODE_NOT_FOUND)
		{//shbaek: Need Create Tree ...
			needRegi = TRUE;
		}
	}

	GRIB_LOGD("# %s: NEED CREATE TREE: %s\n\n", deviceID, GRIB_BOOL_TO_STR(needRegi));

	if(needRegi == TRUE)
	{
		iRes = Grib_HubRegi(pAuthKey);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# %s: HUB TREE CREATE ERROR !!!\n", deviceID);
		}
	}
	iRes = Grib_UpdateHubInfo(pAuthKey);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: HUB INFO UPDATE ERROR !!!\n", deviceID);
	}

	iRes = Grib_UpdateDeviceInfo(gDevInfoAll, pAuthKey);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: DEVICE INFO UPDATE ERROR !!!\n", deviceID);
	}

	STRINIT(hubReqParam.xM2M_URI, sizeof(hubReqParam.xM2M_URI));

	while(TRUE)
	{
		GRIB_LOGD("\n");
//		GRIB_LOGD("# %s: POLLING START\n", deviceID);

		iRes = Grib_LongPolling(&hubReqParam, &hubResParam);
		if(iRes != GRIB_DONE)
		{
			if(hubResParam.http_ResNum == HTTP_STATUS_CODE_REQUEST_TIME_OUT)
			{
				GRIB_LOGD("# %s: POLLING TIME OUT\n", deviceID);
			}
#ifdef FEATURE_CAS
			else if(hubResParam.http_ResNum == HTTP_STATUS_CODE_UNAUTHORIZED)
			{//shbaek: Change Auth Key
				GRIB_LOGD("# %s: CHANGE AUTH KEY !!!\n", deviceID);
				Grib_CasGetAuthKey(deviceID, pAuthKey);
				hubReqParam.authKey = pAuthKey;
			}
#endif
			else
			{
				GRIB_LOGD("# %s: POLLING ERROR MSG: %s[%d]\n", deviceID, hubResParam.http_ResMsg, hubResParam.http_ResNum);
				SLEEP(1);
			}

			continue;
		}

		cmdConValue = hubResParam.xM2M_Content;
		if(iDBG)GRIB_LOGD("# %s: COMMAND: %s\n", deviceID, cmdConValue);

		if(STRCASECMP(cmdConValue, "reboot") == 0)
		{
			char pBuff[GRIB_MAX_SIZE_BRIEF] = {'\0', };
			const char* REBOOT_COMMAND = "sudo reboot";

			GRIB_LOGD("# %s: RESET TIME !!!\n", deviceID);
			SLEEP(10);
			systemCommand(REBOOT_COMMAND, pBuff, sizeof(pBuff));
		}
		else
		{
			GRIB_LOGD("# %s: UN-KNOWN COMMAND: %s\n", deviceID, cmdConValue);
		}

	}

	return pThreadInfo;
}

void* Grib_ReportThread(void* threadArg)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = gDebugThread;
	int iCycleTime = 0;
	int checkStatus = THREAD_STATUS_NONE;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	Grib_DeviceThreadInfo *pThreadInfo;
	Grib_DbRowDeviceInfo* pDeviceInfo;
	Grib_DbRowDeviceFunc** ppFuncInfo;

	char* deviceID = NULL;
	char* pFuncName = NULL;
	char* pRes = NULL;

	char bleSendBuff[BLE_MAX_SIZE_SEND_MSG];
	char bleRecvBuff[BLE_MAX_SIZE_RECV_MSG];

	int iTimeCheck = gDebugThread;
	time_t sysTimer;
	struct tm *sysTime;

	if(threadArg == NULL)
	{
		GRIB_LOGD("%ld THREAD INFO IS NULL", pthread_self());
		return NULL;
	}

	//shbaek: Pre-Pare
	pThreadInfo	= (Grib_DeviceThreadInfo *)threadArg;
	pDeviceInfo	= pThreadInfo->pRowDeviceInfo;
	ppFuncInfo	= pDeviceInfo->ppRowDeviceFunc;
	deviceID	= pDeviceInfo->deviceID;
	iCycleTime	= pDeviceInfo->reportCycle;
	if(iCycleTime < 1)
	{
		iCycleTime = GRIB_DEFAULT_REPORT_CYCLE;
	}

//	GRIB_LOGD("# %s-RPT>: REPORT THREAD START\n", deviceID);

	MEMSET(&reqParam, GRIB_INIT, sizeof(OneM2M_ReqParam));
	MEMSET(&resParam, GRIB_INIT, sizeof(OneM2M_ResParam));

	STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	SNPRINTF(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin), "GRIB/%s", gHubID);

	STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
	STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));	

	STRINIT(reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF));
	SNPRINTF(reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF), "%s:0", HTTP_CONTENT_TYPE_TEXT);

#ifdef FEATURE_CAS
	if(STRLEN(pThreadInfo->authKey)<=1)
	{
		GRIB_LOGD("# %s-RPT>: %c[1;31m%sAUTH KEY IN-VALID ERROR !!!%c[0m\n\n", deviceID, 27, 27);
//		return threadArg;
	}
	reqParam.authKey = pThreadInfo->authKey;
#endif

	while(TRUE)
	{
		// TODO: shbaek: GET FUNCTION's REPORT DATA
		for(i=0; i<pDeviceInfo->deviceFuncCount; i++)
		{
			int useReport = FALSE;
			Grib_DbRowDeviceFunc* pFuncInfo = NULL;

			pFuncInfo = ppFuncInfo[i];
			pFuncName = pFuncInfo->funcName;
			useReport = FUNC_ATTR_CHECK_REPORT(pFuncInfo->funcAttr);
			if(iDBG)GRIB_LOGD("# %s-RPT>: %s USE REPORT: %s[%d]\n", deviceID, pFuncName, GRIB_BOOL_TO_STR(useReport), pFuncInfo->funcAttr);

			if(useReport == FALSE)
			{//3 shbaek: Attribute Have Not Reporting Flag.
				GRIB_LOGD("# %s-RPT>: %s NEED NOT REPORTING\n", deviceID, pFuncName);
				continue;
			}

			while(TRUE)
			{
				checkStatus = THREAD_STATUS_NONE;

				pthread_mutex_lock(&pThreadInfo->threadMutex);
				checkStatus = pThreadInfo->controlThreadStatus;
				pthread_mutex_unlock(&pThreadInfo->threadMutex);

				if(iDBG)GRIB_LOGD("# %s-RPT>: BLE CHECK STATUS: %s[%d]\n", deviceID, Grib_ThreadStatusToStr(checkStatus), checkStatus);

				if(checkStatus == THREAD_STATUS_USE_BLE)
				{//shbaek: Wait for Control Thread.
					SLEEP(1);
					continue;
				}
				else
				{//shbaek: It's My Turn.
					pthread_mutex_lock(&pThreadInfo->threadMutex);
					pThreadInfo->reportThreadStatus = THREAD_STATUS_USE_BLE;
					pthread_mutex_unlock(&pThreadInfo->threadMutex);
					break;
				}
			}//shbaek: while(TRUE)

			STRINIT(bleSendBuff, sizeof(bleSendBuff));
			STRINIT(bleRecvBuff, sizeof(bleRecvBuff));

			if(iTimeCheck)
			{
				sysTimer = time(NULL);
				sysTime  = localtime(&sysTimer);
				GRIB_LOGD("# %s-RPT>: SEND TIME: %02d:%02d:%02d\n", deviceID, sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
			}

			//2 shbaek: WAIT FOR BLE RE-USE
			if(1 < pDeviceInfo->deviceFuncCount)SLEEP(GRIB_WAIT_BLE_REUSE_TIME);

			iRes = Grib_BleGetFuncData(pDeviceInfo->deviceAddr, deviceID, pFuncName, bleRecvBuff);

			pthread_mutex_lock(&pThreadInfo->threadMutex);
			pThreadInfo->reportThreadStatus = THREAD_STATUS_NONE;
			pthread_mutex_unlock(&pThreadInfo->threadMutex);

			if(iTimeCheck)
			{
				sysTimer = time(NULL);
				sysTime  = localtime(&sysTimer);
				GRIB_LOGD("# %s-RPT>: RECV TIME: %02d:%02d:%02d\n", deviceID, sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
			}

			if( (iRes!=GRIB_DONE) || (STRLEN(bleRecvBuff)==0) )
			{
				pThreadInfo->bleContinualError++;
				GRIB_LOGD("# %s-RPT>: %c[1;31m%s GET STATUS FAIL [CONTINUL: %d]%c[0m\n\n", 
					deviceID, 27, pFuncName, pThreadInfo->bleContinualError, 27);
				continue;
			}

			pRes = Grib_Split(bleRecvBuff, GRIB_LN, 0);
			if( (pRes == NULL) || (STRNCMP(pRes, GRIB_STR_ERROR, STRLEN(GRIB_STR_ERROR))==0) )
			{
				GRIB_LOGD("# %s-RPT>: %c[1;31m%s STATUS PARSING ERROR !!!%c[0m\n", deviceID, 27, pFuncName, 27);
				continue;
			}
			//if(iDBG)GRIB_LOGD("# %s-RPT>: %s GET STATUS: %s\n", deviceID, pFuncName, pRes);

			STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
			SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s/%s/%s", deviceID, pFuncName, ONEM2M_URI_CONTENT_STATUS);

			STRINIT(&reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON));
			STRNCPY(reqParam.xM2M_CON, pRes, STRLEN(pRes));

			pThreadInfo->bleContinualError = 0; //shbaek: Init Ble Continueal Error ...

#if (TEST_USE_ONLY_BLE)
			iRes = GRIB_DONE;
#else
			iRes = Grib_ContentInstanceCreate(&reqParam, &resParam);
#endif
			if(iRes == GRIB_DONE)GRIB_LOGD("# %s-RPT>: %c[1;32m%s REPORT DONE ...%c[0m\n", deviceID, 27, pFuncName, 27);
#ifdef FEATURE_CAS
			else if( (resParam.http_ResNum == HTTP_STATUS_CODE_UNAUTHORIZED)
				|| (resParam.http_ResNum == HTTP_STATUS_CODE_INTERNAL_SERVER_ERROR) )
			{//shbaek: Change Auth Key
				GRIB_LOGD("# %s-RPT>: %c[1;32mCHANGE AUTH KEY !!!%c[0m\n", deviceID, 27, 27);
				iRes = Grib_CasGetAuthKey(deviceID, pThreadInfo->authKey);
				if(iRes==GRIB_DONE)reqParam.authKey = pThreadInfo->authKey;
				SLEEP(1);
			}
#endif
			else
			{
				GRIB_LOGD("# %s-RPT>: %c[1;31m%s REPORT ERROR !!!%c[0m\n", deviceID, 27, pFuncName, 27);

			}
			GRIB_LOGD("\n");

			pthread_mutex_lock(&pThreadInfo->threadMutex);
			pThreadInfo->watchdogStatus = WATCHDOG_STATUS_DONE;
			pthread_mutex_unlock(&pThreadInfo->threadMutex);
		}

		for(i=0; i<iCycleTime; i++)
		{
			checkStatus = THREAD_STATUS_NONE;

			//shbaek: Check for Control Thread Status
			pthread_mutex_lock(&pThreadInfo->threadMutex);
			checkStatus = pThreadInfo->controlThreadStatus;
			pthread_mutex_unlock(&pThreadInfo->threadMutex);

			//if(iDBG)GRIB_LOGD("# %s-RPT>: DELAY CHECK STATUS: %s[%d]\n", deviceID, Grib_ThreadStatusToStr(checkStatus), checkStatus);

			if(checkStatus == THREAD_STATUS_NEED_ANSWER)
			{//shbaek: Answer Me Now
				GRIB_LOGD("# %s-RPT>: ANSWER ME NOW PLEASE ...\n", deviceID);

				//shbaek: Init Answer Flag
				pthread_mutex_lock(&pThreadInfo->threadMutex);
				pThreadInfo->controlThreadStatus = THREAD_STATUS_NONE;
				pthread_mutex_unlock(&pThreadInfo->threadMutex);				
				break;
			}
			else
			{//shbaek: Wait For Next Report Time
				SLEEP(1);
			}
		}
		//shbaek: Wait For Next Report Time

	}

	return pThreadInfo;
}

void* Grib_ControlThread(void* threadArg)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = gDebugThread;
	int iTry = 0;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	Grib_DeviceThreadInfo* pThreadInfo;
	Grib_DbRowDeviceInfo* pDeviceInfo;
	Grib_DbRowDeviceFunc** ppFuncInfo;

	char* deviceID = NULL;
	char* cmdConValue = NULL;

	char cmdFuncName[GRIB_MAX_SIZE_MIDDLE]  = {'\0', };
	char bleSendBuff[BLE_MAX_SIZE_SEND_MSG] = {'\0', };
	char bleRecvBuff[BLE_MAX_SIZE_RECV_MSG] = {'\0', };;

	int iTimeCheck = FALSE;
	time_t sysTimer;
	struct tm *sysTime;

	if(threadArg == NULL)
	{
		GRIB_LOGD("%ld THREAD INFO IS NULL", pthread_self());
		return NULL;
	}

	//shbaek: Pre-Pare
	pThreadInfo	= (Grib_DeviceThreadInfo *)threadArg;
	pDeviceInfo	= pThreadInfo->pRowDeviceInfo;
	ppFuncInfo	= pDeviceInfo->ppRowDeviceFunc;
	deviceID	= pDeviceInfo->deviceID;

	pThreadInfo->controlThreadStatus = THREAD_STATUS_NONE;

#if (TEST_USE_ONLY_BLE)
	GRIB_LOGD("# %s-CTR<: %c[1;33mTEST_USE_ONLY_BLE !!!%c[0m\n", deviceID, 27, 27);
	return threadArg;
#endif

	GRIB_LOGD("# %s-CTR<: CONTROL THREAD START\n", deviceID);

	MEMSET(&reqParam, GRIB_INIT, sizeof(OneM2M_ReqParam));
	MEMSET(&resParam, GRIB_INIT, sizeof(OneM2M_ResParam));

	STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	SNPRINTF(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin), "GRIB/%s", gHubID);

	STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
	STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));		

#ifdef FEATURE_CAS
	if(STRLEN(pThreadInfo->authKey)<=1)
	{
		GRIB_LOGD("# %s-RPT>: %c[1;31m%sAUTH KEY IN-VALID ERROR !!!%c[0m\n\n", deviceID, 27, 27);
		//return threadArg;
	}
	reqParam.authKey = pThreadInfo->authKey;
#endif

	while(TRUE)
	{
/*
		pthread_mutex_lock(&pThreadInfo->threadMutex);
		pThreadInfo->controlThreadStatus = THREAD_STATUS_POLLING;
		pthread_mutex_unlock(&pThreadInfo->threadMutex);
*/
//		GRIB_LOGD("# %s-CTR<: POLLING START\n", deviceID);

		GRIB_LOGD("\n");
		iRes = Grib_LongPolling(&reqParam, &resParam);
		if(iRes != GRIB_DONE)
		{
			if(resParam.http_ResNum == HTTP_STATUS_CODE_REQUEST_TIME_OUT)
			{
				GRIB_LOGD("# %s-CTR<: POLLING TIME OUT ...\n", deviceID);
			}
			else
			{//shbaek: General Error
				GRIB_LOGD("# %s-CTR<: POLLING ERROR MSG: %s[%d]\n", deviceID, resParam.http_ResMsg, resParam.http_ResNum);
				SLEEP(1);
			}

			continue;
		}

		//3 shbaek: Check Expire Time
		if(Grib_isAvailableExpireTime(resParam.xM2M_ExpireTime) == FALSE)
		{
			GRIB_LOGD("# %s-CTR<: COMMAND TIME IS NOT AVAILABLE: %s\n", deviceID, resParam.xM2M_ExpireTime);
			continue;
		}

		GRIB_LOGD("# %s-CTR<: COMMAND START\n", deviceID);

		cmdConValue = NULL;
		STRINIT(cmdFuncName, sizeof(cmdFuncName));

		//3 shbaek: Find Function Name
		//iRes = Grib_FindFuncName(resParam.xM2M_PrntID, cmdFuncName);
		iRes = Grib_MatchFuncName(resParam.xM2M_PrntID, cmdFuncName);

#if __NOT_USED__
		for(i=0; i<pDeviceInfo->deviceFuncCount; i++)
		{
			if(STRNCMP(ppFuncInfo[i]->exRsrcID, resParam.xM2M_PrntID, STRLEN(resParam.xM2M_PrntID))==0)
			{//shbaek: Matching Function's Execute Resource ID
				cmdFuncName = ppFuncInfo[i]->funcName;
			}
		}
#endif

		if(STRLEN(cmdFuncName)<=0)
		{
			GRIB_LOGD("# %s-CTR<: IN-VALID Ex RESOURCE ID: %s\n", deviceID, resParam.xM2M_PrntID);
			continue;
		}

		cmdConValue = resParam.xM2M_Content;
		if(iDBG)
		{
			GRIB_LOGD("# %s-CTR<: COMMAND PID : %s\n", deviceID, resParam.xM2M_PrntID);
			GRIB_LOGD("# %s-CTR<: COMMAND RID : %s\n", deviceID, resParam.xM2M_RsrcID);
			GRIB_LOGD("# %s-CTR<: COMMAND CON : %s\n", deviceID, resParam.xM2M_Content);
		}

		while(TRUE)
		{
			int checkStatus = THREAD_STATUS_NONE;

			pthread_mutex_lock(&pThreadInfo->threadMutex);
			checkStatus = pThreadInfo->reportThreadStatus;
			pthread_mutex_unlock(&pThreadInfo->threadMutex);

			if(iDBG)GRIB_LOGD("# %s-CTR<: BLE CHECK STATUS: %s[%d]\n", deviceID, Grib_ThreadStatusToStr(checkStatus), checkStatus);

			if(checkStatus == THREAD_STATUS_USE_BLE)
			{//shbaek: Wait for Report Thread.
				SLEEP(1);
				continue;
			}
			else
			{//shbaek: It's My Turn.
				pthread_mutex_lock(&pThreadInfo->threadMutex);
				pThreadInfo->controlThreadStatus = THREAD_STATUS_USE_BLE;
				pthread_mutex_unlock(&pThreadInfo->threadMutex);
				break;
			}
		}

SEND_BLE:
		// TODO: shbaek: SEND COMMAND USE BLE
		if(iDBG)GRIB_LOGD("# %s-CTR<: SEND COMMAND USE BLE\n", deviceID);

		STRINIT(bleSendBuff, sizeof(bleSendBuff));
		STRINIT(bleRecvBuff, sizeof(bleRecvBuff));

		if(iTimeCheck)
		{
			sysTimer = time(NULL);
			sysTime  = localtime(&sysTimer);
			GRIB_LOGD("# %s-CTR<: SEND TIME: %02d:%02d:%02d\n", deviceID, sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
		}

		//2 shbaek: WAIT FOR BLE RE-USE
		SLEEP(GRIB_WAIT_BLE_REUSE_TIME);

		if(iDBG)GRIB_LOGD("# %s-CTR<: COMMAND FUNC: %s\n", deviceID, cmdFuncName);
		if(iDBG)GRIB_LOGD("# %s-CTR<: COMMAND CON : %s\n", deviceID, cmdConValue);

		iRes = Grib_BleSetFuncData(pDeviceInfo->deviceAddr, deviceID, cmdFuncName, cmdConValue, bleRecvBuff);

		if(iTimeCheck)
		{
			sysTimer = time(NULL);
			sysTime  = localtime(&sysTimer);
			GRIB_LOGD("# %s-CTR<: RECV TIME: %02d:%02d:%02d\n", deviceID, sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
		}

		if(iRes == GRIB_DONE)
		{
			char* pRes = NULL;

			pRes = Grib_Split(bleRecvBuff, GRIB_LN, 0);
			if( (pRes == NULL) || (STRNCMP(pRes, BLE_RESPONSE_STR_ERROR, STRLEN(BLE_RESPONSE_STR_ERROR))==0) )
			{//3 shbaek: RECV BLE ERROR MSG
				GRIB_LOGD("# %s-CTR<: RES ERROR RE-TRY CHANCE: %d\n", deviceID, iTry+1);
				if(iTry < 3)
				{
					iTry++;
					SLEEP(GRIB_CONTROL_FAIL_WAIT_TIME_SEC);
					goto SEND_BLE;
				}
				else
				{
					GRIB_LOGD("# %s-CTR<: ##### ##### ##### ##### ##### ##### #####\n", deviceID);
					GRIB_LOGD("# %s-CTR<: #####       CRITICAL ERROR          #####\n", deviceID);
					GRIB_LOGD("# %s-CTR<: ##### ##### ##### ##### ##### ##### #####\n", deviceID);
				}
			}

			GRIB_LOGD("# %s-CTR<: %s SET %s: %s\n", deviceID, cmdFuncName, cmdConValue, pRes);
		}
		else
		{//3 shbaek: BLE LOCAL ERROR
			GRIB_LOGD("# %s-CTR<: BLE ERROR RE-TRY CHANCE: %d\n", deviceID, iTry+1);
			if(iTry < 3)
			{
				iTry++;
				SLEEP(GRIB_CONTROL_FAIL_WAIT_TIME_SEC);
				goto SEND_BLE;
			}
			else
			{
				GRIB_LOGD("# %s-CTR<: ##### ##### ##### ##### ##### ##### #####\n", deviceID);
				GRIB_LOGD("# %s-CTR<: #####       CRITICAL ERROR          #####\n", deviceID);
				GRIB_LOGD("# %s-CTR<: ##### ##### ##### ##### ##### ##### #####\n", deviceID);
			}
		}

		GRIB_LOGD("# %s-CTR<: THREAD_STATUS_NEED_ANSWER !!!\n", deviceID);

		pthread_mutex_lock(&pThreadInfo->threadMutex);
		pThreadInfo->controlThreadStatus = THREAD_STATUS_NEED_ANSWER;
		pthread_mutex_unlock(&pThreadInfo->threadMutex);

		iTry = 0;

	}

	return pThreadInfo;
}

int Grib_ThreadStart(void)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int DEVICE_MAX_COUNT = 0;
	const int MAX_LOOP_CACHE_RI = INT_MAX;

	Grib_DbRowDeviceInfo*  pRowDeviceInfo = NULL;

	const char* FUNC_TAG = "# BLE-HUB-START:";

	//shbaek: Pre-Pare
	iRes = Grib_DbCreate();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s NEED NOT DB CREATE\n", FUNC_TAG);
		Grib_DbClose();
	}

	gDevInfoAll = (Grib_DbAll *)MALLOC(sizeof(Grib_DbAll));
	MEMSET(gDevInfoAll, GRIB_INIT, sizeof(Grib_DbAll));

	iRes = Grib_DbToMemory(gDevInfoAll);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("%s GET DATABASE ERROR\n", FUNC_TAG);
		goto FINAL;
	}

	DEVICE_MAX_COUNT = gDevInfoAll->deviceCount;
	GRIB_LOGD("%s DEVICE ALL COUNT: %d\n\n", FUNC_TAG, DEVICE_MAX_COUNT);

	if(DEVICE_MAX_COUNT == 0)
	{
		GRIB_LOGD("# ########## ########## ########## ##########\n");
		GRIB_LOGD("%s NO REGISTERED DEVICES\n", FUNC_TAG);
		GRIB_LOGD("%s REGISTE YOUR DEVICE ... \n", FUNC_TAG);
		GRIB_LOGD("# ########## ########## ########## ##########\n");
//		goto FINAL;
	}

	//3 shbaek: Hub Thread Init & Create
	GRIB_LOGD("%s HUB THREAD SETTING\n", FUNC_TAG);
	gHubThread = (Grib_HubThreadInfo*)MALLOC(sizeof(Grib_HubThreadInfo));
	MEMSET(gHubThread, 0x00, sizeof(Grib_HubThreadInfo));
	pthread_attr_init(&gHubThread->threadAttr);
	pthread_cond_init(&gHubThread->threadCond, GRIB_NOT_USED);
	pthread_mutex_init(&gHubThread->threadMutex, GRIB_NOT_USED);
	pthread_create(&gHubThread->threadID, &gHubThread->threadAttr, Grib_HubThread, (void *)gHubThread);
	SLEEP(1);

#if __NOT_USED__
	//3 shbaek: Reset Thread Init & Create
	GRIB_LOGD("%s RE-SET THREAD SETTING\n", FUNC_TAG);
	gResetThread = (Grib_HubThreadInfo*)MALLOC(sizeof(Grib_HubThreadInfo));
	MEMSET(gResetThread, 0x00, sizeof(Grib_HubThreadInfo));
	pthread_attr_init(&gResetThread->threadAttr);
	pthread_cond_init(&gResetThread->threadCond, GRIB_NOT_USED);
	pthread_mutex_init(&gResetThread->threadMutex, GRIB_NOT_USED);
	pthread_create(&gResetThread->threadID, &gResetThread->threadAttr, Grib_ResetThread, (void *)gResetThread);
	SLEEP(1);
#endif

	GRIB_LOGD("%s DEVICE INFO SETTING\n\n", FUNC_TAG);
	gBleThreadList = (Grib_DeviceThreadInfo**)MALLOC(sizeof(Grib_DeviceThreadInfo*)*DEVICE_MAX_COUNT);
	for(i=0; i<DEVICE_MAX_COUNT; i++)
	{
		gBleThreadList[i] = (Grib_DeviceThreadInfo*)MALLOC(sizeof(Grib_DeviceThreadInfo));
		MEMSET(gBleThreadList[i], GRIB_INIT, sizeof(Grib_DeviceThreadInfo));

		pRowDeviceInfo = gBleThreadList[i]->pRowDeviceInfo = gDevInfoAll->ppRowDeviceInfo[i];

		GRIB_LOGD("%s DEVICE[%d/%d] ID:%s ADDR:%s\n", FUNC_TAG, i, DEVICE_MAX_COUNT-1,
			gBleThreadList[i]->pRowDeviceInfo->deviceID, gBleThreadList[i]->pRowDeviceInfo->deviceAddr);

#ifdef FEATURE_CAS
		iRes = Grib_CasGetAuthKey(gBleThreadList[i]->pRowDeviceInfo->deviceID, gBleThreadList[i]->authKey);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("%s DEVICE[%d/%d] ID:%s GET AUTH KEY ERROR !!!\n", FUNC_TAG, i, DEVICE_MAX_COUNT-1,
				gBleThreadList[i]->pRowDeviceInfo->deviceID);
		}
		else
		{
			GRIB_LOGD("%s DEVICE[%d/%d] ID:%s AUTH:%s\n", FUNC_TAG, i, DEVICE_MAX_COUNT-1,
				gBleThreadList[i]->pRowDeviceInfo->deviceID, gBleThreadList[i]->authKey);
		}
		GRIB_LOGD("\n");
#endif

	}

	//2 shbaek: Get One M2M Resource ID
	for(i=0; i<MAX_LOOP_CACHE_RI; i++)
	{
		iRes = Grib_MakeCacheRi(gDevInfoAll, gBleThreadList);
		SLEEP(1);

		if(iRes != GRIB_DONE)
		{//shbaek: Make Cache Table Fail !!!
			GRIB_LOGD("%s %c[1;31mMAKE CACHE TABLE FAIL COUNT: %d%c[0m\n\n", FUNC_TAG, 27, i+1, 27);
			continue;
		}
		else
		{//shbaek: Make Cache Table Done ...
			break;
		}
	}

	GRIB_LOGD("%s ATTR/COND/MUTEX INIT\n", FUNC_TAG);
	//shbaek: Init
	for(i=0; i<DEVICE_MAX_COUNT; i++)
	{
		pthread_attr_init(&gBleThreadList[i]->threadAttr);
		pthread_cond_init(&gBleThreadList[i]->threadCond, GRIB_NOT_USED);
		pthread_mutex_init(&gBleThreadList[i]->threadMutex, GRIB_NOT_USED);		
	}

	//shbaek: Create
	for(i=0; i<DEVICE_MAX_COUNT; i++)
	{
		int   iAttr = GRIB_INIT;
		char *strDeviceID = gBleThreadList[i]->pRowDeviceInfo->deviceID;

		iAttr = Grib_DeviceCheckAttr(gBleThreadList[i]->pRowDeviceInfo);

		if(FUNC_ATTR_CHECK_CONTROL(iAttr)==TRUE)
		{
			GRIB_LOGD("%s %s CONTROL THREAD CREATE\n", FUNC_TAG, strDeviceID);
			pthread_create(&gBleThreadList[i]->controlThreadID, NULL, Grib_ControlThread, (void *)gBleThreadList[i]);
			SLEEP(1);
		}
		else
		{
			GRIB_LOGD("%s %s CONTROL THREAD SKIP !!!\n", FUNC_TAG, strDeviceID);
		}

		if(FUNC_ATTR_CHECK_REPORT(iAttr)==TRUE)
		{
			GRIB_LOGD("%s %s REPORT THREAD CREATE\n", FUNC_TAG, strDeviceID);
			pthread_create(&gBleThreadList[i]->reportThreadID, NULL, Grib_ReportThread, (void *)gBleThreadList[i]);
			SLEEP(1);
		}
		else
		{
			GRIB_LOGD("%s %s REPORT THREAD SKIP !!!\n", FUNC_TAG, strDeviceID);
		}

	}
	Grib_SystemThreadStart(); //shbaek: System Thread Create ...
	

	GRIB_LOGD("%s THREAD WAIT\n", FUNC_TAG);
	//shbaek: Wait
	for(i=0; i<DEVICE_MAX_COUNT; i++)
	{
		char *strDeviceID = gBleThreadList[i]->pRowDeviceInfo->deviceID;

		if(0 < gBleThreadList[i]->controlThreadID)
		{
			//GRIB_LOGD("%s %s CONTROL THREAD[%d/%d] WAIT\n", FUNC_TAG, strDeviceID, i, iDeviceMaxCount);
			pthread_join(gBleThreadList[i]->controlThreadID, (void **)&gBleThreadList[i]);
		}
		else
		{
			GRIB_LOGD("%s %s CONTROL THREAD NEED NOT WAIT !!!\n", FUNC_TAG, strDeviceID);
		}

		if(0 < gBleThreadList[i]->reportThreadID)
		{
			//GRIB_LOGD("%s %s REPORT THREAD[%d/%d] WAIT\n", FUNC_TAG, strDeviceID, i, iDeviceMaxCount);
			pthread_join(gBleThreadList[i]->reportThreadID, (void **)&gBleThreadList[i]);
		}
		else
		{
			GRIB_LOGD("%s %s REPORT THREAD NEED NOT WAIT !!!\n", FUNC_TAG, strDeviceID);
		}

	}

	//3 shbaek: Other Thread Wait
	pthread_join(gHubThread->threadID, (void **)&gHubThread);
	pthread_join(gWatchDogThread->threadID, (void **)&gWatchDogThread);
#if __NOT_USED__
	pthread_join(gResetThread->threadID, (void **)&gResetThread);
#endif

FINAL:
	GRIB_LOGD("%s THREAD FINAL\n", FUNC_TAG);
	GRIB_LOGD("%s ########## ########## ########## ########## ########## ########## ########## ##########\n", FUNC_TAG);
	if(gBleThreadList != NULL)
	{
		for(i=0; i<DEVICE_MAX_COUNT; i++)
		{
			pthread_mutex_destroy(&gBleThreadList[i]->threadMutex);
			FREE(gBleThreadList[i]);
		}
		FREE(gBleThreadList);
		gBleThreadList = NULL;
	}

	if(gDevInfoAll != NULL)
	{
		for(i=0; i<DEVICE_MAX_COUNT; i++)
		{
			pRowDeviceInfo = gDevInfoAll->ppRowDeviceInfo[i];
			Grib_DbFreeRowFunc(pRowDeviceInfo->ppRowDeviceFunc, pRowDeviceInfo->deviceFuncCount);
			if(pRowDeviceInfo!=NULL)FREE(pRowDeviceInfo);
		}
		if(gDevInfoAll->ppRowDeviceInfo!=NULL)FREE(gDevInfoAll->ppRowDeviceInfo);
		FREE(gDevInfoAll);
		gDevInfoAll = NULL;
	}
	Grib_DbClose();

	return iRes;
}


#define __ONEM2M_ETC_FUNC__

