/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "include/grib_db.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

MYSQL*		gSqlConnect;
MYSQL_RES*	gSqlResult;
MYSQL_ROW   gSqlRow;

char gSqlHost[GRIB_MAX_SIZE_SHORT+1];
char gSqlUser[GRIB_MAX_SIZE_SHORT+1];
char gSqlPswd[GRIB_MAX_SIZE_SHORT+1];
int  gSqlPort;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#define __GRIB_DB_BASIC__
int Grib_DbConfig(void)
{
	int iRes = GRIB_ERROR;
	Grib_ConfigInfo pConfigInfo;

	MEMSET(&pConfigInfo, 0x00, sizeof(Grib_ConfigInfo));

	iRes = Grib_LoadConfig(&pConfigInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("LOAD CONFIG ERROR !!!\n");
		return iRes;
	}

	STRINIT(gSqlHost, sizeof(gSqlHost));
	STRNCPY(gSqlHost, pConfigInfo.iotDbHost, STRLEN(pConfigInfo.iotDbHost));

	STRINIT(gSqlUser, sizeof(gSqlUser));
	STRNCPY(gSqlUser, pConfigInfo.iotDbUser, STRLEN(pConfigInfo.iotDbUser));

	STRINIT(gSqlPswd, sizeof(gSqlPswd));
	STRNCPY(gSqlPswd, pConfigInfo.iotDbPswd, STRLEN(pConfigInfo.iotDbPswd));

	gSqlPort = pConfigInfo.iotDbPort;

	GRIB_LOGD("# MY-SQL CONFIG HOST: %s\n", gSqlHost);
	GRIB_LOGD("# MY-SQL CONFIG PORT: %d\n", gSqlPort);
	GRIB_LOGD("# MY-SQL CONFIG USER: %s\n", gSqlUser);
	GRIB_LOGD("# MY-SQL CONFIG PSWD: %s\n", gSqlPswd);

	return GRIB_SUCCESS;
}

