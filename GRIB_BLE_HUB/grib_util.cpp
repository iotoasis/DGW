/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "include/grib_util.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
char* Grib_Split(char* strSrc, char searchChar, int searchCount)
{
	int i=0;
	int iCount = 0;

	int iTmpCount = 0;
	int iSrcCount = 0;
	int iMaxCount = 0;
	
	char tmpChar = NULL;
	char static sTmpBuff[SIZE_1K];

	STRINIT(sTmpBuff, sizeof(sTmpBuff));

	if(strSrc == NULL)
	{
		return NULL;
	}
	iMaxCount = STRLEN(strSrc);

	do
	{
		tmpChar = strSrc[iSrcCount];
		iSrcCount++;

		if(sizeof(sTmpBuff) <= iTmpCount)return NULL;
		sTmpBuff[iTmpCount] = tmpChar;
		iTmpCount++;

		if(tmpChar == searchChar)
		{
			if(iCount == searchCount)break;
			iCount++;
			iTmpCount = 0;
		}
		
	}while(tmpChar != NULL);

	sTmpBuff[iTmpCount-1] = NULL;
	return sTmpBuff;
}

int Grib_CountChar(char* strSrc, char c)
{
	int i=0;
	int iCount = 0;
	char temp = '\0';

	if(strSrc == NULL)
	{
		return GRIB_FAIL;
	}

	do
	{
		temp = *(strSrc++);
		if(temp == c)
		{
			iCount++;
		}
	}while(temp != '\0');

	return iCount;
}

int skipSpace(char **str2Buff)
{
	int iCount = 0;

	if(*str2Buff == NULL)return iCount;

	while( (**str2Buff != '\0') && (isspace(**str2Buff)) )
	{
		(*str2Buff)++;
		iCount++;
	}

	return iCount;
}

//shbaek: returns 1 if line starts with prefix, 0 if it does not */
int strStartsWith(const char *line, const char *prefix)
{
	for ( ; (*line!='\0') && (*prefix!='\0'); line++, prefix++)
	{
		if (*line != *prefix)
		{
			return 0;
		}
	}

	return *prefix == '\0';
}

int strLower(char *strBuff)
{
	int i = 0;
	int iSize = 0;

	if(strBuff==NULL)return -1;

	iSize = STRLEN(strBuff);

	for(i=0; i<iSize; i++)
	{
		strBuff[i] = tolower(strBuff[i]);
	}

	return 0;
}

int strUpper(char *strBuff)
{
	int i = 0;
	int iSize = 0;

	if(strBuff==NULL)return -1;

	iSize = STRLEN(strBuff);

	for(i=0; i<iSize; i++)
	{
		strBuff[i] = toupper(strBuff[i]);
	}

	return 0;
}

void mSleep(long long mSec)
{
	struct timespec ts;
	int iRes = EINTR;

	ts.tv_sec	= (mSec / 1000); 				//shbaek: Sec
	ts.tv_nsec	= (mSec % 1000) * 1000 * 1000;	//shbaek: Nano Sec

	do{
		iRes = nanosleep(&ts, &ts);
	}while(iRes < 0 && errno == EINTR);

	return;
}

int systemCommand(char *pCommand, char *pLineBuffer, int iBufferSize)
{
    FILE*	fp = NULL;
    size_t	readSize = 0;

	//shbaek: Excute Command
    fp = popen(pCommand, "r");
    if(!fp)
    {
    	strcpy(pLineBuffer, strerror(errno));
        return GRIB_FAIL;
    }

	//shbaek: Read Result
    readSize = fread((void*)pLineBuffer, sizeof(char), iBufferSize, fp);
    if(readSize <= 0)
    {
	    strcpy(pLineBuffer, LINUX_ERROR_STR);
        pclose(fp);
        return GRIB_FAIL;
    }

    pclose(fp);
    pLineBuffer[readSize] = NULL;

    return GRIB_DONE;
}

int Grib_GetHostName(char* pBuff)
{
	int  iRes = GRIB_ERROR;
	char strHostName[256];
	size_t sizeHostName = 256;

	iRes = gethostname(strHostName, sizeHostName);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# GET HOST NAME ERROR [MSG: %s(%d)]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return iRes;
	}

	GRIB_LOGD("# HOST NAME: %s\n", strHostName);
	STRINIT(pBuff, sizeHostName);
	STRNCPY(pBuff, strHostName, STRLEN(strHostName));

	return GRIB_DONE;
}

