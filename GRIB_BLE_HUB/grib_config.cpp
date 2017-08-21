/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "grib_config.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

Grib_SqlInfo gConfigSQL;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

Grib_ConfigInfo gConfigInfo;

void Grib_ShowConfig(Grib_ConfigInfo* pConfigInfo)
{
	char* siServerIP = NULL;

	if(pConfigInfo == NULL)
	{
		pConfigInfo = &gConfigInfo;
	}

	//shbaek: Use DNS
	Grib_GetDnsIP(GRIB_PLATFORM_SERVER_DOMAIN, &siServerIP);
	pConfigInfo->siServerPort = GRIB_PLATFORM_SERVER_PORT;

	GRIB_LOGD("\n");
	GRIB_LOGD(GRIB_1LINE_SHARP);

	GRIB_LOGD("# HUB_ID              : %s\n", pConfigInfo->hubID);

	GRIB_LOGD("# SI_SERVER_DOMAIN    : %s\n", GRIB_PLATFORM_SERVER_DOMAIN);
	GRIB_LOGD("# SI_SERVER_IP        : %s\n", siServerIP);	
	GRIB_LOGD("# SI_SERVER_PORT      : %d\n", pConfigInfo->siServerPort);
	GRIB_LOGD("# SI_IN_NAME          : %s\n", pConfigInfo->siInName);
	GRIB_LOGD("# SI_CSE_NAME         : %s\n", pConfigInfo->siCseName);

	GRIB_LOGD("# AUTH_SERVER_IP      : %s\n", pConfigInfo->authServerIP);
	GRIB_LOGD("# AUTH_SERVER_PORT    : %d\n", pConfigInfo->authServerPort);
	GRIB_LOGD("# SMD_SERVER_IP       : %s\n", pConfigInfo->smdServerIP);
	GRIB_LOGD("# SMD_SERVER_PORT     : %d\n", pConfigInfo->smdServerPort);

	GRIB_LOGD("# RESET_TIME_HOUR     : %d\n", pConfigInfo->resetTimeHour);
	GRIB_LOGD("# DEBUG_LEVEL         : %s\n", GRIB_BOOL_TO_STR(pConfigInfo->debugLevel));
	GRIB_LOGD("# TOMBSTONE_LEVEL     : %s\n", GRIB_BOOL_TO_STR(pConfigInfo->tombStone));

	GRIB_LOGD(GRIB_1LINE_SHARP);
	GRIB_LOGD("\n");

	FREE(siServerIP);

}