int Grib_DbClose(void)
{
	if(gSqlConnect == NULL)
	{
		return GRIB_ERROR;
	}

	mysql_close(gSqlConnect);

	STRINIT(gSqlHost, sizeof(gSqlHost));
	STRINIT(gSqlUser, sizeof(gSqlUser));
	STRINIT(gSqlPswd, sizeof(gSqlPswd));
	gSqlPort    = NULL;
	gSqlConnect = NULL;

	GRIB_LOGD("%s CLOSE DONE\n", LOG_TAG_DB);

	return GRIB_DONE;
}
int Grib_DbOpen(void)
{
	if(gSqlConnect != NULL)
	{
		Grib_DbClose();
	}

	Grib_DbConfig();

	gSqlConnect = mysql_init(GRIB_NOT_USED);

	GRIB_LOGD("%s MYSQL VERSION: %s\n", LOG_TAG_DB, mysql_get_client_info());

	gSqlConnect = mysql_real_connect(gSqlConnect, gSqlHost, 
							gSqlUser, gSqlPswd,
							MYSQL_DB_NAME, gSqlPort,
							(char *)NULL, GRIB_NOT_USED);

	if(gSqlConnect == NULL)
	{//shbaek: Connection Fail
		GRIB_LOGD("%s MYSQL CONNECT FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s MYSQL CONNECTED\n", LOG_TAG_DB);

	return GRIB_DONE;
}

int Grib_DbCreate(void)
{
	int iRes = GRIB_ERROR;

	Grib_DbConfig();

	gSqlConnect = mysql_init(GRIB_NOT_USED);

	GRIB_LOGD("%s MYSQL VERSION: %s\n", LOG_TAG_DB, mysql_get_client_info());

	//shbaek: Connect
	gSqlConnect = mysql_real_connect(gSqlConnect, gSqlHost, 
							gSqlUser, gSqlPswd,
							GRIB_NOT_USED, gSqlPort,
							(char *)NULL, GRIB_NOT_USED);
	if(gSqlConnect == NULL)
	{//shbaek: Connection Fail
		GRIB_LOGD("%s OPEN FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s MYSQL CONNECTED\n", LOG_TAG_DB);

	//shbaek: Create DB
	iRes = mysql_query(gSqlConnect, MYSQL_QUERY_DB_CREATE);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE DB FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE DB DONE\n", LOG_TAG_DB);

	//shbaek: Choose DB
	iRes = mysql_query(gSqlConnect, MYSQL_QUERY_DB_USE);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s USE DB FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s USE DB DONE\n", LOG_TAG_DB);

	//shbaek: Create Device Info Table
	iRes = mysql_query(gSqlConnect, MYSQL_QUERY_CREATE_DEVICE_INFO);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE DEVICE TABLE FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE DEVICE TABLE DONE\n", LOG_TAG_DB);

	//shbaek: Create Device func Table
	iRes = mysql_query(gSqlConnect, MYSQL_QUERY_CREATE_DEVICE_FUNC);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE FUNC TABLE FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE FUNC TABLE DONE\n", LOG_TAG_DB);

	return iRes;
}

int Grib_DbDrop(void)
{
	int iRes = GRIB_ERROR;

	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	iRes = mysql_query(gSqlConnect, MYSQL_QUERY_DB_DROP);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s DROP DB FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s DROP DB DONE\n", LOG_TAG_DB);

	return iRes;
}

#define __GRIB_DB_DEVICE_INFO__

int Grib_DbSetDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo)
{
	int iRes = GRIB_ERROR;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	GRIB_LOGD("%s SET DEVICE INFO\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, MYSQL_QUERY_INSERT_DEVICE_INFO, 
							pRowDeviceInfo->deviceID, pRowDeviceInfo->deviceInterface, 
							pRowDeviceInfo->deviceAddr, pRowDeviceInfo->deviceFuncCount,
							pRowDeviceInfo->deviceLoc, pRowDeviceInfo->deviceDesc,
							pRowDeviceInfo->reportCycle);

	GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s SET DEVICE INFO DONE\n", LOG_TAG_DB);

	return iRes;
}

int Grib_DbGetDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo)
{
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(iDBG)GRIB_LOGD("%s GET DEVICE INFO\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, MYSQL_QUERY_SELECT_DEVICE_INFO, pRowDeviceInfo->deviceID);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s NO MATCHING DEVICE ID: %s\n", LOG_TAG_DB, pRowDeviceInfo->deviceID);
		iRes = GRIB_ERROR;
		goto FINAL;
	}

	gSqlRow = NULL;
	gSqlRow = mysql_fetch_row(gSqlResult);
	if(gSqlRow == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s NO MATCHING DEVICE ID: %s\n", LOG_TAG_DB, pRowDeviceInfo->deviceID);
		iRes = GRIB_ERROR;
		goto FINAL;
	}

	//shbaek: Copy to Memory
	//STRNCPY(pRowDeviceInfo->deviceID, gSqlRow[INDEX_DEVICE_ID], STRLEN(gSqlRow[INDEX_DEVICE_ID]));
	STRNCPY(pRowDeviceInfo->deviceAddr, gSqlRow[INDEX_DEVICE_ADDR], STRLEN(gSqlRow[INDEX_DEVICE_ADDR]));
	STRNCPY(pRowDeviceInfo->deviceLoc, gSqlRow[INDEX_DEVICE_LOC], STRLEN(gSqlRow[INDEX_DEVICE_LOC]));
	STRNCPY(pRowDeviceInfo->deviceDesc, gSqlRow[INDEX_DEVICE_DESC], STRLEN(gSqlRow[INDEX_DEVICE_DESC]));
	pRowDeviceInfo->deviceInterface = (Grib_DeviceIfType)ATOI(gSqlRow[INDEX_DEVICE_INTERFACE]);
	pRowDeviceInfo->deviceFuncCount = ATOI(gSqlRow[INDEX_DEVICE_FCOUNT]);
	pRowDeviceInfo->reportCycle = ATOI(gSqlRow[INDEX_REPORT_CYCLE]);

	mysql_free_result(gSqlResult);
	if(iDBG)GRIB_LOGD("%s GET DEVICE INFO DONE\n", LOG_TAG_DB);

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return iRes;
}

