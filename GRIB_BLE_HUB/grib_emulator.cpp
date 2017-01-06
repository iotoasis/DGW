/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Include File
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_define.h"
#include "grib_util.h"

#include "grib_onem2m.h"

#include "grib_sda.h"
#include "grib_auth.h"

#ifdef FEATURE_CAS
#include "grib_cas.h"
#endif

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define GRIB_EMULATOR_SEPARATOR							","

#define AUTH_REGI_TYPE_NONE								0
#define AUTH_REGI_TYPE_GATEWAY							1
#define AUTH_REGI_TYPE_DEVICE								2


typedef struct
{
	int  deviceType;
	char deviceID[GRIB_MAX_SIZE_MIDDLE];
	char devicePW[GRIB_MAX_SIZE_MIDDLE];
}GribEmul_AuthInfo;

typedef struct
{
	char aeName[GRIB_MAX_SIZE_MIDDLE];
	char funcName[GRIB_MAX_SIZE_MIDDLE];
}GribEmul_AeInfo;

typedef struct
{
	char origin[GRIB_MAX_SIZE_MIDDLE];
	char uri[GRIB_MAX_SIZE_LONG];
	int  maxValue;
}GribEmul_ReportInfo;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
char GRIB_PATH[MAXPATHLEN];

void GribEmul_MenuHelp(void)
{
	GRIB_LOGD("\n");
	GRIB_LOGD("# ########## ########## ########## GRIB MENU ########## ########## ##########\n");
	GRIB_LOGD("# grib_emulator auth \"FILE PATH\"\n");
	GRIB_LOGD("# grib_emulator ae \"FILE PATH\"\n");
	GRIB_LOGD("# grib_emulator report \"FILE PATH\"\n");
	GRIB_LOGD("# ########## ########## ########## ######### ########## ########## ##########\n");
	GRIB_LOGD("\n");
	return;
}

void GribEmul_MenuAuth(int argc, char **argv)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	int iTotal = 0;
	int iFailCount = 0;

	FILE* pDataFile = NULL;
	char* dataFilePath = NULL;
	char  pLineBuff[SIZE_1K] = {'\0', };
	char* pTemp = NULL;
	char* pTrim = NULL;
	char* pDeviceID = NULL;
	char* pDevicePW = NULL;

	GribEmul_AuthInfo authList[SIZE_1K];

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# NO DATA FILE !!!\n");
	}

	dataFilePath = GRIB_CMD_SUB[argv];
	pDataFile = fopen(dataFilePath, "r");
	if(pDataFile == NULL)
	{
		GRIB_LOGD("# DATA FILE OPEN FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return;
	}
	
	while(!feof(pDataFile))
	{
		STRINIT(pLineBuff, sizeof(pLineBuff));
		MEMSET(&authList[i], 0x00, sizeof(GribEmul_AuthInfo));

		if( fgets(pLineBuff, sizeof(pLineBuff)-1, pDataFile) == NULL )
		{
			continue;
		}

		if(iDBG)GRIB_LOGD("# LINE: %s\n", pLineBuff);

		pLineBuff[STRLEN(pLineBuff)-1] = '\0';
		pTrim = Grib_TrimAll(pLineBuff);
		if(pTrim == NULL)continue;

		if(*pTrim == GRIB_HASH)
		{//shbaek: It's Comment
			continue;
		}
		else if(*pTrim == '\0')
		{//shbaek: No Data
			continue;
		}

		if(STRLEN(pLineBuff) <= 1)
		{//shbaek: Too Short?
			continue;
		}

		pTemp = STRSTR(pTrim, GRIB_EMULATOR_SEPARATOR);
		if(pTemp == NULL)
		{//shbaek: GateWay Type Regi
			authList[i].deviceType = AUTH_REGI_TYPE_GATEWAY;
			STRNCPY(authList[i].deviceID, pTrim, STRLEN(pTrim));
		}
		else
		{//shbaek: Device Type Regi
			pDevicePW = &pTemp[1];
			*pTemp = '\0';
			pDeviceID = pTrim;

			authList[i].deviceType = AUTH_REGI_TYPE_DEVICE;
			STRNCPY(authList[i].deviceID, pDeviceID, STRLEN(pDeviceID));
			STRNCPY(authList[i].devicePW, pDevicePW, STRLEN(pDevicePW));
		}

		if(iDBG)GRIB_LOGD("# LIST[%d] [ID: %s] [TYPE: %d] [PW: %s]\n", i, authList[i].deviceID, authList[i].deviceType, authList[i].devicePW);

		if((sizeof(authList)/sizeof(GribEmul_AuthInfo)) < i)break; //shbaek: Overflow
		i++;
	}
	iTotal = i;
	GRIB_LOGD("# LIST TOTAL COUNT: %d\n\n", iTotal);

	for(i=0; i<iTotal; i++)
	{
		if(authList[i].deviceType == AUTH_REGI_TYPE_GATEWAY)
		{
			iRes = Grib_AuthGatewayRegi(authList[i].deviceID);
		}
		else if(authList[i].deviceType == AUTH_REGI_TYPE_DEVICE)
		{
			iRes = Grib_AuthDeviceRegi(authList[i].deviceID, authList[i].devicePW);
		}
		if(iRes != GRIB_DONE)iFailCount++;

		GRIB_LOGD("# LIST[%d/%d] REGI: %s : %s \n\n", i, iTotal-1, authList[i].deviceID, iRes==GRIB_DONE?"DONE":"FAIL");
		SLEEP(1);
	}

	GRIB_LOGD("# LIST TOTAL COUNT: %d\n", iTotal);
	GRIB_LOGD("# LIST ERROR COUNT: %d\n\n", iFailCount);

	return;
}

