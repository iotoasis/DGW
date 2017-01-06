/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <pthread.h>

#include "grib_define.h"
#include "grib_onem2m.h"

#include "grib_db.h"
#include "grib_ble.h"
#include "grib_regi.h"
#include "grib_thread.h"
#include "grib_sda.h"
#include "grib_auth.h"

#ifdef FEATURE_CAS
#include "grib_cas.h"
#endif

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

void Grib_MenuHelp(void)
{
	GRIB_LOGD("\n");
	GRIB_LOGD("# ########## ########## ########## GRIB MENU ########## ########## ##########\n");
//	GRIB_LOGD("#\n");
	GRIB_LOGD("# grib version\n");
	GRIB_LOGD("# grib hub\n");
	GRIB_LOGD("# grib regi \"DEVICE ADDR\"\n");
	GRIB_LOGD("# grib deregi \"DEVICE ID\"\n");
	GRIB_LOGD("# grib db all\n");
	GRIB_LOGD("# grib db create\n");
	GRIB_LOGD("# grib db drop\n");
	GRIB_LOGD("# grib ble init\n");
	GRIB_LOGD("# grib ble sacn\n");
	GRIB_LOGD("# grib ble info \"DEVICE ADDR\"\n");
//	GRIB_LOGD("#\n");
	GRIB_LOGD("# ########## ########## ########## ######### ########## ########## ##########\n");
	GRIB_LOGD("\n");
	return;
}

void Grib_MenuConfig(int argc, char **argv)
{
	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("LOAD CONFIG ERROR !!!\n");
		return;
	}

	GRIB_LOGD("\n");
	GRIB_LOGD("# ########## ########## ########## GRIB CONFIG ########## ########## ##########\n");
	GRIB_LOGD("# HUB_ID              : %s\n", pConfigInfo->hubID);
	GRIB_LOGD("# PLATFORM_SERVER_IP  : %s\n", pConfigInfo->platformServerIP);
	GRIB_LOGD("# PLATFORM_SERVER_PORT: %d\n", pConfigInfo->platformServerPort);
	GRIB_LOGD("# AUTH_SERVER_IP      : %s\n", pConfigInfo->authServerIP);
	GRIB_LOGD("# AUTH_SERVER_PORT    : %d\n", pConfigInfo->authServerPort);
	GRIB_LOGD("# SDA_SERVER_IP       : %s\n", pConfigInfo->sdaServerIP);
	GRIB_LOGD("# SDA_SERVER_PORT     : %d\n", pConfigInfo->sdaServerPort);
	GRIB_LOGD("# MYSQL_DB_HOST       : %s\n", pConfigInfo->iotDbHost);
	GRIB_LOGD("# MYSQL_DB_PORT       : %d\n", pConfigInfo->iotDbPort);
	GRIB_LOGD("# MYSQL_DB_USER       : %s\n", pConfigInfo->iotDbUser);
	GRIB_LOGD("# MYSQL_DB_PASSWORD   : %s\n", pConfigInfo->iotDbPswd);
	GRIB_LOGD("# GRIB_DEBUG_ONEM2M   : %s\n", GRIB_BOOL_TO_STR(pConfigInfo->debugOneM2M));
	GRIB_LOGD("# GRIB_DEBUG_BLE      : %s\n", GRIB_BOOL_TO_STR(pConfigInfo->debugBLE));
	GRIB_LOGD("# GRIB_DEBUG_THREAD   : %s\n", GRIB_BOOL_TO_STR(pConfigInfo->debugThread));
	GRIB_LOGD("# GRIB_TOMBSTONE_BLE  : %s\n", GRIB_BOOL_TO_STR(pConfigInfo->tombStoneBLE));
	GRIB_LOGD("# GRIB_TOMBSTONE_HTTP : %s\n", GRIB_BOOL_TO_STR(pConfigInfo->tombStoneHTTP));
	GRIB_LOGD("# ########## ########## ########## ######### ########## ########## ##########\n");
	GRIB_LOGD("\n");

	return;
}