int Grib_DbDelDeviceInfo(char* deviceID)
{
	int iRes = GRIB_ERROR;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	GRIB_LOGD("%s DELETE DEVICE INFO\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, MYSQL_QUERY_DELETE_DEVICE_INFO, deviceID);
	GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s DELETE DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s DELETE DEVICE INFO DONE\n", LOG_TAG_DB);

	return iRes;
}

#define __GRIB_DB_DEVICE_FUNC__
int Grib_DbFreeRowFunc(Grib_DbRowDeviceFunc** ppRowDeviceFunc, int iRowCount)
{
	int i = 0;

	if( (ppRowDeviceFunc==NULL) || (iRowCount<1) )
	{
		GRIB_LOGD("%s PARAMETER IS NULL\n", LOG_TAG_DB);
		return GRIB_ERROR;
	}

	for(i=0; i<iRowCount; i++)
	{
		if(ppRowDeviceFunc[i] != NULL)
		{
			FREE(ppRowDeviceFunc[i]);
			ppRowDeviceFunc[i] = NULL;
		}
	}

	FREE(ppRowDeviceFunc);
	return GRIB_DONE;
}

int Grib_DbSetDeviceFunc(Grib_DbRowDeviceFunc* pRowDeviceFunc)
{
	int iRes = GRIB_ERROR;
	int iDBG = TRUE;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(iDBG)GRIB_LOGD("%s SET DEVICE FUNC\n", LOG_TAG_DB);
	if( (pRowDeviceFunc==NULL) || (pRowDeviceFunc->deviceID==NULL) )
	{
		GRIB_LOGD("%s PARAMETER IS NULL\n", LOG_TAG_DB);
		return GRIB_ERROR;
	}

	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, MYSQL_QUERY_INSERT_DEVICE_FUNC, pRowDeviceFunc->deviceID, 
							pRowDeviceFunc->funcName, pRowDeviceFunc->exRsrcID, pRowDeviceFunc->funcAttr);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SET DEVICE FUNC FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("%s SET DEVICE FUNC DONE\n", LOG_TAG_DB);

	return iRes;
}

int Grib_DbGetDeviceFunc(char *deviceID, Grib_DbRowDeviceFunc** ppRowDeviceFunc, int *pRowCount)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };
	//Grib_DbRowDeviceFunc** ppRowDeviceFunc = *pppRowDeviceFunc;

	if(iDBG)GRIB_LOGD("%s GET DEVICE FUNC\n", LOG_TAG_DB);
	if(deviceID == NULL)
	{
		GRIB_LOGD("%s PARAMETER IS NULL\n", LOG_TAG_DB);
		return GRIB_ERROR;
	}

	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, MYSQL_QUERY_SELECT_DEVICE_FUNC, deviceID);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlRow = NULL;
	*pRowCount = 0;

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s NO MATCHING DEVICE ID: %s\n", LOG_TAG_DB, deviceID);
		goto FINAL;
	}

	*pRowCount = mysql_num_rows(gSqlResult);
	if(iDBG)GRIB_LOGD("%s SELECTED ROW COUNT: %d\n", LOG_TAG_DB, *pRowCount);

/*
	ppRowDeviceFunc = (Grib_DbRowDeviceFunc**)MALLOC(sizeof(Grib_DbRowDeviceFunc*)*(*pRowCount));
	GRIB_LOGD("%s pRowDeviceFunc: %p\n", LOG_TAG_DB, ppRowDeviceFunc);

	for(i=0; i<(*pRowCount); i++)
	{
		ppRowDeviceFunc[i] = (Grib_DbRowDeviceFunc*)MALLOC(sizeof(Grib_DbRowDeviceFunc));
		MEMSET(ppRowDeviceFunc[i], GRIB_INIT, sizeof(Grib_DbRowDeviceFunc));

		GRIB_LOGD("%s pRowDeviceFunc[%d]: %p\n", LOG_TAG_DB, i, ppRowDeviceFunc[i]);
	}
*/

	i = 0;
	while( (gSqlRow=mysql_fetch_row(gSqlResult)) != NULL)
	{
		//shbaek: Copy to Memory
		STRNCPY(ppRowDeviceFunc[i]->deviceID, gSqlRow[INDEX_DEVICE_ID], STRLEN(gSqlRow[INDEX_DEVICE_ID]));
		STRNCPY(ppRowDeviceFunc[i]->funcName, gSqlRow[INDEX_FUNC_NAME], STRLEN(gSqlRow[INDEX_FUNC_NAME]));
		STRNCPY(ppRowDeviceFunc[i]->exRsrcID, gSqlRow[INDEX_FUNC_EXRI], STRLEN(gSqlRow[INDEX_FUNC_EXRI]));
		ppRowDeviceFunc[i]->funcAttr = ATOI(gSqlRow[INDEX_FUNC_ATTR]);
		i++;
	}

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return iRes;
}