void GribEmul_MenuAE(int argc, char **argv)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	int iTotal = 0;
	int iFailCount = 0;

	FILE* pDataFile = NULL;
	char* dataFilePath = NULL;
	char  pLineBuff[SIZE_1K] = {'\0', };
	char* pTemp = NULL;
	char* pTrim = NULL;
	char* pAeName = NULL;
	char* pFuncName = NULL;

	Grib_DbRowDeviceInfo  rowDeviceInfo;
	Grib_DbRowDeviceFunc* pRowDeviceFunc;

#ifdef FEATURE_CAS
	char pAuthKey[CAS_AUTH_KEY_SIZE] = {'\0', };
#endif

	GribEmul_AeInfo aeList[SIZE_1K];

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# NO DATA FILE !!!\n");
	}

	dataFilePath = GRIB_CMD_SUB[argv];
	pDataFile = fopen(dataFilePath, "r");
	if(pDataFile == NULL)
	{
		GRIB_LOGD("# DATA FILE OPEN FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return;
	}
	
	while(!feof(pDataFile))
	{
		STRINIT(pLineBuff, sizeof(pLineBuff));
		MEMSET(&aeList[i], 0x00, sizeof(GribEmul_AeInfo));

		if( fgets(pLineBuff, sizeof(pLineBuff)-1, pDataFile) == NULL )
		{
			continue;
		}

		if(iDBG)GRIB_LOGD("# LINE: %s\n", pLineBuff);

		pLineBuff[STRLEN(pLineBuff)-1] = '\0';
		pTrim = Grib_TrimAll(pLineBuff);
		if(pTrim == NULL)continue;

		if(*pTrim == GRIB_HASH)
		{//shbaek: It's Comment
			continue;
		}
		else if(*pTrim == '\0')
		{//shbaek: No Data
			continue;
		}

		if(STRLEN(pLineBuff) <= 1)
		{//shbaek: Too Short?
			continue;
		}

		pTemp = STRSTR(pTrim, GRIB_EMULATOR_SEPARATOR);
		if(pTemp == NULL)
		{//shbaek: In-Valid Data
			continue;
		}

		pFuncName = &pTemp[1];
		*pTemp = '\0';
		pAeName = pTrim;

		STRNCPY(aeList[i].aeName, pAeName, STRLEN(pAeName));
		STRNCPY(aeList[i].funcName, pFuncName, STRLEN(pFuncName));

		if(iDBG)GRIB_LOGD("# LIST[%d] [AE: %s][FUNC: %s]\n", i, aeList[i].aeName, aeList[i].funcName);

		if((sizeof(aeList)/sizeof(GribEmul_AeInfo)) < i)break; //shbaek: Overflow
		i++;
	}
	iTotal = i;
	GRIB_LOGD("# LIST TOTAL COUNT: %d\n\n", iTotal);

	rowDeviceInfo.deviceFuncCount = 1;
	rowDeviceInfo.ppRowDeviceFunc = &pRowDeviceFunc;
	pRowDeviceFunc = (Grib_DbRowDeviceFunc*)MALLOC(sizeof(Grib_DbRowDeviceFunc));

	for(i=0; i<iTotal; i++)
	{
		STRINIT(rowDeviceInfo.deviceID, sizeof(rowDeviceInfo.deviceID));
		STRINIT(pRowDeviceFunc->funcName, sizeof(pRowDeviceFunc->funcName));

		STRNCPY(rowDeviceInfo.deviceID, aeList[i].aeName, STRLEN(aeList[i].aeName));
		STRNCPY(pRowDeviceFunc->funcName, aeList[i].funcName, STRLEN(aeList[i].funcName));

#ifdef FEATURE_CAS
		iRes = Grib_AuthGetPW(rowDeviceInfo.deviceID, pAuthKey);
		if(iRes == GRIB_DONE)
		{
			iRes = Grib_CreateOneM2MTree(&rowDeviceInfo, pAuthKey);
		}
#else
		iRes = Grib_CreateOneM2MTree(&rowDeviceInfo);
#endif

		if(iRes != GRIB_DONE)iFailCount++;

		GRIB_LOGD("# LIST[%d/%d] AE: %s : %s \n\n", i, iTotal-1, aeList[i].aeName, (iRes==GRIB_DONE)?"DONE":"FAIL");
		SLEEP(1);
	}

	if(pRowDeviceFunc)FREE(pRowDeviceFunc);

	GRIB_LOGD("# LIST TOTAL COUNT: %d\n", iTotal);
	GRIB_LOGD("# LIST ERROR COUNT: %d\n\n", iFailCount);

	return;
}