void Grib_MenuHub(int argc, char **argv)
{
	int iRes = GRIB_ERROR;
	const char* FUNC_TAG = "# GRIB-HUB:";
	Grib_ConfigInfo* pConfigInfo = NULL;

	//3 shbaek: MAIN FUNCTION
	GRIB_LOGD("# ##### ##### ##### ##### ##### GRIB HUB START ##### ##### ##### ##### #####\n");
	GRIB_LOGD("# COMPILE TIME : %s %s\n", __DATE__, __TIME__);

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("%s GET CONFIG ERROR !!!\n", FUNC_TAG);
		return;
	}

	//shbaek: Config
	iRes = Grib_SetThreadConfig();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SERVER CONFIG ERROR\n", FUNC_TAG);
		return;
	}
	iRes = Grib_SiSetServerConfig();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SERVER CONFIG ERROR\n", FUNC_TAG);
		return;
	}
	iRes = Grib_BleConfig();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s BLE CONFIG ERROR\n", FUNC_TAG);
		return;
	}

	//shbaek: Init
	iRes = Grib_BleDetourInit();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s BLE INIT FAIL\n", FUNC_TAG);
	}

	iRes = Grib_BleCleanAll();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s BLE PIPE CLEAN FAIL\n", FUNC_TAG);
	}

#ifdef FEATURE_CAS
	//shbaek: CAS Lib Init & Get Certification
	iRes = Grib_CasInit(pConfigInfo->hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s: CAS INIT FAIL !!!\n", FUNC_TAG);
		return;
	}
#endif

	Grib_ThreadStart();
	return;
}

void Grib_MenuRegi(int argc, char **argv)
{
	const char* FUNC_TAG = "# GRIB-REGI:";

	int   iRes = GRIB_ERROR;
	char* deviceAddr = NULL;
	char* option = NULL;
	int   optAuth = GRIB_NOT_USED;

	Grib_ConfigInfo* pConfigInfo = NULL;

	//shbaek: You Must be Set Server Config.
	Grib_SiSetServerConfig();

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# USAGE: grib regi \"DEVICE ADDR\"\n");
		return;
	}

#ifdef FEATURE_CAS
	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("%s GET CONFIG ERROR !!!\n", FUNC_TAG);
		return;
	}

	//shbaek: CAS Lib Init & Get Certification
	iRes = Grib_CasInit(pConfigInfo->hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s: CAS INIT FAIL !!!\n", FUNC_TAG);
		return;
	}
#endif

	deviceAddr = argv[GRIB_CMD_SUB];

	if(GRIB_CMD_ARG1 < argc)
	{
		option = argv[GRIB_CMD_ARG1];

		if(STRNCASECMP(option, "NEW", STRLEN("NEW"))==0)
		{//shbaek: Regi Hub Self
			optAuth = AUTH_REGI_OPT_PW_OVER_WRITE;
		}

		if(STRNCASECMP(option, "RE", STRLEN("RE"))==0)
		{//shbaek: Regi Hub Self
			optAuth = AUTH_REGI_OPT_PW_RE_USED;
		}
	}

	Grib_DeviceRegi(deviceAddr, optAuth);
	return;
}

void Grib_MenuDeRegi(int argc, char **argv)
{
	const char* FUNC_TAG = "# GRIB-DeREGI:";

	int   iRes = GRIB_ERROR;
	Grib_ConfigInfo* pConfigInfo = NULL;

	int   delOneM2M = FALSE;
	char* deviceID  = NULL;
	char* option    = NULL;

	//shbaek: You Must be Set Server Config.
	Grib_SiSetServerConfig();

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# USAGE: grib deregi \"DEVICE ID\"\n");
		return;
	}

#ifdef FEATURE_CAS
	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("%s GET CONFIG ERROR !!!\n", FUNC_TAG);
		return;
	}

	//shbaek: CAS Lib Init & Get Certification
	iRes = Grib_CasInit(pConfigInfo->hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s: CAS INIT FAIL !!!\n", FUNC_TAG);
		return;
	}
#endif

	deviceID = argv[GRIB_CMD_SUB];

	if(GRIB_CMD_ARG1 < argc)
	{
		option = argv[GRIB_CMD_ARG1];

		if(STRNCASECMP(option, "ALL", STRLEN("ALL"))==0)
		{//shbaek: Regi Hub Self
			delOneM2M = TRUE;
		}
	}

	Grib_DeviceDeRegi(deviceID, delOneM2M);
	return;
}