int Grib_GetIPAddr(char* pBuff)
{
	const int MAX_SIZE_IP_STR = 128;

	int  iRes = GRIB_ERROR;
	struct ifaddrs *ifp = NULL;
	struct ifaddrs *ifa = NULL;

	char  strIP[MAX_SIZE_IP_STR];
	const char* STR_LOCAL_HOST_IP = "127.0.0.1";

	iRes = getifaddrs(&ifp);
	if(iRes < 0)
	{
		GRIB_LOGD("# GET IP ADDR ERROR [MSG: %s(%d)]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return iRes;
	}

	for(ifa=ifp; ifa; ifa=ifa->ifa_next)
	{
		socklen_t sizeSocketAddr;
		sizeSocketAddr = sizeof(struct sockaddr_in);

		if(getnameinfo(ifa->ifa_addr, sizeSocketAddr, strIP, sizeof(strIP), NULL, 0, NI_NUMERICHOST) < 0)
		{
			continue;
		}
		if(STRNCMP(strIP, STR_LOCAL_HOST_IP, STRLEN(STR_LOCAL_HOST_IP)) == 0)
		{
			continue;
		}

		//GRIB_LOGD("# IP ADDR : %s\n", strIP);
		STRINIT(pBuff, MAX_SIZE_IP_STR);
		STRNCPY(pBuff, strIP, STRLEN(strIP));
	}


	return GRIB_DONE;
}


char* Grib_TrimAll(char *pSource)
{
	int  i = 0;
	int  iDBG = FALSE;
	int  INDEX_TRIM  = 0;
	int  INDEX_SOURCE= 0;
	int  iStringSize = 0;
	int  iBlankCount = 0;

	char *pTrim = NULL;

	if(pSource == NULL)
	{
		GRIB_LOGD("# TRIM SOURCE IS NULL\n");
		return NULL;
	}

	iBlankCount = Grib_CountChar(pSource, GRIB_SPACE);
	if(iDBG)GRIB_LOGD("# TRIM: SPACE COUNT: %d\n", iBlankCount);

	iBlankCount += Grib_CountChar(pSource, GRIB_TAB);
	if(iDBG)GRIB_LOGD("# TRIM: SPACE+TAB COUNT: %d\n", iBlankCount);

	iStringSize = STRLEN(pSource);
	pTrim = (char *)MALLOC(iStringSize-iBlankCount+1);
	STRINIT(pTrim, iStringSize-iBlankCount+1);

	for(INDEX_SOURCE=0; INDEX_SOURCE<iStringSize; INDEX_SOURCE++)
	{
		if(isblank(INDEX_SOURCE[pSource]))
		{
			continue;
		}
		else
		{
			INDEX_TRIM[pTrim] = INDEX_SOURCE[pSource];
			INDEX_TRIM++;
		}
	}
	if(iDBG)GRIB_LOGD("# TRIMED: [%s]\n", pTrim);
	return pTrim;
}

int Grib_LoadConfig(Grib_ConfigInfo* pConfigInfo)
{
	int iRes = GRIB_DONE;
	int iDBG = FALSE;
	FILE* pConfigFile = NULL;
	char  pLineBuff[SIZE_1K] = {NULL, };
	char* pTemp = NULL;
	char* pTrim = NULL;
	char* pValue = NULL;

	if(iDBG)GRIB_LOGD("# LOAD CONFIG FILE START\n");

	pConfigFile = fopen(GRIB_CONFIG_FILE_PATH, "r");
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
		else if(*pTrim == NULL)
		{
			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_HUB_ID, STRLEN(GRIB_CONFIG_HUB_ID)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
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

		if(STRNCMP(pTrim, GRIB_CONFIG_PLATFORM_SERVER_IP, STRLEN(GRIB_CONFIG_PLATFORM_SERVER_IP)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->platformServerIP, sizeof(pConfigInfo->platformServerIP));
			STRNCPY(pConfigInfo->platformServerIP, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_PLATFORM_SERVER_PORT, STRLEN(GRIB_CONFIG_PLATFORM_SERVER_PORT)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->platformServerPort = ATOI(pValue);
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_MYSQL_DB_HOST, STRLEN(GRIB_CONFIG_MYSQL_DB_HOST)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->iotDbHost, sizeof(pConfigInfo->iotDbHost));
			STRNCPY(pConfigInfo->iotDbHost, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_MYSQL_DB_PORT, STRLEN(GRIB_CONFIG_MYSQL_DB_PORT)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->iotDbPort = ATOI(pValue);
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_MYSQL_DB_USER, STRLEN(GRIB_CONFIG_MYSQL_DB_USER)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->iotDbUser, sizeof(pConfigInfo->iotDbUser));
			STRNCPY(pConfigInfo->iotDbUser, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_MYSQL_DB_PASSWORD, STRLEN(GRIB_CONFIG_MYSQL_DB_PASSWORD)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->iotDbPswd, sizeof(pConfigInfo->iotDbPswd));
			STRNCPY(pConfigInfo->iotDbPswd, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_DEBUG_ONEM2M, STRLEN(GRIB_CONFIG_DEBUG_ONEM2M)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];
		
			if( STRNCASECMP(pValue, GRIB_BOOL_TO_STR(TRUE), STRLEN(GRIB_BOOL_TO_STR(TRUE))) == 0)
			{
				pConfigInfo->debugOneM2M = TRUE;
			}
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_DEBUG_BLE, STRLEN(GRIB_CONFIG_DEBUG_BLE)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			if( STRNCASECMP(pValue, GRIB_BOOL_TO_STR(TRUE), STRLEN(GRIB_BOOL_TO_STR(TRUE))) == 0)
			{
				pConfigInfo->debugBLE = TRUE;
			}
			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_DEBUG_THREAD, STRLEN(GRIB_CONFIG_DEBUG_THREAD)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			if( STRNCASECMP(pValue, GRIB_BOOL_TO_STR(TRUE), STRLEN(GRIB_BOOL_TO_STR(TRUE))) == 0)
			{
				pConfigInfo->debugThread = TRUE;
			}
			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_BLE_TOMBSTONE, STRLEN(GRIB_CONFIG_BLE_TOMBSTONE)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			if( STRNCASECMP(pValue, GRIB_BOOL_TO_STR(TRUE), STRLEN(GRIB_BOOL_TO_STR(TRUE))) == 0)
			{
				pConfigInfo->bleTombStone = TRUE;
			}
			continue;
		}

		FREE(pTrim);
		pTrim = NULL;
	}

	if(iDBG)
	{
		GRIB_LOGD("\n");
		GRIB_LOGD("# HUB_ID              : [%s]\n", pConfigInfo->hubID);
		GRIB_LOGD("# PLATFORM_SERVER_IP  : [%s]\n", pConfigInfo->platformServerIP);
		GRIB_LOGD("# PLATFORM_SERVER_PORT: [%d]\n", pConfigInfo->platformServerPort);
		GRIB_LOGD("# MYSQL_DB_HOST       : [%s]\n", pConfigInfo->iotDbHost);
		GRIB_LOGD("# MYSQL_DB_PORT       : [%d]\n", pConfigInfo->iotDbPort);
		GRIB_LOGD("# MYSQL_DB_USER       : [%s]\n", pConfigInfo->iotDbUser);
		GRIB_LOGD("# MYSQL_DB_PASSWORD   : [%s]\n", pConfigInfo->iotDbPswd);
		GRIB_LOGD("# GRIB_DEBUG_ONEM2M   : [%d]\n", pConfigInfo->debugOneM2M);
		GRIB_LOGD("# GRIB_DEBUG_BLE      : [%d]\n", pConfigInfo->debugBLE);
		GRIB_LOGD("# GRIB_DEBUG_THREAD   : [%d]\n", pConfigInfo->debugThread);
		GRIB_LOGD("# GRIB_BLE_TOMBSTONE  : [%d]\n", pConfigInfo->bleTombStone);
		GRIB_LOGD("\n");
	}

FINAL:
	if(iDBG)GRIB_LOGD("# LOAD CONFIG FINAL\n");

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

#define __UTIL_STRING_CONVERTER__
const char* Grib_ThreadStatusToStr(int iStatus)
{
    switch(iStatus)
	{
		case THREAD_STATUS_NONE:				return "NONE";
		case THREAD_STATUS_POLLING:				return "POLLING";
		case THREAD_STATUS_USE_BLE:				return "COMMAND";
		case THREAD_STATUS_NEED_ANSWER:			return "ANSWER";
		default:								return "NOT_DEFINE";
    }
}

const char* Grib_FuncAttrToStr(int iAttr)
{
    switch(iAttr)
	{
		case FUNC_ATTR_USE_CONTROL:				return "USE CONTROL";
		case FUNC_ATTR_USE_REPORT:				return "USE REPORT";
		case FUNC_ATTR_USE_ALL:					return "USE CONTROL & REPORT";
		default:								return "NOT_DEFINE";
    }
}

const char* Grib_InterfaceToStr(Grib_DeviceIfType iType)
{
    switch(iType)
	{
		case DEVICE_IF_TYPE_NONE:				return "NONE TYPE";
		case DEVICE_IF_TYPE_BLE:				return "BLE TYPE";
		case DEVICE_IF_TYPE_ZIGBEE:				return "ZIGBEE TYPE";
		case DEVICE_IF_TYPE_ZWAVE:				return "ZWAVE TYPE";
		default:								return "NOT_DEFINE";
    }
}

const char* Grib_BleErrorToStr(Grib_BleErrorCode iType)
{
    switch(iType)
	{
		case BLE_ERROR_CODE_NONE:				return "BLE_ERROR_CODE_NONE";
		case BLE_ERROR_CODE_BASE:				return "BLE_ERROR_CODE_BASE";
		case BLE_ERROR_CODE_INVALID_PARAM:		return "BLE_ERROR_CODE_INVALID_PARAM";
		case BLE_ERROR_CODE_CONNECT_FAIL:		return "BLE_ERROR_CODE_CONNECT_FAIL";
		case BLE_ERROR_CODE_SEND_FAIL:			return "BLE_ERROR_CODE_SEND_FAIL";
		case BLE_ERROR_CODE_RECV_FAIL:			return "BLE_ERROR_CODE_RECV_FAIL";
		case BLE_ERROR_CODE_INTERNAL:			return "BLE_ERROR_CODE_INTERNAL";
		case BLE_ERROR_CODE_INTERACTIVE:		return "BLE_ERROR_CODE_INTERACTIVE";
		case BLE_ERROR_CODE_CRITICAL:			return "BLE_ERROR_CODE_CRITICAL";
		default:								return "NOT_DEFINE";
    }
}