void GribEmul_MenuReport(int argc, char **argv)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;
	int iTotal = 0;
	int iContentValue = 0;
	int isFixValue = FALSE;
	int iFailCount = 0;
	int iLoopCount = 0;

	FILE* pDataFile = NULL;
	char* dataFilePath = NULL;
	char  pLineBuff[SIZE_1K] = {'\0', };
	char* pTemp = NULL;
	char* pTrim = NULL;
	char* pOrigin = NULL;
	char* pUri = NULL;
	char* pMaxValue = NULL;

#ifdef FEATURE_CAS
	char pAuthKey[CAS_AUTH_KEY_SIZE] = {'\0', };
#endif

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	GribEmul_ReportInfo reportList[SIZE_1K];

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# NO DATA FILE !!!\n");
		return;
	}

	dataFilePath = GRIB_CMD_SUB[argv];
	pDataFile = fopen(dataFilePath, "r");
	if(pDataFile == NULL)
	{
		GRIB_LOGD("# DATA FILE OPEN FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return;
	}

	if( (GRIB_CMD_ARG1+1 < argc) && (STRCASECMP(GRIB_CMD_ARG1[argv], "FIX")==0) )
	{
		isFixValue = TRUE;
		GRIB_LOGD("# REPORT CONTENT VALUE IS FIXED !!!\n");
	}

	while(!feof(pDataFile))
	{
		STRINIT(pLineBuff, sizeof(pLineBuff));
		MEMSET(&reportList[i], 0x00, sizeof(GribEmul_AuthInfo));

		if( fgets(pLineBuff, sizeof(pLineBuff)-1, pDataFile) == NULL )
		{
			continue;
		}

		if(iDBG)GRIB_LOGD("# LINE: %s\n", pLineBuff);

		pLineBuff[STRLEN(pLineBuff)-1] = '\0';
		pTrim = Grib_TrimAll(pLineBuff);
		if(pTrim == NULL)continue;

		if(*pTrim == GRIB_HASH)
		{//shbaek: It's Comment
			continue;
		}
		else if(*pTrim == '\0')
		{//shbaek: No Data
			continue;
		}

		if(STRLEN(pLineBuff) <= 1)
		{//shbaek: Too Short?
			continue;
		}

		pTemp = STRSTR(pTrim, GRIB_EMULATOR_SEPARATOR);
		if(pTemp == NULL)
		{//shbaek: In-Valid Data
			continue;
		}

		pOrigin = pTrim;
		*pTemp = '\0';
		pTrim = &pTemp[1];

		pTemp = STRSTR(pTrim, GRIB_EMULATOR_SEPARATOR);
		if(pTemp == NULL)
		{//shbaek: In-Valid Data
			continue;
		}

		pUri = pTrim;
		*pTemp = '\0';
		pMaxValue = &pTemp[1];

		reportList[i].maxValue = ATOI(pMaxValue);
		STRNCPY(reportList[i].origin, pOrigin, STRLEN(pOrigin));
		STRNCPY(reportList[i].uri, pUri, STRLEN(pUri));

		if(iDBG)GRIB_LOGD("# LIST[%d] [ID: %s] [URI: %s] [MAX: %d]\n", i, 
				reportList[i].origin, reportList[i].uri, reportList[i].maxValue);

		if((sizeof(reportList)/sizeof(GribEmul_ReportInfo)) < i)break; //shbaek: Overflow
		i++;
	}
	iTotal = i;
	GRIB_LOGD("# LIST TOTAL COUNT: %d\n\n", iTotal);

LOOP:
	iLoopCount++;
	for(i=0; i<iTotal; i++)
	{
		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, reportList[i].origin, STRLEN(reportList[i].origin));

		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		SNPRINTF(&reqParam.xM2M_URI, sizeof(reportList[i].uri), "%s", reportList[i].uri);

		STRINIT(&reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF));
		SNPRINTF(&reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF), "%s:0", HTTP_CONTENT_TYPE_TEXT);

		if(isFixValue)iContentValue = reportList[i].maxValue;
		else
		{
			iContentValue = Grib_RandNum(0, reportList[i].maxValue+1);
		}

		STRINIT(&reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON));
		SNPRINTF(&reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON), "%d", iContentValue);