void Grib_MenuBle(int argc, char **argv)
{
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* subMenu = NULL;
	char* pSendMsg = NULL;

	char* deviceID = NULL;
	char* deviceAddr = NULL;

	subMenu = GRIB_CMD_SUB[argv];

	Grib_BleConfig();

	if(STRCASECMP(subMenu, "init") == 0)
	{
		Grib_BleDetourInit();
	}
	if(STRCASECMP(subMenu, "info") == 0)
	{
		Grib_DbRowDeviceInfo rowDeviceInfo;
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib ble info \"DEVICE ADDR\"\n");
			return;
		}
		deviceAddr = GRIB_CMD_ARG1[argv];
		STRINIT(rowDeviceInfo.deviceAddr, sizeof(rowDeviceInfo.deviceAddr));
		STRNCPY(rowDeviceInfo.deviceAddr, deviceAddr, STRLEN(deviceAddr));

		Grib_BleDeviceInfo(&rowDeviceInfo);
	}
	if(STRCASECMP(subMenu, "send") == 0)
	{
		if(argc < GRIB_CMD_ARG3+1)
		{
			GRIB_LOGD("# USAGE: grib ble send \"DEVICE ADDR\" \"DEVICE ID\" \"SEND MSG\"\n");
			return;
		}

		deviceAddr	= GRIB_CMD_ARG1[argv];
		deviceID	= GRIB_CMD_ARG2[argv];
		pSendMsg	= GRIB_CMD_ARG3[argv];

		Grib_BleSendMsg(deviceAddr, deviceID, pSendMsg, recvBuff);

		//GRIB_LOGD("# SEND MSG[%03d]: %s\n", STRLEN(pSendMsg), pSendMsg);
		//GRIB_LOGD("# RECV MSG[%03d]: %s\n", STRLEN(recvBuff), recvBuff);
	}
	if(STRCASECMP(subMenu, "clean") == 0)
	{
		Grib_BleCleanAll();
	}

	return;
}


void Grib_MenuDb(int argc, char **argv)
{
	int   i = 0;
	int   x = 0;
	int	  iRes = GRIB_ERROR;
	int   iDeviceCount = 0;
	int   iFuncCount = 0;
	char* subMenu = NULL;

	char* deviceID = NULL;

	Grib_DbAll			   dbAll;
	Grib_DbAll*			   pDbAll;
	Grib_DbRowDeviceInfo*  pRowDeviceInfo;
	Grib_DbRowDeviceFunc*  pRowDeviceFunc;

	subMenu = GRIB_CMD_SUB[argv];

	if(STRCASECMP(subMenu, "create") == 0)
	{
		Grib_DbCreate();
		Grib_DbClose();
	}
	if(STRCASECMP(subMenu, "drop") == 0)
	{
		Grib_DbDrop();
		Grib_DbClose();
	}
	if(STRCASECMP(subMenu, "all") == 0)
	{
		Grib_DbToMemory(&dbAll);
		pDbAll = &dbAll;
		iDeviceCount = pDbAll->deviceCount;

		for(i=0; i<iDeviceCount; i++)
		{
			pRowDeviceInfo = pDbAll->ppRowDeviceInfo[i];
			iFuncCount = pRowDeviceInfo->deviceFuncCount;

			GRIB_LOGD("\n");
			GRIB_LOGD("# ---------- ---------- ---------- ---------- ---------- ---------- ----------\n");
			GRIB_LOGD("# DEVICE ID     : %s\n", pRowDeviceInfo->deviceID);
//			GRIB_LOGD("# DEVICE IF     : %s\n", Grib_InterfaceToStr((Grib_DeviceIfType)pRowDeviceInfo->deviceInterface));
			GRIB_LOGD("# DEVICE ADDR   : %s\n", pRowDeviceInfo->deviceAddr);
//			GRIB_LOGD("# DEVICE LOC    : %s\n", pRowDeviceInfo->deviceLoc);
//			GRIB_LOGD("# DEVICE DESC   : %s\n", pRowDeviceInfo->deviceDesc);
			GRIB_LOGD("# REPORT CYCLE  : %d\n", pRowDeviceInfo->reportCycle);
//			GRIB_LOGD("# DEVICE FCOUNT : %d\n", pRowDeviceInfo->deviceFuncCount);

			for(x=0; x<iFuncCount; x++)
			{
				pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[x];
				GRIB_LOGD("# FUNC INFO[%d/%d]: [%s] [%s] [%s]\n", x+1, iFuncCount, pRowDeviceFunc->funcName,
					pRowDeviceFunc->exRsrcID, Grib_FuncAttrToStr(pRowDeviceFunc->funcAttr));
				//GRIB_LOGD("# NAME/RI/ATTR : [%s][%s][0x%x]\n", pRowDeviceFunc->funcName, pRowDeviceFunc->exRsrcID, pRowDeviceFunc->funcAttr);
			}
		}
		//GRIB_LOGD("\n");
	}
	if(STRCASECMP(subMenu, "delete") == 0)
	{
		deviceID 	= GRIB_CMD_ARG1[argv];

		iRes = Grib_DbDelDeviceFunc(deviceID);
		if(iRes == GRIB_ERROR)
		{
			GRIB_LOGD("# DELETE DEVICE FUNC FAIL\n");
			return;
		}
		
		iRes = Grib_DbDelDeviceInfo(deviceID);
		if(iRes == GRIB_ERROR)
		{
			GRIB_LOGD("# DELETE DEVICE INFO FAIL\n");
		}
	}

	return;
}