int Grib_DbDelDeviceFunc(char* deviceID)
{
	int iRes = GRIB_ERROR;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	GRIB_LOGD("%s DELETE DEVICE FUNC\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, MYSQL_QUERY_DELETE_DEVICE_FUNC, deviceID);
	GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s DELETE DEVICE FUNC FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s DELETE DEVICE FUNC DONE\n", LOG_TAG_DB);

	return iRes;
}



#define __GRIB_DB_UTIL__
int Grib_DbGetDeviceCount(void)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDeviceCount = -1;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	STRNCPY(sqlQuery, MYSQL_QUERY_SELECT_DEVICE_INFO_ALL, STRLEN(MYSQL_QUERY_SELECT_DEVICE_INFO_ALL));

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s DEVICE INFO TABLE IS EMPTY?\n", LOG_TAG_DB);
		goto FINAL;
	}

	iDeviceCount = mysql_num_rows(gSqlResult);
	GRIB_LOGD("%s TOTAL DEVICE COUNT: %d\n", LOG_TAG_DB, iDeviceCount);

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return iDeviceCount;
}

int Grib_DbGetDeviceInfoAll(Grib_DbRowDeviceInfo** pRowDeviceInfo)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(iDBG)GRIB_LOGD("%s GET DEVICE INFO ALL\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	STRNCPY(sqlQuery, MYSQL_QUERY_SELECT_DEVICE_INFO_ALL, STRLEN(MYSQL_QUERY_SELECT_DEVICE_INFO_ALL));
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s DEVICE INFO TABLE IS EMPTY?\n", LOG_TAG_DB);
		goto FINAL;
	}

	i = 0;
	while( (gSqlRow=mysql_fetch_row(gSqlResult)) != NULL)
	{
		//shbaek: Copy to Memory
		STRNCPY(pRowDeviceInfo[i]->deviceID, gSqlRow[INDEX_DEVICE_ID], STRLEN(gSqlRow[INDEX_DEVICE_ID]));
		STRNCPY(pRowDeviceInfo[i]->deviceAddr, gSqlRow[INDEX_DEVICE_ADDR], STRLEN(gSqlRow[INDEX_DEVICE_ADDR]));
		STRNCPY(pRowDeviceInfo[i]->deviceLoc, gSqlRow[INDEX_DEVICE_LOC], STRLEN(gSqlRow[INDEX_DEVICE_LOC]));
		STRNCPY(pRowDeviceInfo[i]->deviceDesc, gSqlRow[INDEX_DEVICE_DESC], STRLEN(gSqlRow[INDEX_DEVICE_DESC]));
		pRowDeviceInfo[i]->deviceInterface = (Grib_DeviceIfType)ATOI(gSqlRow[INDEX_DEVICE_INTERFACE]);
		pRowDeviceInfo[i]->deviceFuncCount = ATOI(gSqlRow[INDEX_DEVICE_FCOUNT]);
		pRowDeviceInfo[i]->reportCycle = ATOI(gSqlRow[INDEX_REPORT_CYCLE]);
		i++;
	}

	if(iDBG)GRIB_LOGD("%s GET DEVICE INFO ALL DONE\n", LOG_TAG_DB);

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return iRes;
}

