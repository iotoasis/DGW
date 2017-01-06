/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "grib_util.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
Grib_ConfigInfo gConfigInfo;

const char BASE64TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
char* Grib_Split(char* strSrc, char searchChar, int searchCount)
{
	unsigned int iCount = 0;

	unsigned int iTmpCount = 0;
	unsigned int iSrcCount = 0;
	unsigned int iMaxCount = 0;

	char tmpChar = '\0';
	char static sTmpBuff[SIZE_1M];

	STRINIT(sTmpBuff, sizeof(sTmpBuff));

	if(strSrc == NULL)
	{
		return NULL;
	}
	iMaxCount = STRLEN(strSrc);

	do
	{
		if(iMaxCount < iSrcCount)break;

		tmpChar = strSrc[iSrcCount];
		iSrcCount++;

		if(sizeof(sTmpBuff) <= iTmpCount)return NULL;
		sTmpBuff[iTmpCount] = tmpChar;
		iTmpCount++;

		if(tmpChar == searchChar)
		{
			if(iCount == (unsigned int)searchCount)break;
			iCount++;
			iTmpCount = 0;
		}
		
	}while(tmpChar != '\0');

	sTmpBuff[iTmpCount-1] = '\0';
	return sTmpBuff;
}

int Grib_CountChar(char* strSrc, char c)
{
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

int Grib_isHexString(char* strNum, int checkSize)
{
	int i = 0;
	int isHex = FALSE;

	if( (strNum==NULL) || (checkSize==0) )
	{
		return FALSE;
	}

	for(i=0; i<checkSize; i++)
	{
		isHex = isxdigit(strNum[i]);
		if(isHex == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}


long Grib_GetBase64EncodeSize(char* decData)
{
	float tmpSize = 0.0F;
	long  encSize = 0L;

	if(decData == NULL)
	{
		return -1;
	}

	tmpSize = STRLEN(decData) * sizeof(char)/3.0F;
	if((int)tmpSize < tmpSize) tmpSize = int(tmpSize)+1.0F;//shbaek: Round Off

	encSize = long(tmpSize*4)+1L;//shbaek: for NULL

	return encSize;
}

long Grib_GetBase64DecodeSize(char* encData)
{
	float tmpSize = 0.0F;
	long  decSize = 0L;

	if( (encData == NULL) || (STRLEN(encData)%4!=0) )
	{//shbaek: In-Valid Base64 Data Size
		return -1;
	}

	tmpSize = STRLEN(encData) * sizeof(char)/4.0F;
	decSize = long(tmpSize*3)+1L;//shbaek: for NULL

	return decSize;
}

int Grib_GetBase64Value(char data)
{
	int i = 0;

	if(data == BASE64_PAD)
	{
		return NULL;
	}

	for(i=0; i<sizeof(BASE64TABLE); i++)
	{
		if(data == BASE64TABLE[i])
		{
			return i;
		}
	}

	return GRIB_ERROR;
}

int Grib_Base64Encode(char* srcBuff, char* encBuff, int opt)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int srcSize = 0;
	int encSize = 0;

	char* pSrc = NULL;
	char  unit = '\0';

	if( (srcBuff==NULL) || (encBuff==NULL) )
	{
		GRIB_LOGD("# BASE64 ENC: PARAM NULL ERROR !!!\n");
		return GRIB_ERROR;		
	}

	srcSize = STRLEN(srcBuff);

	while( BASE64_ENC_SRC_BYTE <= (srcSize-encSize) )
	{
		pSrc = srcBuff + encSize;
		unit = '\0';

		//shbaek: 1st Unit Encode
		unit = (pSrc[0] >> 2) & BIT_MASK_6;
		encBuff[i] = BASE64TABLE[unit];
		i++;

		//shbaek: 2nd Unit Encode
		unit = ((pSrc[0] & BIT_MASK_2) << 4) + ((pSrc[1] >> 4) & BIT_MASK_4);
		encBuff[i] = BASE64TABLE[unit];
		i++;

		//shbaek: 3rd Unit Encode
		unit = ((pSrc[1] & BIT_MASK_4) << 2) + ((pSrc[2] >> 6) & BIT_MASK_2);
		encBuff[i] = BASE64TABLE[unit];
		i++;
		
		//shbaek: 4th Unit Encode
		unit = (pSrc[2] & BIT_MASK_6);
		encBuff[i] = BASE64TABLE[unit];
		i++;

		encSize += BASE64_ENC_SRC_BYTE;
	}

	if( (srcSize%BASE64_ENC_SRC_BYTE) == 2 )
	{
		pSrc = srcBuff + encSize;
		unit = '\0';

		//shbaek: 1st Unit Encode
		unit = (pSrc[0] >> 2) & BIT_MASK_6;
		encBuff[i] = BASE64TABLE[unit];
		i++;

		//shbaek: 2nd Unit Encode
		unit = ((pSrc[0] & BIT_MASK_2) << 4) + ((pSrc[1] >> 4) & BIT_MASK_4);
		encBuff[i] = BASE64TABLE[unit];
		i++;

		//shbaek: 3rd Unit Partial Encode
		unit = ((pSrc[1] & BIT_MASK_4) << 2);
		encBuff[i] = BASE64TABLE[unit];
		i++;

		//shbaek: 4th Unit Pad
		encBuff[i] = BASE64_PAD;
		i++;
	}

	if( (srcSize%BASE64_ENC_SRC_BYTE) == 1 )
	{
		pSrc = srcBuff + encSize;
		unit = '\0';

		//shbaek: 1st Unit Encode
		unit = (pSrc[0] >> 2) & BIT_MASK_6;
		encBuff[i] = BASE64TABLE[unit];
		i++;

		//shbaek: 2nd Unit Partial Encode
		unit = ((pSrc[0] & BIT_MASK_2) << 4);
		encBuff[i] = BASE64TABLE[unit];
		i++;

		//shbaek: 4th Unit Pad
		encBuff[i] = BASE64_PAD;
		i++;

		//shbaek: 4th Unit Pad
		encBuff[i] = BASE64_PAD;
		i++;
	}

	encBuff[i] = '\0';

	return GRIB_DONE;

}

int Grib_Base64Decode(char* srcBuff, char* decBuff, int opt)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int srcSize = 0;
	int decSize = 0;

	int unit1 = 0;
	int unit2 = 0;
	int unit3 = 0;
	int unit4 = 0;

	char* pSrc = NULL;

	if( (srcBuff==NULL) || (decBuff==NULL) )
	{
		GRIB_LOGD("# BASE64 DEC: PARAM NULL ERROR !!!\n");
		return GRIB_ERROR;		
	}

	srcSize = STRLEN(srcBuff);
	if(srcSize % BASE64_DEC_SRC_BYTE != 0)
	{//shbaek: In-Valid Base64 Data Size
		GRIB_LOGD("# BASE64 DEC: INVALID SOURCE SIZE !!!\n");
		return GRIB_ERROR;
	}

	while( BASE64_DEC_SRC_BYTE <= (srcSize-decSize) )
	{
		pSrc = srcBuff + decSize;

		unit1 = Grib_GetBase64Value(pSrc[0]);
		unit2 = Grib_GetBase64Value(pSrc[1]);
		unit3 = Grib_GetBase64Value(pSrc[2]);
		unit4 = Grib_GetBase64Value(pSrc[3]);

		//shbaek: 1st Data Decode
		decBuff[i] = (unit1 << 2) + (unit2 >> 4);
		i++;

		//shbaek: 2nd Data Decode
		decBuff[i] = (unit2 << 4) + (unit3 >> 2);
		i++;

		//shbaek: 3rd Data Decode
		decBuff[i] = (unit3 << 6) + unit4;
		i++;

		decSize += BASE64_DEC_SRC_BYTE;
	}

	decBuff[i] = '\0';

	return GRIB_DONE;
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

int systemCommand(const char *pCommand, char *pLineBuffer, int iBufferSize)
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
    pLineBuffer[readSize] = '\0';

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

	char  strIP[MAX_SIZE_IP_STR] = {'\0', };
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

int Grib_WriteTextFile(char* filePath, char* pBuff, char* opt)
{
	int i = 0;
	int iReadCount = 0;
	int iTotalCount = 0;
	int iCopyCount = 0;

	FILE *pFile = NULL;
	char  pLineBuff[SIZE_1K] = {'\0', };

	if( (STRLEN(filePath)<=0) || (pBuff==NULL) )
	{
		GRIB_LOGD("# INVALID PARAM\n");
		return GRIB_ERROR;
	}

	if(opt == GRIB_NOT_USED)
	{
		opt = "w";
	}

	pFile = fopen(filePath, opt);
	if(pFile == NULL)
	{
		GRIB_LOGD("# OPEN FILE FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}

	while(pBuff[i] != '\0')
	{
		if(fputc(pBuff[i], pFile) == EOF)
		{
			GRIB_LOGD("# WRITE COUNT: %d, CHAR: %c\n", i, pBuff[i]);
			GRIB_LOGD("# WRITE FILE FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
			return GRIB_ERROR;
		}
		i++;
	}
	fclose(pFile);

	GRIB_LOGD("# WRITE TOTAL COUNT: %d\n", i);

	return GRIB_DONE;
}

int Grib_ReadTextFile(char* filePath, char* pBuff, int opt)
{
	int iReadCount = 0;
	int iTotalCount = 0;
	int iCopyCount = 0;

	FILE *pFile = NULL;
	char  pLineBuff[SIZE_1K] = {'\0', };

	if( (STRLEN(filePath)<=0) || (pBuff==NULL) )
	{
		GRIB_LOGD("# INVALID PARAM\n");
		return GRIB_ERROR;
	}

	pFile = fopen(filePath, "r");
	if(pFile == NULL)
	{
		GRIB_LOGD("# OPEN FILE FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}

	while(!feof(pFile))
	{
		STRINIT(pLineBuff, sizeof(pLineBuff));

		if( fgets(pLineBuff, sizeof(pLineBuff)-1, pFile) == NULL )
		{
			continue;
		}
		iCopyCount = STRLEN(pLineBuff);

		if(opt == READ_OPT_IGNORE_LF)iCopyCount -= 1;//shbaek: for LF

		MEMCPY(pBuff+iTotalCount, pLineBuff, iCopyCount);
		iTotalCount += iCopyCount;
	}

	fclose(pFile);

	return GRIB_DONE;
}
Grib_ConfigInfo* Grib_GetConfigInfo(void)
{
	int iRes = GRIB_ERROR;
	int LOOP = 10;

	if(gConfigInfo.isLoad)
	{
		return &gConfigInfo;
	}

LOAD_CONFIG:
	LOOP--;
	iRes = Grib_LoadConfig(&gConfigInfo);
	if(iRes != GRIB_DONE)
	{
		SLEEP(1);
		if(0<LOOP)goto LOAD_CONFIG;
		return NULL;
	}

	return &gConfigInfo;
}

int Grib_LoadConfig(Grib_ConfigInfo* pConfigInfo)
{
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
		else if(*pTrim == '\0')
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

		//shbaek: Platform
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

		//shbaek: Authentication
		if(STRNCMP(pTrim, GRIB_CONFIG_AUTH_SERVER_IP, STRLEN(GRIB_CONFIG_AUTH_SERVER_IP)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
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
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
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
		if(STRNCMP(pTrim, GRIB_CONFIG_SDA_SERVER_IP, STRLEN(GRIB_CONFIG_SDA_SERVER_IP)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			STRINIT(pConfigInfo->sdaServerIP, sizeof(pConfigInfo->sdaServerIP));
			STRNCPY(pConfigInfo->sdaServerIP, pValue, STRLEN(pValue));
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_SDA_SERVER_PORT, STRLEN(GRIB_CONFIG_SDA_SERVER_PORT)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
			if(pTemp == NULL)
			{
				GRIB_LOGD("# THIS LINE NOT \":\" EXIST\n");
				iRes = GRIB_ERROR;
				continue;				
			}
			pValue = &pTemp[1];

			pConfigInfo->sdaServerPort = ATOI(pValue);
			continue;
		}

		//shbaek: MySQL
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

		//shbaek: Reset
		if(STRNCMP(pTrim, GRIB_CONFIG_RESET_TIMER_USE, STRLEN(GRIB_CONFIG_RESET_TIMER_USE)) == 0)
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
				pConfigInfo->resetTimerSwitch = TRUE;
			}
			else
			{
				pConfigInfo->resetTimerSwitch = FALSE;
			}
			continue;
		}
		
		if(STRNCMP(pTrim, GRIB_CONFIG_RESET_TIME_HOUR, STRLEN(GRIB_CONFIG_RESET_TIME_HOUR)) == 0)
		{
			pTemp = STRSTR(pTrim, GRIB_CONFIG_SEPARATOR);
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

		//shbaek: ETC
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
			else
			{
				pConfigInfo->debugOneM2M = FALSE;
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
			else
			{
				pConfigInfo->debugBLE = FALSE;
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
			else
			{
				pConfigInfo->debugThread = FALSE;
			}

			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_TOMBSTONE_BLE, STRLEN(GRIB_CONFIG_TOMBSTONE_BLE)) == 0)
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
				pConfigInfo->tombStoneBLE = TRUE;
			}
			else
			{
				pConfigInfo->tombStoneBLE = FALSE;
			}

			continue;
		}

		if(STRNCMP(pTrim, GRIB_CONFIG_TOMBSTONE_HTTP, STRLEN(GRIB_CONFIG_TOMBSTONE_HTTP)) == 0)
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
				pConfigInfo->tombStoneHTTP = TRUE;
			}
			else
			{
				pConfigInfo->tombStoneHTTP = FALSE;
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
		GRIB_LOGD("# RESET_TIMER_USE     : [%d]\n", pConfigInfo->resetTimerSwitch);
		GRIB_LOGD("# RESET_TIME_HOUR     : [%d]\n", pConfigInfo->resetTimeHour);
		GRIB_LOGD("# GRIB_DEBUG_ONEM2M   : [%d]\n", pConfigInfo->debugOneM2M);
		GRIB_LOGD("# GRIB_DEBUG_BLE      : [%d]\n", pConfigInfo->debugBLE);
		GRIB_LOGD("# GRIB_DEBUG_THREAD   : [%d]\n", pConfigInfo->debugThread);
		GRIB_LOGD("# GRIB_TOMBSTONE_BLE  : [%d]\n", pConfigInfo->tombStoneBLE);
		GRIB_LOGD("# GRIB_TOMBSTONE_HTTP : [%d]\n", pConfigInfo->tombStoneHTTP);
		GRIB_LOGD("\n");
	}

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

	pConfigInfo->isLoad = TRUE;

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
		case BLE_ERROR_CODE_GENERIC:			return "BLE_ERROR_CODE_GENERIC";
		case BLE_ERROR_CODE_INVALID_COMMAND:	return "BLE_ERROR_CODE_INVALID_COMMAND";
		case BLE_ERROR_CODE_SENSOR:				return "BLE_ERROR_CODE_SENSOR";
		case BLE_ERROR_CODE_BLE:				return "BLE_ERROR_CODE_BLE";
		case BLE_ERROR_CODE_NOT_YET_IMPLEMENT:	return "BLE_ERROR_CODE_NOT_YET_IMPLEMENT";
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


int Grib_RandNum(int iMin, int iRange)
{
	return ( iMin + (rand()%iRange) );
}

void Grib_PrintHex(const char* LABEL, char* pHexBuff, int iSize)
{
	int i = 0;

	GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
	GRIB_LOGD("# %s[%d]:", LABEL, iSize);

	for(i=0; i<iSize; i++)
	{
		if(i%10 == 0)GRIB_LOGD("\n");
		GRIB_LOGD("0x%02X ", pHexBuff[i]);
	}
	GRIB_LOGD("\n");
	GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");

	return;
}

void Grib_PrintOnlyHex(char* pHexBuff, int iSize)
{
	int i = 0;

	for(i=0; i<iSize; i++)
	{
		if(i%10 == 0)
		{
			if(i != 0)GRIB_LOGD("\n");
			GRIB_LOGD("  ");
		}

		GRIB_LOGD("0x%02X ", pHexBuff[i]);
		if(i==iSize-1)GRIB_LOGD("\n");
	}

	return;
}

long Grib_GetStackLimit(void)
{
	struct rlimit limit;

	getrlimit (RLIMIT_STACK, &limit);
	printf ("# STACK LIMIT: %ld / %ld\n", limit.rlim_cur, limit.rlim_max);

	return limit.rlim_cur;
}