#ifdef FEATURE_CAS
		Grib_AuthGetPW(reportList[i].origin, pAuthKey);
		reqParam.authKey = pAuthKey;
#endif

		//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_CNF[If NULL, Set Default "text/plain:0"], xM2M_CON
		iRes = Grib_ContentInstanceCreate(&reqParam, &resParam);
		if(iRes != GRIB_DONE)iFailCount++;

		GRIB_LOGD("# LIST[%d/%d] REPORT: %s [CON:%d]: %s \n\n", i, iTotal-1, reportList[i].uri, iContentValue, (iRes==GRIB_DONE)?"DONE":"FAIL");
		SLEEP(1);
	}

	GRIB_LOGD("# [LOOP: %d]LIST TOTAL COUNT: %d\n", iLoopCount, iTotal);
	GRIB_LOGD("# [LOOP: %d]LIST ERROR COUNT: %d\n\n", iLoopCount, iFailCount);

	goto LOOP;

	return;
}

int main(int argc, char **argv)
{
	char* mainMenu = NULL;

	if(argc < GRIB_CMD_MAIN+1)
	{
		GribEmul_MenuHelp();
		return GRIB_DONE;
	}
	mainMenu = GRIB_CMD_MAIN[argv];

	realpath(argv[0], GRIB_PATH);

	if(FALSE)
	{
		GRIB_LOGD("# GRIB_PATH: %s\n", GRIB_PATH);
		for(int i=0; i<argc; i++)GRIB_LOGD("# ARG[%d/%d]: %s\n", i, argc, argv[i]);
	}

	if(STRCASECMP(mainMenu, "auth") == 0)
	{
		GribEmul_MenuAuth(argc, argv);
		return GRIB_DONE;
	}

	if(STRCASECMP(mainMenu, "ae") == 0)
	{
		GribEmul_MenuAE(argc, argv);
		return GRIB_DONE;
	}

	if(STRCASECMP(mainMenu, "report") == 0)
	{
		GribEmul_MenuReport(argc, argv);
		return GRIB_DONE;
	}

	GRIB_LOGD("# INVALID MENU\n");
	for(int i=0; i<argc; i++)GRIB_LOGD("# ARG[%d/%d]: %s\n", i, argc, argv[i]);
	GribEmul_MenuHelp();

	return GRIB_ERROR;
}
