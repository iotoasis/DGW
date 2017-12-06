/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_cas.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int gDebugCas = FALSE;

char casHubID[SIZE_1K];
char casSignCert[SIZE_1K*10];
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define __CAS_FUNC__
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_CasInit(char* hubID)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugCas;

	const char* FUNC = "CAS-INIT";
	char  pathCurrunt[PATH_MAX] = {'\0', };
	char  pathCasLib[PATH_MAX] = {'\0', };

	if(iDBG)GRIB_LOGD("# %s: START\n", FUNC);

	if(STRLEN(hubID) <= 1)
	{
		GRIB_LOGD("# %s: HUB ID IN-VALID !!!\n", FUNC);
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# %s: HUB ID: %s\n", FUNC, hubID);

	getcwd(pathCurrunt, sizeof(pathCurrunt));
	SNPRINTF(pathCasLib, sizeof(pathCasLib), "%s/%s", pathCurrunt, CAS_LIB_DIR_PATH);

	GRIB_LOGD("# %s: CAS HUB ID  : %s\n", FUNC, hubID);
	GRIB_LOGD("# %s: CAS LIB PATH: %s\n", FUNC, pathCasLib);

	iRes = TK_Init(pathCasLib);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: %c[1;31mTOOL KIT INIT ERROR !!!%c[0m\n", FUNC, 27, 27);
		return iRes;
	}

	iRes = TK_IssueCert(hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: %c[1;31mTOOL KIT GET CERT ERROR !!!%c[0m\n", FUNC, 27, 27);
		goto FINAL;
	}

	STRINIT(casHubID, sizeof(casHubID));
	STRNCPY(casHubID, hubID, STRLEN(hubID));

#if __NOT_USED__
	STRINIT(casSignCert, sizeof(casSignCert));
	iRes = Grib_CasGetSignCert(casSignCert);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: TOOL KIT INIT ERROR !!!\n", FUNC);
		goto FINAL;
	}
#endif

	GRIB_LOGD("# %s: %c[1;32mTOOL KIT INIT DONE ...%c[0m\n", FUNC, 27, 27);

	return GRIB_DONE;

FINAL:
	Grib_CasFinal();

	return iRes;
}

int Grib_CasFinal(void)
{
	int iRes = GRIB_ERROR;
	const char* FUNC = "CAS-FINAL";

	iRes = TK_Final();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: TOOL KIT FINAL ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	if(STRLEN(casHubID) != 0)
	{
		STRINIT(casHubID, sizeof(casHubID));
	}

	if(STRLEN(casSignCert) != 0)
	{
		STRINIT(casSignCert, sizeof(casSignCert));
	}

	GRIB_LOGD("# %s: TOOL KIT FINAL DONE\n", FUNC);

	return GRIB_DONE;
}


//##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
#define CAS_CERT_IGNORE_LF						ON
//##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

int Grib_CasGetSignCert(char* pSignCert)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugCas;
	const char* FUNC = "CAS-CERT";

	char pathCasCert[PATH_MAX] = {'\0', };
	char pBuff[SIZE_1K*128] = {'\0', };
	char* pathHome = NULL;

	if(STRLEN(casHubID) == 0)
	{
		GRIB_LOGD("# %s: TOOL KIT IS NOT INIT !!!\n", FUNC);
		return GRIB_ERROR;
	}

//	pathHome = getenv("HOME"); //shbaek: Fail to root run
	pathHome = "/home/pi";

	SNPRINTF(pathCasCert, sizeof(pathCasCert), "%s/%s/%s/%s", 
		pathHome, CAS_SIGN_CERT_FILE_DIR, casHubID, CAS_SIGN_CERT_FILE_NAME);

	GRIB_LOGD("# %s: PATH: %s\n", FUNC, pathCasCert);

#if (CAS_CERT_IGNORE_LF==ON)
	iRes = Grib_ReadTextFile(pathCasCert, pBuff, READ_OPT_IGNORE_LF);
#else
	iRes = Grib_ReadTextFile(pathCasCert, pBuff, GRIB_NOT_USED);
#endif
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: READ CERT FAIL !!!\n", FUNC);
		return iRes;
	}

	if(STRSTR(pBuff, CAS_SIGN_CERT_BEGIN) != NULL)
	{
		unsigned int SKIP_SIZE_HEAD = 0;
		unsigned int SKIP_SIZE_TAIL = 0;

#if (CAS_CERT_IGNORE_LF==ON)
		SKIP_SIZE_HEAD = STRLEN(CAS_SIGN_CERT_BEGIN);
		SKIP_SIZE_TAIL = STRLEN(CAS_SIGN_CERT_END);
#else
		//shbaek: for LF
		SKIP_SIZE_HEAD = STRLEN(CAS_SIGN_CERT_BEGIN)+1;
		SKIP_SIZE_TAIL = STRLEN(CAS_SIGN_CERT_END)+1;
#endif
		MEMCPY(pSignCert, pBuff+SKIP_SIZE_HEAD, STRLEN(pBuff)-(SKIP_SIZE_HEAD+SKIP_SIZE_TAIL));

	}
	if(iDBG)GRIB_LOGD("# %s: CERT[%d]: \n%s\n", FUNC, STRLEN(pSignCert), pSignCert);

	return iRes;
}

