/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "grib_db.h"

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


#define __GRIB_DB_COMMON__

int Grib_DbInfoLoad(void)
{
	const char* FUNC_TAG = "DB-INFO";
	int iDBG = FALSE;

	int iRes = GRIB_DONE;
	int isLoad = TRUE;

	FILE* pDbInfoFile = NULL;
	char  pLineBuff[SIZE_1K] = {'\0', };
	char* pTemp = NULL;
	char* pTrim = NULL;
	char* pValue = NULL;

	if(iDBG)GRIB_LOGD("# %s: START\n", FUNC_TAG);

	if( (STRLEN(gSqlHost)==0) || (STRLEN(gSqlUser)==0) || (STRLEN(gSqlPswd)==0) || (gSqlPort<=0) )
	{//shbaek: Need Load !!!
		isLoad = FALSE;
	}

	if(isLoad == TRUE)
	{
		if(iDBG)GRIB_LOGD("# %s: ALREADY LOAD DB INFO ...\n", FUNC_TAG);
		return GRIB_DONE;
	}

	pDbInfoFile = fopen(GRIB_DB_INFO_FILE_PATH, "r");
	if(pDbInfoFile == NULL)
	{
		GRIB_LOGD("# %s FAIL: %s[%d]\n", FUNC_TAG, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}

	while(!feof(pDbInfoFile))
	{
		STRINIT(pLineBuff, sizeof(pLineBuff));

		if( fgets(pLineBuff, sizeof(pLineBuff)-1, pDbInfoFile) == NULL )
		{
			continue;
		}

		pLineBuff[STRLEN(pLineBuff)-1] = '\0';
		pTrim = Grib_TrimAll(pLineBuff);
		if(pTrim == NULL)continue;

		if(*pTrim == GRIB_HASH)
		{
			continue;
		}
		else if(*pTrim == '\0')
		{
			continue;
		}

		//shbaek: MySQL
		if(STRNCMP(pTrim, GRIB_DB_INFO_MYSQL_HOST, STRLEN(GRIB_DB_INFO_MYSQL_HOST)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# %s: THIS LINE NOT \":\" EXIST\n", FUNC_TAG);
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(gSqlHost, sizeof(gSqlHost));
			STRNCPY(gSqlHost, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_DB_INFO_MYSQL_PORT, STRLEN(GRIB_DB_INFO_MYSQL_PORT)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# %s: THIS LINE NOT \":\" EXIST\n", FUNC_TAG);
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			gSqlPort = ATOI(pValue);
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_DB_INFO_MYSQL_USER, STRLEN(GRIB_DB_INFO_MYSQL_USER)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# %s: THIS LINE NOT \":\" EXIST\n", FUNC_TAG);
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(gSqlUser, sizeof(gSqlUser));
			STRNCPY(gSqlUser, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_DB_INFO_MYSQL_PASSWORD, STRLEN(GRIB_DB_INFO_MYSQL_PASSWORD)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# %s: THIS LINE NOT \":\" EXIST\n", FUNC_TAG);
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(gSqlPswd, sizeof(gSqlPswd));
			STRNCPY(gSqlPswd, pValue, STRLEN(pValue));
			continue;
		}

		FREE(pTrim);
		pTrim = NULL;
	}

	if(iDBG)
	{
		GRIB_LOGD("\n");
		GRIB_LOGD("# %s: MYSQL_DB_HOST       : [%s]\n", FUNC_TAG, gSqlHost);
		GRIB_LOGD("# %s: MYSQL_DB_PORT       : [%d]\n", FUNC_TAG, gSqlPort);
		GRIB_LOGD("# %s: MYSQL_DB_USER       : [%s]\n", FUNC_TAG, gSqlUser);
		GRIB_LOGD("# %s: MYSQL_DB_PASSWORD   : [%s]\n", FUNC_TAG, gSqlPswd);
		GRIB_LOGD("\n");
	}

	if(pTrim != NULL)
	{
		FREE(pTrim);
		pTrim = NULL;
	}
	if(pDbInfoFile != NULL)
	{
		fclose(pDbInfoFile);
		pDbInfoFile = NULL;
	}

	if(iDBG)GRIB_LOGD("# %s: LOAD DB INFO DONE\n", FUNC_TAG);

	return iRes;
}

int Grib_ShowSqlInfo(Grib_SqlInfo* pSQL)
{
	const char* FUNC_TAG = "SHOW-SQL";

	if(pSQL == NULL)
	{
		GRIB_LOGD("# %s: PARAM IS NULL ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	GRIB_LOGD("\n");
	GRIB_LOGD(GRIB_1LINE_SHARP);
	GRIB_LOGD("# MYSQL_DB_HOST       : %s\n", pSQL->host);
	GRIB_LOGD("# MYSQL_DB_PORT       : %d\n", pSQL->port);
	GRIB_LOGD("# MYSQL_DB_USER       : %s\n", pSQL->user);
	GRIB_LOGD("# MYSQL_DB_PASSWORD   : %s\n", pSQL->pswd);
	GRIB_LOGD("# MYSQL_DB_NAME       : %s\n", pSQL->db);
	GRIB_LOGD("# MYSQL_DB_ERROR      : %s\n", pSQL->errStr==NULL?"NO ERROR":pSQL->errStr);
	GRIB_LOGD(GRIB_1LINE_SHARP);
	GRIB_LOGD("\n");

	return GRIB_DONE;

}

int Grib_FreeSqlInfo(Grib_SqlInfo* pSQL)
{
	const char* FUNC_TAG = "FREE-SQL";
	int iDBG = TRUE;
	int iRes = GRIB_DONE;

	if(pSQL == NULL)
	{
		GRIB_LOGD("# %s: PARAM IS NULL ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	if(0 < STRLEN(pSQL->host))FREE(pSQL->host);
	if(0 < STRLEN(pSQL->user))FREE(pSQL->user);
	if(0 < STRLEN(pSQL->pswd))FREE(pSQL->pswd);
	if(0 < STRLEN(pSQL->db))FREE(pSQL->db);

	if(pSQL->result!=NULL)
	{//shbaek: Free Result
		mysql_free_result(pSQL->result);
		pSQL->result = NULL;
	}

	if(pSQL->connect != NULL)
	{//shbaek: Free Connect
		mysql_close(pSQL->connect);
		pSQL->connect = NULL;
	}

	pSQL->port   = 0;
	pSQL->errNum = 0;
	pSQL->errStr = NULL;

	return iRes;
}

int Grib_MakeSqlInfo(Grib_SqlInfo* pSQL)
{
	const char* FUNC_TAG = "MAKE-SQL";
	int iDBG = TRUE;
	int iRes = GRIB_DONE;

	if(pSQL == NULL)
	{
		GRIB_LOGD("# %s: PARAM IS NULL ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	iRes = Grib_DbInfoLoad();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: DB INFO LOAD FAIL !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	MEMSET(pSQL, 0x00, sizeof(Grib_SqlInfo));

	pSQL->connect = (MYSQL*) MALLOC(sizeof(MYSQL));
	pSQL->result  = NULL;

	pSQL->host = STRDUP(gSqlHost);
	pSQL->user = STRDUP(gSqlUser);
	pSQL->pswd = STRDUP(gSqlPswd);
	pSQL->port = gSqlPort;

	pSQL->db   = STRDUP(MYSQL_DB_NAME);

	pSQL->errStr = NULL;
	pSQL->errNum = 0;

	return iRes;
}

int Grib_DbDisconnect(Grib_SqlInfo* pSQL)
{
	if(pSQL->connect == NULL)
	{
		GRIB_LOGD("%s PARAM IS NULL ERROR !!!\n", LOG_TAG_DB);
		return GRIB_ERROR;
	}

	mysql_close(pSQL->connect);
	pSQL->connect = NULL;

	GRIB_LOGD("%s DIS-CONNECT DONE\n", LOG_TAG_DB);

	return GRIB_DONE;
}

int Grib_DbConnect(Grib_SqlInfo* pSQL)
{
	MYSQL* myConnect;
	int iDBG = FALSE;

	if(pSQL->db == NULL)
	{
		GRIB_LOGD("%s PARAM IS NULL ERROR !!!\n", LOG_TAG_DB);
		return GRIB_ERROR;
	}

	Grib_DbInfoLoad();

	myConnect = mysql_init(pSQL->connect);

	if(iDBG)GRIB_LOGD("%s MYSQL VERSION: %s\n", LOG_TAG_DB, mysql_get_client_info());

	myConnect = mysql_real_connect(pSQL->connect, pSQL->host, 
							pSQL->user, pSQL->pswd,
							pSQL->db, pSQL->port,
							(char *)NULL, GRIB_NOT_USED);

	if(myConnect == NULL)
	{//shbaek: Connection Fail
		pSQL->errStr = (char*)MYSQL_ERROR_STR(pSQL->connect);
		pSQL->errNum = MYSQL_ERROR_NUM(pSQL->connect);

		GRIB_LOGD("%s %s CONNECT FAIL: %s[%d]\n", LOG_TAG_DB, pSQL->db, pSQL->errStr, pSQL->errNum);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("%s %s DB CONNECTED\n\n", LOG_TAG_DB, pSQL->db);

	return GRIB_DONE;
}

int Grib_DbQuery(Grib_SqlInfo* pSQL, char* sqlQuery)
{
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;

	if( (pSQL==NULL) || (pSQL->connect==NULL) ||(STRLEN(sqlQuery)==0) )
	{
		GRIB_LOGD("%s IN-VALID PARAM !!!\n", LOG_TAG_DB);
		GRIB_LOGD("%s SQL CONNECT: %p\n", LOG_TAG_DB, pSQL->connect);
		GRIB_LOGD("%s SQL QUERY: %s\n", LOG_TAG_DB, sqlQuery);

		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	if(pSQL->result!=NULL)
	{//shbaek: Free Result
		mysql_free_result(pSQL->result);
		pSQL->result = NULL;
	}

	iRes = mysql_query(pSQL->connect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		if(iDBG)GRIB_LOGD("# QUERY: %s ERROR !!!\n", sqlQuery);
		goto ERROR;
	}

	pSQL->result = mysql_store_result(pSQL->connect);

	if(iDBG)GRIB_LOGD("%s QUERY DONE ...\n", LOG_TAG_DB);

	return GRIB_DONE;

ERROR:
	pSQL->errStr = (char*)MYSQL_ERROR_STR(pSQL->connect);
	pSQL->errNum = MYSQL_ERROR_NUM(pSQL->connect);

	return GRIB_ERROR;
}


int Grib_DbGetRowCount(Grib_SqlInfo* pSQL)
{
	const char* FUNC_TAG = "ROW-COUNT";
	int iDBG = FALSE;
	int iCount = 0;

	if( (pSQL==NULL) || (pSQL->connect==NULL) || (pSQL->result==NULL) )
	{
		GRIB_LOGD("# %s: PARAM IS NULL ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	iCount = mysql_num_rows(pSQL->result);
	if(iDBG)GRIB_LOGD("# %s: %d\n", FUNC_TAG, iCount);

	return iCount;
}


#define __GRIB_DB_BASIC__

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
	gSqlPort    = 0;
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

	Grib_DbInfoLoad();

	gSqlConnect = mysql_init(GRIB_NOT_USED);

	GRIB_LOGD("%s MYSQL VERSION: %s\n", LOG_TAG_DB, mysql_get_client_info());

	gSqlConnect = mysql_real_connect(gSqlConnect, gSqlHost, 
							gSqlUser, gSqlPswd,
							MYSQL_DB_NAME, gSqlPort,
							(char *)NULL, GRIB_NOT_USED);

	if(gSqlConnect == NULL)
	{//shbaek: Connection Fail
		GRIB_LOGD("%s MYSQL CONNECT FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s MYSQL CONNECTED\n", LOG_TAG_DB);

	return GRIB_DONE;
}

int Grib_DbCreate(void)
{
	int iRes = GRIB_ERROR;

	Grib_DbInfoLoad();

	gSqlConnect = mysql_init(GRIB_NOT_USED);

	GRIB_LOGD("%s MYSQL VERSION: %s\n", LOG_TAG_DB, mysql_get_client_info());

	//shbaek: Connect
	gSqlConnect = mysql_real_connect(gSqlConnect, gSqlHost, 
							gSqlUser, gSqlPswd,
							GRIB_NOT_USED, gSqlPort,
							(char *)NULL, GRIB_NOT_USED);
	if(gSqlConnect == NULL)
	{//shbaek: Connection Fail
		GRIB_LOGD("%s OPEN FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s MYSQL CONNECTED\n", LOG_TAG_DB);

	//shbaek: Create DB
	iRes = mysql_query(gSqlConnect, QUERY_DB_CREATE);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE DB FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE DB DONE\n", LOG_TAG_DB);

	//shbaek: Choose DB
	iRes = mysql_query(gSqlConnect, QUERY_DB_USE);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s USE DB FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s USE DB DONE\n", LOG_TAG_DB);

	//shbaek: Create Device Info Table
	iRes = mysql_query(gSqlConnect, QUERY_CREATE_DEVICE_INFO);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE DEVICE TABLE FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE DEVICE TABLE DONE\n", LOG_TAG_DB);

	//shbaek: Create Device func Table
	iRes = mysql_query(gSqlConnect, QUERY_CREATE_DEVICE_FUNC);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE FUNC TABLE FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE FUNC TABLE DONE\n", LOG_TAG_DB);

	//shbaek: Create Config Table
	iRes = mysql_query(gSqlConnect, QUERY_CREATE_CONFIG);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE CONFIG TABLE FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE CONFIG TABLE DONE\n", LOG_TAG_DB);

	//shbaek: Create Cache Table
	iRes = mysql_query(gSqlConnect, QUERY_CREATE_CACHE_RI);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE CACHE TABLE FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE CACHE TABLE DONE\n", LOG_TAG_DB);

	//shbaek: Create Scan Table
	iRes = mysql_query(gSqlConnect, QUERY_CREATE_SCAN_DEVICE);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CREATE SCAN TABLE FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
//		return GRIB_ERROR;
	}
	GRIB_LOGD("%s CREATE SCAN TABLE DONE\n", LOG_TAG_DB);

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

	iRes = mysql_query(gSqlConnect, QUERY_DB_DROP);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s DROP DB FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
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

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_INSERT_DEVICE_INFO, 
							pRowDeviceInfo->deviceID, pRowDeviceInfo->deviceInterface, 
							pRowDeviceInfo->deviceAddr, pRowDeviceInfo->deviceFuncCount,
							pRowDeviceInfo->deviceLoc, pRowDeviceInfo->deviceDesc,
							pRowDeviceInfo->reportCycle);

	GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
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

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_SELECT_DEVICE_INFO, pRowDeviceInfo->deviceID);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
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

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_DELETE_DEVICE_INFO, deviceID);
	GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s DELETE DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
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

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_INSERT_DEVICE_FUNC, pRowDeviceFunc->deviceID, 
							pRowDeviceFunc->funcName, pRowDeviceFunc->funcAttr);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SET DEVICE FUNC FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
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

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_SELECT_DEVICE_FUNC, deviceID);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET DEVICE INFO FAIL: %s[%d]\n", LOG_TAG_DB, 
			MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
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
//		STRNCPY(ppRowDeviceFunc[i]->exRsrcID, gSqlRow[INDEX_FUNC_EXRI], STRLEN(gSqlRow[INDEX_FUNC_EXRI]));
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

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_DELETE_DEVICE_FUNC, deviceID);
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

	STRNCPY(sqlQuery, QUERY_SELECT_DEVICE_INFO_ALL, STRLEN(QUERY_SELECT_DEVICE_INFO_ALL));

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

	STRNCPY(sqlQuery, QUERY_SELECT_DEVICE_INFO_ALL, STRLEN(QUERY_SELECT_DEVICE_INFO_ALL));
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
	if(iDBG)GRIB_LOGD("%s GET DEVICE COUNT : %d\n", LOG_TAG_DB, pDbAll->deviceCount);

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
//				GRIB_LOGD("%s GET EXE RSRC ID  : %s\n", LOG_TAG_DB, pRowDeviceFunc->exRsrcID);
				GRIB_LOGD("%s GET FUNC ATTR    : 0x%x\n", LOG_TAG_DB, pRowDeviceFunc->funcAttr);
				GRIB_LOGD("%s\n", LOG_TAG_DB);
			}
		}
		if(iDBG)GRIB_LOGD("%s---------- ---------- ---------- ---------- ---------- ---------- ----------\n", LOG_TAG_DB);

	}

	return iRes;
}




#define __GRIB_DB_CACHE_RI__

int Grib_DbSetCacheRi(Grib_DbRowCacheRi* pRowCacheRi)
{
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(iDBG)GRIB_LOGD("%s SET CACHE RI\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_INSERT_CACHE_RI, 
							pRowCacheRi->rid, pRowCacheRi->rtype,
							pRowCacheRi->rname, pRowCacheRi->pid,
							pRowCacheRi->uri);

	GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SET CACHE RI FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("%s SET CACHE RI DONE\n", LOG_TAG_DB);

	return iRes;
}

//2 shbaek: Need RI
int Grib_DbGetCacheCount(void)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = TRUE;
	int iRowCount = 0;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(iDBG)GRIB_LOGD("%s GET CACHE COUNT\n", LOG_TAG_DB);

	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	STRNCPY(sqlQuery, QUERY_SELECT_CACHE_RI_ALL, STRLEN(QUERY_SELECT_CACHE_RI_ALL));
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET CACHE COUNT FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s CACHE RI TABLE IS EMPTY?\n", LOG_TAG_DB);
		goto FINAL;
	}

	iRowCount = mysql_num_rows(gSqlResult);
	if(iDBG)GRIB_LOGD("%s ROW COUNT: %d\n", LOG_TAG_DB, iRowCount);

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return iRowCount;
}

//2 shbaek: Need Ri
int Grib_DbGetCacheInfo(Grib_DbRowCacheRi* pRowCacheRi)
{
	int iRes = GRIB_ERROR;
	int iDBG = TRUE;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(iDBG)GRIB_LOGD("%s GET CACHE RI\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_SELECT_CACHE_RI, pRowCacheRi->rid);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET CACHE RI FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s NO MATCHING RESOUCE ID: %s\n", LOG_TAG_DB, pRowCacheRi->rid);
		iRes = GRIB_ERROR;
		goto FINAL;
	}

	gSqlRow = NULL;
	gSqlRow = mysql_fetch_row(gSqlResult);
	if(gSqlRow == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s NO MATCHING RESOUCE ID: %s\n", LOG_TAG_DB, pRowCacheRi->rid);
		iRes = GRIB_ERROR;
		goto FINAL;
	}

	//2 shbaek: Skip Resource ID
	pRowCacheRi->rtype = ATOI(gSqlRow[INDEX_CACHE_TY]);
	STRNCPY(pRowCacheRi->rname, gSqlRow[INDEX_CACHE_RN], STRLEN(gSqlRow[INDEX_CACHE_RN]));
	STRNCPY(pRowCacheRi->pid, gSqlRow[INDEX_CACHE_PI], STRLEN(gSqlRow[INDEX_CACHE_PI]));
	STRNCPY(pRowCacheRi->uri, gSqlRow[INDEX_CACHE_URI], STRLEN(gSqlRow[INDEX_CACHE_URI]));

	if(iDBG)GRIB_LOGD("%s GET CACHE RI DONE\n", LOG_TAG_DB);

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return iRes;
}

//2 shbaek: Need URI
int Grib_DbGetCacheRi(Grib_DbRowCacheRi* pRowCacheRi)
{
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(iDBG)GRIB_LOGD("%s GET CACHE URI\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_SELECT_CACHE_URI, pRowCacheRi->uri);
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET CACHE URI FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s NO MATCHING URI: %s\n", LOG_TAG_DB, pRowCacheRi->uri);
		iRes = GRIB_ERROR;
		goto FINAL;
	}

	gSqlRow = NULL;
	gSqlRow = mysql_fetch_row(gSqlResult);
	if(gSqlRow == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s NO MATCHING URI: %s\n", LOG_TAG_DB, pRowCacheRi->uri);
		iRes = GRIB_ERROR;
		goto FINAL;
	}

	//2 shbaek: Skip URI
	pRowCacheRi->rtype = ATOI(gSqlRow[INDEX_CACHE_TY]);
	STRNCPY(pRowCacheRi->rid, gSqlRow[INDEX_CACHE_RI], STRLEN(gSqlRow[INDEX_CACHE_RI]));
	STRNCPY(pRowCacheRi->rname, gSqlRow[INDEX_CACHE_RN], STRLEN(gSqlRow[INDEX_CACHE_RN]));
	STRNCPY(pRowCacheRi->pid, gSqlRow[INDEX_CACHE_PI], STRLEN(gSqlRow[INDEX_CACHE_PI]));

	mysql_free_result(gSqlResult);
	if(iDBG)GRIB_LOGD("%s GET CACHE RI DONE\n", LOG_TAG_DB);

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return iRes;
}

int Grib_DbGetCacheRiAll(Grib_DbCacheRiAll* pCacheRiAll)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	int iRowCount = 0;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	Grib_DbRowCacheRi** ppRowCacheRi;

	if(iDBG)GRIB_LOGD("%s GET CACHE RI ALL: %p\n", LOG_TAG_DB, pCacheRiAll);

	if(pCacheRiAll == NULL)
	{
		GRIB_LOGD("%s: IN-VALID PARAM !!!\n", LOG_TAG_DB);
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

	STRNCPY(sqlQuery, QUERY_SELECT_CACHE_RI_ALL, STRLEN(QUERY_SELECT_CACHE_RI_ALL));
	if(iDBG)GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s GET CACHE RI FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}

	gSqlResult = mysql_store_result(gSqlConnect);
	if(gSqlResult == NULL)
	{//shbaek: Need Manual Regi
		GRIB_LOGD("%s CACHE RI TABLE IS EMPTY?\n", LOG_TAG_DB);
		goto FINAL;
	}

	iRowCount = mysql_num_rows(gSqlResult);
	if(iDBG)GRIB_LOGD("%s ROW COUNT: %d\n", LOG_TAG_DB, iRowCount);


	//shbaek: Get Cache Count
	pCacheRiAll->cacheCount= iRowCount;
	pCacheRiAll->ppRowCacheRi = (Grib_DbRowCacheRi**) MALLOC(sizeof(Grib_DbRowCacheRi*) * pCacheRiAll->cacheCount);
	ppRowCacheRi = pCacheRiAll->ppRowCacheRi;
	for(i=0; i<iRowCount; i++)
	{
		ppRowCacheRi[i] = (Grib_DbRowCacheRi*) MALLOC(sizeof(Grib_DbRowCacheRi));
		MEMSET(ppRowCacheRi[i], GRIB_INIT, sizeof(Grib_DbRowCacheRi));
	}

	for(i=0; i<iRowCount; i++)
	{
		gSqlRow = mysql_fetch_row(gSqlResult);

		ppRowCacheRi[i]->rtype = ATOI(gSqlRow[INDEX_CACHE_TY]);
		STRNCPY(ppRowCacheRi[i]->rid, gSqlRow[INDEX_CACHE_RI], STRLEN(gSqlRow[INDEX_CACHE_RI]));
		STRNCPY(ppRowCacheRi[i]->rname, gSqlRow[INDEX_CACHE_RN], STRLEN(gSqlRow[INDEX_CACHE_RN]));
		STRNCPY(ppRowCacheRi[i]->pid, gSqlRow[INDEX_CACHE_PI], STRLEN(gSqlRow[INDEX_CACHE_PI]));
		STRNCPY(ppRowCacheRi[i]->uri, gSqlRow[INDEX_CACHE_URI], STRLEN(gSqlRow[INDEX_CACHE_URI]));

		if(iDBG)
		{
			GRIB_LOGD("%s: CACHE RID: %s\n", LOG_TAG_DB, ppRowCacheRi[i]->rid);
			GRIB_LOGD("%s: CACHE RTY: %d\n", LOG_TAG_DB, ppRowCacheRi[i]->rtype);
			GRIB_LOGD("%s: CACHE URI: %s\n", LOG_TAG_DB, ppRowCacheRi[i]->uri);
		}

	}

	if(iDBG)GRIB_LOGD("%s GET CACHE RI ALL DONE\n", LOG_TAG_DB);

FINAL:
	if(gSqlResult!=NULL)
	{
		mysql_free_result(gSqlResult);
		gSqlResult = NULL;
	}

	return GRIB_DONE;
}

int Grib_DbDelCacheRi(char* rid)
{
	int iRes = GRIB_ERROR;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	GRIB_LOGD("%s DELETE CACHE RI\n", LOG_TAG_DB);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	SNPRINTF(sqlQuery, MYSQL_MAX_SIZE_QUERY, QUERY_DELETE_CACHE_RI, rid);
	GRIB_LOGD("%s QUERY: %s\n", LOG_TAG_DB, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s DELETE CACHE RI FAIL: %s[%d]\n", LOG_TAG_DB, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("%s DELETE CACHE RI DONE\n", LOG_TAG_DB);

	return iRes;
}

int Grib_DbDelCacheRiAll(void)
{
	int iRes = GRIB_ERROR;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };
	const char* FUNC_TAG = "CLEAN-CACHE";

	GRIB_LOGD("# %s: DELETE CACHE RI ALL\n", FUNC_TAG);
	if(gSqlConnect == NULL)
	{
		iRes = Grib_DbOpen();
		if(iRes == GRIB_ERROR)
		{
			Grib_DbClose();
			return GRIB_ERROR;
		}
	}

	STRNCPY(sqlQuery, QUERY_DELETE_CACHE_RI_ALL, STRLEN(QUERY_DELETE_CACHE_RI_ALL));
	GRIB_LOGD("# %s: QUERY: %s\n", FUNC_TAG, sqlQuery);

	iRes = mysql_query(gSqlConnect, sqlQuery);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: DELETE CACHE RI ALL FAIL: %s[%d]\n", FUNC_TAG, MYSQL_ERROR_STR(gSqlConnect), MYSQL_ERROR_NUM(gSqlConnect));
		return GRIB_ERROR;
	}
	GRIB_LOGD("# %s: DELETE CACHE RI ALL DONE\n", FUNC_TAG);

	return iRes;
}