int Grib_DbToMemory(Grib_DbAll *pDbAll)
{
	int i = 0;
	int x = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	
	int iFuncCount = 0;
	int iCheckCount = 0;
	Grib_DbRowDeviceInfo*  pRowDeviceInfo;
	Grib_DbRowDeviceFunc*  pRowDeviceFunc;

	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	if(iDBG)
	{
		GRIB_LOGD("%s---------- ---------- ---------- ---------- ---------- ---------- ----------\n", LOG_TAG_DB);
		GRIB_LOGD("%s---------- ----------        DATABASE TO MEMORY        ---------- ----------\n", LOG_TAG_DB);
		GRIB_LOGD("%s---------- ---------- ---------- ---------- ---------- ---------- ----------\n", LOG_TAG_DB);
	}

	//shbaek: Get Device Count
	pDbAll->deviceCount = Grib_DbGetDeviceCount();

	pDbAll->ppRowDeviceInfo = (Grib_DbRowDeviceInfo**)MALLOC(sizeof(Grib_DbRowDeviceInfo*)*pDbAll->deviceCount);
	for(i=0; i<pDbAll->deviceCount; i++)
	{
		
		pDbAll->ppRowDeviceInfo[i] = (Grib_DbRowDeviceInfo*)MALLOC(sizeof(Grib_DbRowDeviceInfo));
		MEMSET(pDbAll->ppRowDeviceInfo[i], GRIB_INIT, sizeof(Grib_DbRowDeviceInfo));
	}

	iRes = Grib_DbGetDeviceInfoAll(pDbAll->ppRowDeviceInfo);

	//shbaek: LOG
	for(i=0; i<pDbAll->deviceCount; i++)
	{
		pRowDeviceInfo = pDbAll->ppRowDeviceInfo[i];

		if(iDBG)
		{
			GRIB_LOGD("%s---------- ---------- ---------- ---------- ---------- ---------- ----------\n", LOG_TAG_DB);
			GRIB_LOGD("%s GET DEVICE ID    : %s\n", LOG_TAG_DB, pRowDeviceInfo->deviceID);
			GRIB_LOGD("%s GET DEVICE ADDR  : %s\n", LOG_TAG_DB, pRowDeviceInfo->deviceAddr);
			GRIB_LOGD("%s GET DEVICE FCOUNT: %d\n", LOG_TAG_DB, pRowDeviceInfo->deviceFuncCount);
			GRIB_LOGD("%s GET DEVICE LOC   : %s\n", LOG_TAG_DB, pRowDeviceInfo->deviceLoc);
			GRIB_LOGD("%s GET DEVICE DESC  : %s\n", LOG_TAG_DB, pRowDeviceInfo->deviceDesc);
			GRIB_LOGD("%s---------- ---------- ---------- ---------- ---------- ---------- ----------\n", LOG_TAG_DB);
		}
	}

	for(i=0; i<pDbAll->deviceCount; i++)
	{
		pRowDeviceInfo = pDbAll->ppRowDeviceInfo[i];
		iFuncCount = pRowDeviceInfo->deviceFuncCount;

		//shbaek: Alloc Func Memory 
		pRowDeviceInfo->ppRowDeviceFunc = (Grib_DbRowDeviceFunc**)MALLOC(sizeof(Grib_DbRowDeviceFunc*)*iFuncCount);
		for(x=0; x<iFuncCount; x++)
		{
			pRowDeviceInfo->ppRowDeviceFunc[x] = (Grib_DbRowDeviceFunc*)MALLOC(sizeof(Grib_DbRowDeviceFunc));
			MEMSET(pRowDeviceInfo->ppRowDeviceFunc[x], GRIB_INIT, sizeof(Grib_DbRowDeviceFunc));
		}

		iRes = Grib_DbGetDeviceFunc(pRowDeviceInfo->deviceID, pRowDeviceInfo->ppRowDeviceFunc, &iCheckCount);

		if(iFuncCount != iCheckCount)
		{
			GRIB_LOGD("%s # ########## ########## ########## ########## ########## ##########\n", LOG_TAG_DB);
			GRIB_LOGD("%s # COUNT NOT MATCHING !!!\n", LOG_TAG_DB);
			GRIB_LOGD("%s # INFO TABLE's FUNC COUNT: %d\n", LOG_TAG_DB, iFuncCount);
			GRIB_LOGD("%s # FUNC TABLE's REAL COUNT: %d\n", LOG_TAG_DB, iCheckCount);
			GRIB_LOGD("%s # ########## ########## ########## ########## ########## ##########\n", LOG_TAG_DB);
		}

		if(iDBG)GRIB_LOGD("%s---------- ---------- ---------- ---------- ---------- ---------- ----------\n", LOG_TAG_DB);
		for(x=0; x<iFuncCount; x++)
		{
			pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[x];
			if(iDBG)
			{
				GRIB_LOGD("%s GET DEVICE ID    : %s\n", LOG_TAG_DB, pRowDeviceFunc->deviceID);
				GRIB_LOGD("%s GET FUNC NAME    : %s\n", LOG_TAG_DB, pRowDeviceFunc->funcName);
				GRIB_LOGD("%s GET EXE RSRC ID  : %s\n", LOG_TAG_DB, pRowDeviceFunc->exRsrcID);
				GRIB_LOGD("%s GET FUNC ATTR    : 0x%x\n", LOG_TAG_DB, pRowDeviceFunc->funcAttr);
				GRIB_LOGD("%s\n", LOG_TAG_DB);
			}
		}
		if(iDBG)GRIB_LOGD("%s---------- ---------- ---------- ---------- ---------- ---------- ----------\n", LOG_TAG_DB);

	}

	return iRes;
}

