/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "grib_util.h"
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

const char BASE64TABLE[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=";

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_DoubleFree(void** ppMem, int count)
{
	int i = 0;

	if(count < 1)
	{
		//GRIB_LOGD("# DOUBLE FREE: [COUNT:%d]\n", count);
		if(ppMem!=NULL)FREE(ppMem);

		return GRIB_ERROR;
	}

	for(i=0; i<count; i++)
	{
		if(ppMem[i] != NULL)
		{
			FREE(ppMem[i]);
			ppMem[i] = NULL;
		}
	}

	FREE(ppMem);
	return GRIB_DONE;
}

char* Grib_Split(char* strSrc, char searchChar, int searchCount)
{
	unsigned int iCount = 0;

	unsigned int iTmpCount = 0;
	unsigned int iSrcCount = 0;
	unsigned int iMaxCount = 0;

	char tmpChar = '\0';
	char static sTmpBuff[SIZE_1M];

	STRINIT(sTmpBuff, sizeof(sTmpBuff));

	if( (strSrc==NULL) || (STRLEN(strSrc)==0) )
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

	if(strStartsWith(strNum, "0x"))
	{//shbaek: Ignore Hex Prefix ...
		i = 2;
	}

	for(i; i<checkSize; i++)
	{
		isHex = isxdigit(strNum[i]);
		if(isHex == FALSE)
		{
			return FALSE;
		}
	}

	return TRUE;
}


int Grib_isNumString(char* strNum)
{
	int i = 0;
	int isNum = FALSE;

	if( (strNum==NULL) )
	{
		return FALSE;
	}

	for(i=0; i<strlen(strNum); i++)
	{
		isNum = isdigit(strNum[i]);
		if(isNum == FALSE)
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
	if( (line==NULL) || (prefix==NULL) )
	{
		return 0;
	}

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

void systemReboot(int waitSec, char* msg)
{
	int  i = 0;
	char pBuff[GRIB_MAX_SIZE_BRIEF] = {'\0', };
	const char* REBOOT_COMMAND = "sudo reboot --reboot --force";

	char  logFile[SIZE_1K] = {'\0', };
	char  logPath[SIZE_1K] = {'\0', };
	char  logTime[GRIB_MAX_SIZE_TIME_STR] = {'\0', };

	char* pOption = "w";

	if(msg == NULL)msg = "Don't Ask Me ...";
	Grib_GetCurrDateTime(logTime);
	SNPRINTF(logFile, sizeof(logFile), "REBOOT_%s.log", logTime);
	SNPRINTF(logPath, sizeof(logPath), "%s/%s", GRIB_FILE_PATH_LOG_ROOT, logFile);

	Grib_WriteTextFile(logPath, msg, pOption);
	sync();

	for(i=waitSec; 0<i; i--)
	{
		GRIB_LOGD("# SYSTEM RE-BOOT: %c[1;33mCOUNT DOWN: %d%c[0m\n", 27, i, 27);
		SLEEP(1);
	}

	GRIB_LOGD("# %c[1;33mSYSTEM RE-BOOT !!!%c[0m\n", 27, 27);
	systemCommand(REBOOT_COMMAND, pBuff, sizeof(pBuff));

    return;
}


int Grib_GetCurrDateTime(char* pBuff)
{
	time_t sysTimer;
	TimeInfo *sysTime;

	if(pBuff == NULL)
	{
		GRIB_LOGD("# PARAM IS NULL ERROR !!!\n");
		return GRIB_ERROR;
	}

	sysTimer = time(NULL);
	sysTime  = localtime(&sysTimer);

	STRINIT(pBuff, GRIB_MAX_SIZE_TIME_STR);
	SNPRINTF(pBuff, GRIB_MAX_SIZE_TIME_STR, GRIB_STR_TIME_FORMAT, 
			sysTime->tm_year+1900, sysTime->tm_mon+1, sysTime->tm_mday,
			sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);

	return GRIB_DONE;
}

void Grib_ShowCurrDateTime(void)
{
	char tempTime[GRIB_MAX_SIZE_TIME_STR] = {'\0', };

	Grib_GetCurrDateTime(tempTime);
	GRIB_LOGD("# CURRENT TIME: %c[1;33m%s%c[0m\n", 27, tempTime, 27);

	return;
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

int Grib_GetDnsIP(char* domain, char** ip)
{
	const char* FUNC = "GET-DNS_IP";
	const int	iDBG = FALSE;

	int iRes = GRIB_ERROR;
	int i = 0;

	hostent* pHost = NULL;

	if( (!domain) || (!ip) )
	{
		GRIB_LOGD("# %s: PARAM IS NULL !!!", FUNC);
		return GRIB_ERROR;
	}

	pHost = gethostbyname(domain);
	if(!pHost)
	{
		GRIB_LOGD("# %s: GET HOST NAME FAIL !!!", FUNC);
		return GRIB_ERROR;
	}

	for(i=0; pHost->h_addr_list[i]!=NULL; i++)
	{
		const char* ipAddr = inet_ntoa( *(struct in_addr*)pHost->h_addr_list[i]);

		if(0<STRLEN(ipAddr))
		{
			*ip = STRDUP(ipAddr);
			if(iDBG)GRIB_LOGD("# %s: HOST[%02d] [NAME:%s] [ADDR:%s]\n", FUNC, i, pHost->h_name, *ip);
			break;
		}
	}

	return iRes;
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

void Grib_MemLog(char* logDir)
{
	int   iRes = GRIB_ERROR;

	char  logFile[SIZE_1K] = {'\0', };
	char  logPath[SIZE_1K] = {'\0', };
	char  logTime[GRIB_MAX_SIZE_TIME_STR] = {'\0', };

	char* pCommand = "free -b";
	char  pBuffer[SIZE_1M] = {'\0', };
	char* pOption = "w";

	if(logDir == NULL)return;

	iRes = systemCommand(pCommand, pBuffer, sizeof(pBuffer));
	if(iRes != GRIB_DONE)return;

	Grib_GetCurrDateTime(logTime);

	SNPRINTF(logFile, sizeof(logFile), "MEM_%s.log", logTime);
	SNPRINTF(logPath, sizeof(logPath), "%s/%s", logDir, logFile);

	Grib_WriteTextFile(logPath, pBuffer, pOption);

}



#define __UTIL_STRING_CONVERTER__
const char* Grib_ThreadStatusToStr(int iStatus)
{
    switch(iStatus)
	{
		case THREAD_STATUS_NONE:				return "NONE";
		case THREAD_STATUS_POLLING:				return "POLLING";
		case THREAD_STATUS_USE_BLE:				return "USE_BLE";
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
		case BLE_ERROR_CODE_GET_HANDLE:			return "BLE_ERROR_CODE_GET_HANDLE";
		case BLE_ERROR_CODE_READ_TIMEOUT:		return "BLE_ERROR_CODE_READ_TIMEOUT";

		case BLE_ERROR_CODE_CRITICAL:			return "BLE_ERROR_CODE_CRITICAL";
		default:								return "NOT_DEFINE";
    }
}


int Grib_RandNum(int iMin, int iRange)
{
	return ( iMin + (rand()%iRange) );
}

//shbaek: "TEST+" -> "544553542B"
int Grib_StrToHex(char* strBuff, char* hexBuff)
{
	int i = 0;

	for(i=0; i<strlen(strBuff); i++)
	{
		sprintf(hexBuff+i*2, "%02X", *(strBuff+i));
	}

	return i;
}

//shbaek: "114D" (4Byte String) -> 0x11 0x4D (2Byte Binary)
int Grib_HexToBin(char* hexBuff, char* binBuff, int strSize)
{
	int i = 0;
	int pos = 0;
	char strBin[3] = {'\0', };

	for(i=0; i+1<strSize; i+=2)
	{
		memset(strBin, '\0', sizeof(strBin));
		strBin[0] = hexBuff[i+0];
		strBin[1] = hexBuff[i+1];
		strBin[2] = '\0';

		binBuff[(i/2)] = strtol(strBin, GRIB_NOT_USED, 16);		
	}

	return i/2;
}

long Grib_GetStackLimit(void)
{
	struct rlimit limit;

	getrlimit(RLIMIT_DATA, &limit);
	printf("# RLIMIT_DATA : %ld / %ld\n", limit.rlim_cur, limit.rlim_max);

	getrlimit(RLIMIT_STACK, &limit);
	printf("# RLIMIT_STACK: %ld / %ld\n", limit.rlim_cur, limit.rlim_max);

	return limit.rlim_cur;
}