void Grib_MenuXM2M(int argc, char **argv)
{
	const char* FUNC_TAG = "# GRIB-xM2M:";

	int 	iRes = GRIB_ERROR;

	char* 	mainMenu = NULL;
	char* 	subMenu  = NULL;

	char* 	argNM = NULL;
	char* 	argURI = NULL;
	char* 	argCON = NULL;
	char* 	argOrigin = NULL;
	char*   deviceID = NULL;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

#ifdef FEATURE_CAS
	char pAuthKey[CAS_AUTH_KEY_SIZE] = {'\0', };
	Grib_ConfigInfo* pConfigInfo = NULL;
#endif

	MEMSET(&reqParam, GRIB_INIT, sizeof(reqParam));
	MEMSET(&resParam, GRIB_INIT, sizeof(resParam));

	//shbaek: You Must be Set Server Config.
	Grib_SiSetServerConfig();

	if(argc < GRIB_CMD_ARG1+1)
	{
		GRIB_LOGD("# NO CMD ARG\n");
		return;
	}

	mainMenu = GRIB_CMD_MAIN[argv];
	subMenu  = GRIB_CMD_SUB[argv];
	deviceID = GRIB_CMD_ARG1[argv];

#ifdef FEATURE_CAS
	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("%s GET CONFIG ERROR !!!\n", FUNC_TAG);
		return;
	}

	//shbaek: CAS Lib Init & Get Certification
	iRes = Grib_CasInit(pConfigInfo->hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CAS INIT FAIL !!!\n", FUNC_TAG);
		return;
	}

	iRes = Grib_AuthGetPW(deviceID, pAuthKey);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s %s GET AUTH PASSWORD ERROR !!!\n", FUNC_TAG, deviceID);

		iRes = Grib_CasGetAuthKey(deviceID, pAuthKey);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("%s %s GET AUTH KEY ERROR !!!\n", FUNC_TAG, deviceID);
		}
	}

	reqParam.authKey = pAuthKey;