int Grib_LoadDefaultConfig(Grib_ConfigInfo* pConfigInfo)
{
	const char* FUNC = "LOAD-CFG";
	int iRes = GRIB_DONE;
	int iDBG = FALSE;

	FILE* pConfigFile = NULL;
	char  pLineBuff[SIZE_1K] = {'\0', };
	char* pTemp = NULL;
	char* pTrim = NULL;
	char* pValue = NULL;

	if(iDBG)GRIB_LOGD("# LOAD CONFIG FILE START\n");

	if(pConfigInfo == NULL)
	{
		pConfigInfo = &gConfigInfo;
	}

	pConfigFile = fopen(GRIB_DEFAULT_CONFIG_FILE_PATH, "r");
	if(pConfigFile == NULL)
	{
		GRIB_LOGD("# LOAD CONFIG FILE FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}

	while(!feof(pConfigFile))
	{
		STRINIT(pLineBuff, sizeof(pLineBuff));

		if( fgets(pLineBuff, sizeof(pLineBuff)-1, pConfigFile) == NULL )
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

		if(STRNCMP(pTrim, GRIB_CONFIG_HUB_ID, STRLEN(GRIB_CONFIG_HUB_ID)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->hubID, sizeof(pConfigInfo->hubID));
			STRNCPY(pConfigInfo->hubID, pValue, STRLEN(pValue));
			continue;
		}

		//shbaek: SI Server
		if(STRNCMP(pTrim, GRIB_CONFIG_SI_SERVER_IP, STRLEN(GRIB_CONFIG_SI_SERVER_IP)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->siServerIP, sizeof(pConfigInfo->siServerIP));
			STRNCPY(pConfigInfo->siServerIP, pValue, STRLEN(pValue));
			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_SI_SERVER_PORT, STRLEN(GRIB_CONFIG_SI_SERVER_PORT)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->siServerPort= ATOI(pValue);
			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_SI_IN_NAME, STRLEN(GRIB_CONFIG_SI_IN_NAME)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->siInName, sizeof(pConfigInfo->siInName));
			STRNCPY(pConfigInfo->siInName, pValue, STRLEN(pValue));
			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_SI_CSE_NAME, STRLEN(GRIB_CONFIG_SI_CSE_NAME)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->siCseName, sizeof(pConfigInfo->siCseName));
			STRNCPY(pConfigInfo->siCseName, pValue, STRLEN(pValue));
			continue;
		}

		//shbaek: Authentication
		if(STRNCMP(pTrim, GRIB_CONFIG_AUTH_SERVER_IP, STRLEN(GRIB_CONFIG_AUTH_SERVER_IP)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->authServerIP, sizeof(pConfigInfo->authServerIP));
			STRNCPY(pConfigInfo->authServerIP, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_AUTH_SERVER_PORT, STRLEN(GRIB_CONFIG_AUTH_SERVER_PORT)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->authServerPort = ATOI(pValue);
			continue;
		}

		//shbaek: Semantic Descriptor
		if(STRNCMP(pTrim, GRIB_CONFIG_SMD_SERVER_IP, STRLEN(GRIB_CONFIG_SMD_SERVER_IP)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->smdServerIP, sizeof(pConfigInfo->smdServerIP));
			STRNCPY(pConfigInfo->smdServerIP, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_SMD_SERVER_PORT, STRLEN(GRIB_CONFIG_SMD_SERVER_PORT)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->smdServerPort = ATOI(pValue);
			continue;
		}

		//shbaek: ETC
		if(STRNCMP(pTrim, GRIB_CONFIG_RESET_TIME_HOUR, STRLEN(GRIB_CONFIG_RESET_TIME_HOUR)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->resetTimeHour = ATOI(pValue);
			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_DEBUG_LEVEL, STRLEN(GRIB_CONFIG_DEBUG_LEVEL)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->debugLevel = ATOI(pValue);
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_TOMBSTONE, STRLEN(GRIB_CONFIG_TOMBSTONE)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_STR_COLON);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;	
			}
			pValue = &pTemp[1];

			pConfigInfo->tombStone = ATOI(pValue);
			continue;
		}

		FREE(pTrim);
		pTrim = NULL;
	}

	if(iDBG)Grib_ShowConfig(pConfigInfo);

	if(pTrim != NULL)
	{
		FREE(pTrim);
		pTrim = NULL;
	}
	if(pConfigFile != NULL)
	{
		fclose(pConfigFile);
		pConfigFile = NULL;
	}

	if(iDBG)GRIB_LOGD("# LOAD CONFIG FILE DONE\n");

	return iRes;
}

int Grib_SetConfigDB(Grib_ConfigInfo* pConfigInfo)
{
	const char* FUNC = "SET-CFG";
	int iRes = GRIB_ERROR;
	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# %s: PARAM NULL ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	if(STRLEN(pConfigInfo->hubID)<=1)
	{
		GRIB_LOGD("# %s: INVALID HUB ID ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	//1 shbaek: SI SERVER
	if(STRLEN(pConfigInfo->siServerIP)<=1)
	{
		GRIB_LOGD("# %s: INVALID SI SERVER IP ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}
	if(pConfigInfo->siServerPort<=0)
	{
		GRIB_LOGD("# %s: INVALID SI SERVER PORT ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}
	if(STRLEN(pConfigInfo->siInName)<=1)
	{
		GRIB_LOGD("# %s: INVALID IN NAME ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}
	if(STRLEN(pConfigInfo->siCseName)<=1)
	{
		GRIB_LOGD("# %s: INVALID CSE NAME ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	//1 shbaek: AUTH SERVER
	if(STRLEN(pConfigInfo->authServerIP)<=1)
	{
		GRIB_LOGD("# %s: INVALID AUTH SERVER IP ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}
	if(pConfigInfo->authServerPort<=0)
	{
		GRIB_LOGD("# %s: INVALID AUTH SERVER PORT ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	//1 shbaek: SMD SERVER
	if(STRLEN(pConfigInfo->smdServerIP)<=1)
	{
		GRIB_LOGD("# %s: INVALID SMD SERVER IP ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	if(pConfigInfo->smdServerPort<=0)
	{
		GRIB_LOGD("# %s: INVALID SMD SERVER PORT ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	//1 shbaek: CHECK VALID DONE ...

	//2 shbaek: DELETE 
	STRINIT(sqlQuery, sizeof(sqlQuery));
	SPRINTF(sqlQuery, "%s", QUERY_DELETE_CONFIG);
	Grib_DbQuery(&gConfigSQL, sqlQuery);

	//2 shbaek: INSERT
	STRINIT(sqlQuery, sizeof(sqlQuery));
	SPRINTF(sqlQuery, QUERY_INSERT_CONFIG, pConfigInfo->hubID, 
											pConfigInfo->siServerIP,   pConfigInfo->siServerPort,
											pConfigInfo->siInName,     pConfigInfo->siCseName,
											pConfigInfo->authServerIP, pConfigInfo->authServerPort,
											pConfigInfo->smdServerIP,  pConfigInfo->smdServerPort,
											pConfigInfo->resetTimeHour,pConfigInfo->debugLevel,
											pConfigInfo->tombStone);
	iRes = Grib_DbQuery(&gConfigSQL, sqlQuery);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: QUERY FAIL !!!\n", FUNC);
		return GRIB_ERROR;
	}

	return iRes;
}


int Grib_GetConfigDB(void)
{
	const char* FUNC = "GET-CFG";
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;

	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };
	MYSQL_ROW   sqlRow = NULL;

	Grib_SqlInfo* pConfigSql = &gConfigSQL;
	Grib_ConfigInfo* pConfigInfo = &gConfigInfo;

	iRes = Grib_MakeSqlInfo(pConfigSql);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: MAKE SQL FAIL !!!\n", FUNC);
		return GRIB_ERROR;
	}
	if(iDBG)Grib_ShowSqlInfo(pConfigSql);

	iRes = Grib_DbConnect(pConfigSql);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: DB CONNECT FAIL !!!\n", FUNC);
		return GRIB_ERROR;
	}

	STRINIT(sqlQuery, sizeof(sqlQuery));
	SPRINTF(sqlQuery, "%s", QUERY_SELECT_CONFIG);

	iRes = Grib_DbQuery(pConfigSql, sqlQuery);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: QUERY FAIL !!!\n", FUNC);
		return GRIB_ERROR;
	}

	pConfigSql->result = mysql_store_result(pConfigSql->connect);
	if(pConfigSql->result == NULL)
	{
		Grib_ErrLog(FUNC, "RESULT IS NULL !!!");
		goto FINAL;
	}

	if(0 < Grib_DbGetRowCount(pConfigSql))
	{//shbaek: Normal ...
		sqlRow = mysql_fetch_row(pConfigSql->result);
		if(sqlRow == NULL)
		{
			GRIB_LOGD("# %s: FETCH ROW ERROR !!!\n", FUNC);
			return GRIB_ERROR;
		}

		MEMSET(pConfigInfo, 0x00, sizeof(Grib_ConfigInfo));

		STRNCPY(pConfigInfo->hubID, sqlRow[INDEX_CONFIG_HUB_ID], STRLEN(sqlRow[INDEX_CONFIG_HUB_ID]));
		STRNCPY(pConfigInfo->siServerIP, sqlRow[INDEX_CONFIG_SI_SERVER_IP], STRLEN(sqlRow[INDEX_CONFIG_SI_SERVER_IP]));
		pConfigInfo->siServerPort = ATOI(sqlRow[INDEX_CONFIG_SI_SERVER_PORT]);
		
		STRNCPY(pConfigInfo->siInName, sqlRow[INDEX_CONFIG_SI_IN_ADDR], STRLEN(sqlRow[INDEX_CONFIG_SI_IN_ADDR]));
		STRNCPY(pConfigInfo->siCseName, sqlRow[INDEX_CONFIG_SI_CSE_ADDR], STRLEN(sqlRow[INDEX_CONFIG_SI_CSE_ADDR]));

		STRNCPY(pConfigInfo->authServerIP, sqlRow[INDEX_CONFIG_AUTH_SERVER_IP], STRLEN(sqlRow[INDEX_CONFIG_AUTH_SERVER_IP]));
		pConfigInfo->authServerPort = ATOI(sqlRow[INDEX_CONFIG_AUTH_SERVER_PORT]);

		STRNCPY(pConfigInfo->smdServerIP, sqlRow[INDEX_CONFIG_SMD_SERVER_IP], STRLEN(sqlRow[INDEX_CONFIG_SMD_SERVER_IP]));
		pConfigInfo->smdServerPort = ATOI(sqlRow[INDEX_CONFIG_SMD_SERVER_PORT]);

		pConfigInfo->resetTimeHour = ATOI(sqlRow[INDEX_CONFIG_RESET_HOUR]);
		pConfigInfo->debugLevel    = ATOI(sqlRow[INDEX_CONFIG_DEBUG_LEVEL]);
		pConfigInfo->tombStone     = ATOI(sqlRow[INDEX_CONFIG_TOMBSTONE]);

	}
	else
	{//shbaek: No Config Data !!!
		iRes = Grib_LoadDefaultConfig(pConfigInfo);
		if(iRes!=GRIB_DONE)
		{
			GRIB_LOGD("# %s: LOAD DEFAULT CONFIG ERROR !!!\n", FUNC);
			return GRIB_ERROR;
		}

		iRes = Grib_SetConfigDB(pConfigInfo);
		if(iRes!=GRIB_DONE)
		{
			GRIB_LOGD("# %s: SET DEFAULT CONFIG ERROR !!!\n", FUNC);
			return GRIB_ERROR;
		}
	}

	pConfigInfo->isLoad = TRUE;

FINAL:
	if(pConfigSql->result!=NULL)
	{
		mysql_free_result(pConfigSql->result);
		pConfigSql->result = NULL;
	}

	return iRes;
}

Grib_ConfigInfo* Grib_GetConfigInfo(void)
{
	int iRes = GRIB_ERROR;
	int LOOP = 1;//10;

	if(gConfigInfo.isLoad)
	{
		return &gConfigInfo;
	}

LOAD_CONFIG:
	LOOP--;
	iRes = Grib_GetConfigDB();
	if(iRes != GRIB_DONE)
	{
		SLEEP(1);
		if(0<LOOP)goto LOAD_CONFIG;
		return NULL;
	}

	gConfigInfo.isLoad = TRUE;

	return &gConfigInfo;
}

int Grib_SetConfigHub(Grib_ConfigInfo* pConfigInfo)
{
	const char* FUNC = "SET-HUB";

	int i = 0;
	int iCount = 0;
	int iRes = GRIB_ERROR;

	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };
	char sqlValue[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# %s: PARAM NULL ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	//2 shbaek: UPDATE
	STRINIT(sqlValue, sizeof(sqlValue));
	SPRINTF(sqlValue, COLUMN_HUB_ID "=\"%s\"", pConfigInfo->hubID);

	STRINIT(sqlQuery, sizeof(sqlQuery));
	SPRINTF(sqlQuery, QUERY_UPDATE_CONFIG, sqlValue);

	iRes = Grib_DbQuery(&gConfigSQL, sqlQuery);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: QUERY FAIL: %s [%d]\n", FUNC, gConfigSQL.errStr, gConfigSQL.errNum);
		return GRIB_ERROR;
	}

	return iRes;
}

int Grib_SetConfigSi(Grib_ConfigInfo* pConfigInfo)
{
	const char* FUNC = "SET-SI";

	int i = 0;
	int iCount = 0;
	int iRes = GRIB_ERROR;

	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };
	char sqlValue[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# %s: PARAM NULL ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	//2 shbaek: UPDATE
	STRINIT(sqlValue, sizeof(sqlValue));
	SPRINTF(sqlValue, COLUMN_SI_SERVER_IP "=\"%s\", " COLUMN_SI_SERVER_PORT "=%d, " COLUMN_SI_IN_ADDR "=\"%s\", " COLUMN_SI_CSE_ADDR "=\"%s\" ",
				pConfigInfo->siServerIP, pConfigInfo->siServerPort, pConfigInfo->siInName, pConfigInfo->siCseName);

	STRINIT(sqlQuery, sizeof(sqlQuery));
	SPRINTF(sqlQuery, QUERY_UPDATE_CONFIG, sqlValue);

	iRes = Grib_DbQuery(&gConfigSQL, sqlQuery);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: QUERY FAIL: %s [%d]\n", FUNC, gConfigSQL.errStr, gConfigSQL.errNum);
		return GRIB_ERROR;
	}

	return iRes;
}

int Grib_SetConfigEtc(Grib_ConfigInfo* pConfigInfo)
{
	const char* FUNC = "SET-ETC";

	int i = 0;
	int iCount = 0;
	int iRes = GRIB_ERROR;

	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };
	char sqlValue[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# %s: PARAM NULL ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	//2 shbaek: UPDATE
	STRINIT(sqlValue, sizeof(sqlValue));
	SPRINTF(sqlValue, COLUMN_RESET_HOUR "=%d, " COLUMN_DEBUG_LEVEL "=%d, " COLUMN_TOMBSTONE "=%d ",
				pConfigInfo->resetTimeHour, pConfigInfo->debugLevel, pConfigInfo->tombStone);

	STRINIT(sqlQuery, sizeof(sqlQuery));
	SPRINTF(sqlQuery, QUERY_UPDATE_CONFIG, sqlValue);

	iRes = Grib_DbQuery(&gConfigSQL, sqlQuery);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: QUERY FAIL: %s [%d]\n", FUNC, gConfigSQL.errStr, gConfigSQL.errNum);
		return GRIB_ERROR;
	}

	return iRes;
}