int Grib_CasGetAuthKey(char* devID, char* keyBuff)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugCas;
	const char* FUNC = "CAS-KEY";
	char *pAuthKey = NULL;

	if( (devID==NULL) || (keyBuff==NULL) )
	{
		GRIB_LOGD("# %s: PARAM IS NULL !!!\n", FUNC);
		return GRIB_ERROR;
	}

	if(STRLEN(casHubID) == 0)
	{
		Grib_ConfigInfo* pConfigInfo = NULL;
		
		GRIB_LOGD("# %s: TOOL KIT NOT INIT !!!\n", FUNC);

		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# %s: GET CONFIG ERROR !!!\n", FUNC);
			return GRIB_ERROR;
		}
		iRes = Grib_CasInit(pConfigInfo->hubID);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# %s: INIT FAIL !!!\n", FUNC);
			return GRIB_ERROR;
		}
	}

	if(iDBG)
	{
		GRIB_LOGD("# %s: HUB ID[%03d]: %s\n", FUNC, STRLEN(casHubID), casHubID);
		GRIB_LOGD("# %s: DEV ID[%03d]: %s\n", FUNC, STRLEN(devID), devID);
	}

	iRes = TK_Sign(casHubID, devID, &pAuthKey);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("\n");
		GRIB_LOGD("# %s: %c[1;31mTOOL KIT SIGN ERROR !!!%c[0m\n", FUNC, 27, 27);
		return GRIB_ERROR;
	}

	if(pAuthKey == NULL)
	{
		GRIB_LOGD("# %s: %c[1;31mAUTH KEY IS NULL ERROR !!!%c[0m\n", FUNC, 27, 27);
		return GRIB_ERROR;
	}

	MEMCPY(keyBuff, pAuthKey, STRLEN(pAuthKey));

	GRIB_LOGD("# %s: %c[1;32m%s -> %s%c[0m\n", FUNC, 27, devID, pAuthKey, 27);

	FREE(pAuthKey);

	return iRes;
}

void Grib_CasTest(int argc, char **argv)
{
	int iRes = GRIB_ERROR;
	const char* FUNC = "CAS-TEST";

	char* hubID = "TEST_HUB01";
	char* devID = "TEST_DEV01";

	char  keyBuff[GRIB_MAX_SIZE_AUTH_KEY] = {'\0', };

	Grib_ConfigInfo* pConfigInfo = NULL;

	GRIB_LOGD("# %s: START ##### ##### ##### ##### ##### #####\n", FUNC);

#ifdef FEATURE_CAS
	GRIB_LOGD("# %s: FEATURE_CAS ON!!!\n", FUNC);
#endif

	pConfigInfo = Grib_GetConfigInfo();

	if( (pConfigInfo!=NULL) && (0<STRLEN(pConfigInfo->hubID)) )
	{
		hubID = pConfigInfo->hubID;
	}

	if( (4<=argc) && (0<STRLEN(argv[3])) )
	{
		devID = argv[3];
	}

	iRes = Grib_CasInit(hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: INIT FAIL !!!\n", FUNC);
		return;
	}

	iRes = Grib_CasGetAuthKey(devID, keyBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: GET AUTH KEY FAIL !!!\n", FUNC);
		return;
	}
	GRIB_LOGD("# %s: AUTH KEY[%d]: %s\n", FUNC, STRLEN(keyBuff), keyBuff);

	GRIB_LOGD("# %s: DONE  ##### ##### ##### ##### ##### #####\n", FUNC);
	return;
}