#endif

	if(STRCASECMP(subMenu, "cae") == 0)
	{
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m cae \"DEVICE ID\"\n");
			return;
		}
		argNM 	= GRIB_CMD_ARG1[argv];
		GRIB_LOGD("# CREATE APP ENTITY: %s\n", argNM);

		STRINIT(&reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(&reqParam.xM2M_NM, argNM, STRLEN(argNM));

		iRes = Grib_AppEntityCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}

	}
	if(STRCASECMP(subMenu, "rae") == 0)
	{
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m rae \"DEVICE ID\"\n");
			return;
		}
		argOrigin 	= GRIB_CMD_ARG1[argv];
		GRIB_LOGD("# RETRIEVE APP ENTITY: %s\n", argOrigin);

		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, argOrigin, STRLEN(argOrigin));

		iRes = Grib_AppEntityRetrieve(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}

	}
	if(STRCASECMP(subMenu, "dae") == 0)
	{
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m dae \"DEVICE ID\"\n");
			return;
		}
		argOrigin 	= GRIB_CMD_ARG1[argv];
		GRIB_LOGD("# DELETE APP ENTITY: %s\n", argOrigin);

		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, argOrigin, STRLEN(argOrigin));

		iRes = Grib_AppEntityDelete(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}
	if(STRCASECMP(subMenu, "ccnt") == 0)
	{
		if(argc < GRIB_CMD_ARG3+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m ccnt \"DEVICE ID\" \"URI\" \"NM\"\n");
			return;
		}

		argOrigin	= GRIB_CMD_ARG1[argv];
		argURI		= GRIB_CMD_ARG2[argv];
		argNM		= GRIB_CMD_ARG3[argv];

		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, argOrigin, STRLEN(argOrigin));

		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(&reqParam.xM2M_URI, argURI, STRLEN(argURI));

		STRINIT(&reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(&reqParam.xM2M_NM, argNM, STRLEN(argNM));

		iRes = Grib_ContainerCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}
	if(STRCASECMP(subMenu, "rcnt") == 0)
	{
		if(argc < GRIB_CMD_ARG2+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m rcnt \"DEVICE ID\" \"URI\"\n");
			return;
		}

		argOrigin	= GRIB_CMD_ARG1[argv];
		argURI		= GRIB_CMD_ARG2[argv];

		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, argOrigin, STRLEN(argOrigin));

		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(&reqParam.xM2M_URI, argURI, STRLEN(argURI));

		iRes = Grib_ContainerRetrieve(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}
	if(STRCASECMP(subMenu, "dcnt") == 0)
	{
		if(argc < GRIB_CMD_ARG2+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m dcnt \"DEVICE ID\" \"URI\"\n");
			return;
		}

		argOrigin	= GRIB_CMD_ARG1[argv];
		argURI		= GRIB_CMD_ARG2[argv];

		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, argOrigin, STRLEN(argOrigin));

		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(&reqParam.xM2M_URI, argURI, STRLEN(argURI));

		iRes = Grib_ContainerDelete(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}
	if(STRCASECMP(subMenu, "ccin") == 0)
	{
		if(argc < GRIB_CMD_ARG3+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m ccin \"DEVICE ID\" \"URI\" \"CONTENT\"\n");
			return;
		}
		argOrigin 	= GRIB_CMD_ARG1[argv];
		argURI 		= GRIB_CMD_ARG2[argv];
		argCON 		= GRIB_CMD_ARG3[argv];

		GRIB_LOGD("# CREATE INSTANCE: %s[CON:%s]\n", argURI, argCON);

		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, argOrigin, STRLEN(argOrigin));

		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s", argURI);

		STRINIT(&reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF));
		SNPRINTF(&reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF), "%s:0", HTTP_CONTENT_TYPE_TEXT);

		STRINIT(&reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON));
		SNPRINTF(&reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON), "%s", argCON);

		//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_CNF[If NULL, Set Default "text/plain:0"], xM2M_CON
		iRes = Grib_ContentInstanceCreate(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}
	if(STRCASECMP(subMenu, "rcin") == 0)
	{
		if(argc < GRIB_CMD_ARG2+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m rcin \"DEVICE ID\" \"URI\"\n");
			return;
		}
		argOrigin 	= GRIB_CMD_ARG1[argv];
		argURI 		= GRIB_CMD_ARG2[argv];

		STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(&reqParam.xM2M_Origin, argOrigin, STRLEN(argOrigin));

		STRINIT(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		SNPRINTF(&reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s", argURI);

		iRes = Grib_ContentInstanceRetrieve(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}

	return;
}

void Grib_MenuAuth(int argc, char **argv)
{
	const char* FUNC_TAG = "# GRIB-AUTH:";

	int   iRes = GRIB_ERROR;

	char* 	mainMenu = NULL;
	char* 	subMenu  = NULL;
	char*	pID = NULL;
	char*	pPW = NULL;

	if(STRLEN(GRIB_CMD_SUB[argv]) <= 0)
	{
		GRIB_LOGD("# USAGE: grib auth \"[gwRegi, devRegi, info, deRegi]\" \"Device ID\" \"PassWord\"\n");
		return;
	}

	mainMenu = GRIB_CMD_MAIN[argv];
	subMenu  = GRIB_CMD_SUB[argv];

	//shbaek: You Must be Set Server Config.
	Grib_AuthSetServerConfig();

	if(STRCASECMP(subMenu, "gwRegi") == 0)
	{
		if(STRLEN(GRIB_CMD_ARG1[argv]) <= 0)
		{
			GRIB_LOGD("# USAGE: grib gwRegi \"GateWay ID\"\n");
			return;
		}
		pID = GRIB_CMD_ARG1[argv];
		GRIB_LOGD("%s GATEWAY ID: %s\n", FUNC_TAG, pID);

		iRes = Grib_AuthGatewayRegi(pID);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}

	if(STRCASECMP(subMenu, "devRegi") == 0)
	{
		if(STRLEN(GRIB_CMD_ARG1[argv]) <= 0)
		{
			GRIB_LOGD("# USAGE: grib devRegi \"Device ID\" \"PassWord\"\n");
			return;
		}
		pID = GRIB_CMD_ARG1[argv];

		if(STRLEN(GRIB_CMD_ARG2[argv]) <= 0)
		{
			pPW = AUTH_DEFAULT_DEVICE_PW;
		}
		pPW = GRIB_CMD_ARG2[argv];

		iRes = Grib_AuthDeviceRegi(pID, pPW);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}

	if(STRCASECMP(subMenu, "info") == 0)
	{
		if(STRLEN(GRIB_CMD_ARG1[argv]) <= 0)
		{
			GRIB_LOGD("# USAGE: grib info \"Device ID\"\n");
			return;
		}
		pID = GRIB_CMD_ARG1[argv];
		GRIB_LOGD("%s DEVICE ID: %s\n", FUNC_TAG, pID);

		iRes = Grib_AuthDeviceInfo(pID, GRIB_NOT_USED);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}
	if(STRCASECMP(subMenu, "pw") == 0)
	{
		if(STRLEN(GRIB_CMD_ARG1[argv]) <= 0)
		{
			GRIB_LOGD("# USAGE: grib pw \"Device ID\"\n");
			return;
		}
		pID = GRIB_CMD_ARG1[argv];
		GRIB_LOGD("%s DEVICE ID: %s\n", FUNC_TAG, pID);

		iRes = Grib_AuthGetPW(pID, GRIB_NOT_USED);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}
	if(STRCASECMP(subMenu, "deRegi") == 0)
	{
		if(STRLEN(GRIB_CMD_ARG1[argv]) <= 0)
		{
			GRIB_LOGD("# USAGE: grib gwRegi \"Device ID\"\n");
			return;
		}
		pID = GRIB_CMD_ARG1[argv];
		GRIB_LOGD("%s GATEWAY ID: %s\n", FUNC_TAG, pID);

		iRes = Grib_AuthDeviceDeRegi(pID);
		if(iRes == GRIB_ERROR)
		{
			strUpper(mainMenu);
			strUpper(subMenu);
			GRIB_LOGD("# %s %s MENU FAIL\n", mainMenu, subMenu);
		}
	}

	return;
}

void Grib_MenuCas(int argc, char **argv)
{
	int   iDelay = 0;
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* subMenu = NULL;
	char* pSendMsg = NULL;

	char* deviceID = NULL;
	char* deviceAddr = NULL;

#ifdef FEATURE_CAS
	Grib_CasTest(argc, argv);
#else
	GRIB_LOGD("# FEATURE_CAS IS NOT DEFINE !!!\n");
#endif

	return;
}

void Grib_MenuTest(int argc, char **argv)
{
	int   iDelay = 0;
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* subMenu = NULL;
	char* pSendMsg = NULL;

	char* deviceID = NULL;
	char* deviceAddr = NULL;

	subMenu = GRIB_CMD_SUB[argv];

	if(STRCASECMP(subMenu, "sda") == 0)
	{
		char  deviceInfo[HTTP_MAX_SIZE_RECV_MSG] = {0x00, };

		if(STRLEN(argv[GRIB_CMD_ARG1]) <= 0)
		{
			deviceID = TEST_SDA_DEVICE_ID;
		}
		else
		{
			deviceID = argv[GRIB_CMD_ARG1];
		}
		Grib_SdaGetDeviceInfo(deviceID, deviceInfo);
		return;
	}

	if(STRCASECMP(subMenu, "cas") == 0)
	{
#ifdef FEATURE_CAS
		Grib_CasTest(argc, argv);
#else
		GRIB_LOGD("# FEATURE_CAS IS NOT DEFINE !!!\n");
#endif
		return;
	}

	if(STRCASECMP(subMenu, "print") == 0)
	{
		long stackLimit = Grib_GetStackLimit();
		printf("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		printf("# TEST PRINT: %ld\n", stackLimit);
		printf("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		return;
	}


	if(STRCASECMP(subMenu, "ebase64") == 0)
	{
		char  encBuff[HTTP_MAX_SIZE_RECV_MSG] = {0x00, };
		char* srcText = NULL;

		if(STRLEN(argv[GRIB_CMD_ARG1]) <= 0)
		{
			srcText = TEST_BASE64_ENC_SRC;
		}
		else
		{
			srcText = argv[GRIB_CMD_ARG1];
		}
		Grib_Base64Encode(srcText, encBuff, GRIB_NOT_USED);

		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		GRIB_LOGD("# SRC[%d]:\n%s\n", STRLEN(srcText), srcText);
		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		GRIB_LOGD("\n");
		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		GRIB_LOGD("# ENC[%d]:\n%s\n", STRLEN(encBuff), encBuff);
		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		
		return;
	}

	if(STRCASECMP(subMenu, "dbase64") == 0)
	{
		char  decBuff[HTTP_MAX_SIZE_RECV_MSG] = {0x00, };
		char* srcText = NULL;

		if(STRLEN(argv[GRIB_CMD_ARG1]) <= 0)
		{
			srcText = TEST_BASE64_DEC_SRC;
		}
		else
		{
			srcText = argv[GRIB_CMD_ARG1];
		}
		Grib_Base64Decode(srcText, decBuff, GRIB_NOT_USED);

		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		GRIB_LOGD("# SRC[%d]:\n%s\n", STRLEN(srcText), srcText);
		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		GRIB_LOGD("\n");
		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		GRIB_LOGD("# DEC[%d]:\n%s\n", STRLEN(decBuff), decBuff);
		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
		
		return;
	}

	return;
}

int main(int argc, char **argv)
{
	char* mainMenu = NULL;
	char GRIB_PATH[MAXPATHLEN];

	if(argc < GRIB_CMD_MAIN+1)
	{
		Grib_MenuHelp();
		return GRIB_DONE;
	}
	mainMenu = GRIB_CMD_MAIN[argv];
	realpath(argv[0], GRIB_PATH);

	if(FALSE)
	{
		GRIB_LOGD("# GRIB_PATH: %s\n", GRIB_PATH);
		for(int i=0; i<argc; i++)GRIB_LOGD("# ARG[%d/%d]: %s\n", i, argc, argv[i]);
	}

	if(STRCASECMP(mainMenu, "version") == 0)
	{
		printf("# ##### ##### ##### ##### HUB VERSION ##### ##### ##### #####\n");
		printf("# HUB VERSION : %s\n", GRIB_HUB_VERSION);
		printf("# COMPILE TIME: %s-%s\n", __DATE__, __TIME__);
		printf("# ##### ##### ##### ##### ##### ##### ##### ##### ##### #####\n");

		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "help") == 0)
	{
		Grib_MenuHelp();
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "hub") == 0)
	{
		Grib_MenuHub(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "regi") == 0)
	{
		Grib_MenuRegi(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "deregi") == 0)
	{
		Grib_MenuDeRegi(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "ble") == 0)
	{
		Grib_MenuBle(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "xm2m") == 0)
	{
		Grib_MenuXM2M(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "db") == 0)
	{
		Grib_MenuDb(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "config") == 0)
	{
		Grib_MenuConfig(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "auth") == 0)
	{
		Grib_MenuAuth(argc, argv);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "test") == 0)
	{
		Grib_MenuTest(argc, argv);
		return GRIB_DONE;
	}

	GRIB_LOGD("# INVALID MENU\n");
	for(int i=0; i<argc; i++)GRIB_LOGD("# ARG[%d/%d]: %s\n", i, argc, argv[i]);

	Grib_MenuHelp();

	return GRIB_ERROR;
}
