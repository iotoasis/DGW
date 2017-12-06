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
#include "grib_auth.h"
#include "grib_log.h"

#ifdef FEATURE_CAS
#include "grib_cas.h"
#endif

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

void Grib_MenuHelp(void)
{
	GRIB_LOGD("\n");
	GRIB_LOGD(GRIB_1LINE_SHARP);
//	GRIB_LOGD("#\n");
	GRIB_LOGD("# grib version\n");
	GRIB_LOGD("# grib hub\n");
	GRIB_LOGD("# grib kill\n");
	GRIB_LOGD("# grib regi \"DEVICE ADDR\"\n");
	GRIB_LOGD("# grib deregi \"DEVICE ID\"\n");
	GRIB_LOGD("# grib db all\n");
	GRIB_LOGD("# grib db create\n");
	GRIB_LOGD("# grib db drop\n");
	GRIB_LOGD("# grib config\n");
	GRIB_LOGD("# grib config hub \"HUB ID\"\n");
	GRIB_LOGD("# grib config si \"SI IP\", \"SI PORT\", \"IN NAME\", \"CSE NMAE\"\n");
	GRIB_LOGD("# grib config etc \"RESET HOUR\", \"DEBUG[0 or 1]\", \"TOMBSTONE[0 or 1]\"\n");
	GRIB_LOGD("# grib ble init\n");
	GRIB_LOGD("# grib ble sacn\n");
	GRIB_LOGD("# grib ble info \"DEVICE ADDR\"\n");
//	GRIB_LOGD("#\n");
	GRIB_LOGD(GRIB_1LINE_SHARP);
	GRIB_LOGD("\n");
	return;
}

void Grib_MenuConfig(int argc, char **argv)
{
	const char* FUNC = "CONFIG-MENU";

	Grib_ConfigInfo* pConfigInfo = NULL;

	int   iRes = GRIB_ERROR;
	char* subMenu = NULL;

	if(STRLEN(argv[GRIB_CMD_SUB]) == 0)
	{
		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
			return;
		}

		Grib_ShowConfig(pConfigInfo);

		return;		
	}
	subMenu = argv[GRIB_CMD_SUB];

	if(STRNCASECMP(subMenu, "HUB", STRLEN("HUB"))==0)
	{
		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
			return;
		}

		if(STRLEN(argv[GRIB_CMD_ARG1]) != 0)
		{
			STRINIT(pConfigInfo->hubID, sizeof(pConfigInfo->hubID));
			STRNCPY(pConfigInfo->hubID, argv[GRIB_CMD_ARG1], STRLEN(argv[GRIB_CMD_ARG1]));
		}

		iRes = Grib_SetConfigHub(pConfigInfo);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# SET CONFIG SI ERROR !!!\n");
			return;
		}
		GRIB_LOGD("# SET CONFIG SI DONE ...\n");
		Grib_ShowConfig(pConfigInfo);

		return;	
	}
	
	if(STRNCASECMP(subMenu, "SI", STRLEN("SI"))==0)
	{
		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
			return;
		}

		if(STRLEN(argv[GRIB_CMD_ARG1]) != 0)
		{
			STRINIT(pConfigInfo->siServerIP, sizeof(pConfigInfo->siServerIP));
			STRNCPY(pConfigInfo->siServerIP, argv[GRIB_CMD_ARG1], STRLEN(argv[GRIB_CMD_ARG1]));
		}
		if(STRLEN(argv[GRIB_CMD_ARG2]) != 0)
		{
			pConfigInfo->siServerPort = ATOI(argv[GRIB_CMD_ARG2]);
		}
		if(STRLEN(argv[GRIB_CMD_ARG3]) != 0)
		{
			STRINIT(pConfigInfo->siInName, sizeof(pConfigInfo->siInName));
			STRNCPY(pConfigInfo->siInName, argv[GRIB_CMD_ARG3], STRLEN(argv[GRIB_CMD_ARG3]));
		}
		if(STRLEN(argv[GRIB_CMD_ARG4]) != 0)
		{
			STRINIT(pConfigInfo->siCseName, sizeof(pConfigInfo->siCseName));
			STRNCPY(pConfigInfo->siCseName, argv[GRIB_CMD_ARG4], STRLEN(argv[GRIB_CMD_ARG4]));
		}

		iRes = Grib_SetConfigSi(pConfigInfo);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# SET CONFIG SI ERROR !!!\n");
			return;
		}
		GRIB_LOGD("# SET CONFIG SI DONE ...\n");
		Grib_ShowConfig(pConfigInfo);

		return;	
	}

	if(STRNCASECMP(subMenu, "SMD", STRLEN("SMD"))==0)
	{
		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
			return;
		}

		if(STRLEN(argv[GRIB_CMD_ARG1]) != 0)
		{
			STRINIT(pConfigInfo->smdServerIP, sizeof(pConfigInfo->smdServerIP));
			STRNCPY(pConfigInfo->smdServerIP, argv[GRIB_CMD_ARG1], STRLEN(argv[GRIB_CMD_ARG1]));
		}
		if(STRLEN(argv[GRIB_CMD_ARG2]) != 0)
		{
			pConfigInfo->smdServerPort = ATOI(argv[GRIB_CMD_ARG2]);
		}

		iRes = Grib_SetConfigSmd(pConfigInfo);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# SET CONFIG SMD ERROR !!!\n");
			return;
		}
		GRIB_LOGD("# SET CONFIG SMD DONE ...\n");
		Grib_ShowConfig(pConfigInfo);

		return;	
	}

	if(STRNCASECMP(subMenu, "ETC", STRLEN("ETC"))==0)
	{
		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
			return;
		}

		if(STRLEN(argv[GRIB_CMD_ARG1]) != 0)
		{
			pConfigInfo->resetTimeHour = ATOI(argv[GRIB_CMD_ARG1]);
		}

		if(STRLEN(argv[GRIB_CMD_ARG2]) != 0)
		{
			pConfigInfo->debugLevel = ATOI(argv[GRIB_CMD_ARG2]);
		}

		if(STRLEN(argv[GRIB_CMD_ARG3]) != 0)
		{
			pConfigInfo->tombStone = ATOI(argv[GRIB_CMD_ARG3]);
		}

		iRes = Grib_SetConfigEtc(pConfigInfo);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# SET CONFIG ETC ERROR !!!\n");
			return;
		}
		GRIB_LOGD("# SET CONFIG ETC DONE ...\n");
		Grib_ShowConfig(pConfigInfo);

		return;	
	}

	Grib_ErrLog(FUNC, "INVALID ARG !!!");

	return;
}

void Grib_HubConfig(int mode)
{
	int iRes = GRIB_ERROR;
	const char* FUNC = "# GRIB-HUB:";
	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("%s GET CONFIG ERROR !!!\n", FUNC);
		return;
	}

	//shbaek: Config
	iRes = Grib_SetThreadConfig();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SERVER CONFIG ERROR\n", FUNC);
		return;
	}
	iRes = Grib_SiSetServerConfig();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s SERVER CONFIG ERROR\n", FUNC);
		return;
	}

#ifdef FEATURE_CAS
	//shbaek: CAS Lib Init & Get Certification
	iRes = Grib_CasInit(pConfigInfo->hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s: CAS INIT FAIL !!!\n", FUNC);
		return;
	}
#endif

	return;
}

void Grib_MenuHub(int argc, char **argv)
{
	const char* FUNC = "# GRIB-HUB:";
	int iRes = GRIB_ERROR;

	//3 shbaek: MAIN FUNCTION
	GRIB_LOGD(GRIB_1LINE_SHARP);
	GRIB_LOGD("# COMPILE TIME : %s %s\n", __DATE__, __TIME__);

	Grib_HubConfig(1);

	iRes = Grib_BleConfig();
	if(iRes != GRIB_DONE)
	{
		Grib_ErrLog(FUNC, "BLE CONFIG ERROR !!!");
		return;
	}

	//shbaek: Init
	iRes = Grib_BleDetourInit();
	if(iRes != GRIB_DONE)
	{
		Grib_ErrLog(FUNC, "BLE INIT FAIL !!!");
	}

	iRes = Grib_BleCleanAll();
	if(iRes != GRIB_DONE)
	{
		Grib_ErrLog(FUNC, "BLE PIPE CLEAN FAIL !!!");
	}

	Grib_ThreadStart();
	return;
}

void Grib_MenuRegi(int argc, char **argv)
{
	char* deviceAddr = NULL;
	char* option = NULL;
	int   optAuth = GRIB_NOT_USED;

	Grib_BleConfig();

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# USAGE: grib regi \"DEVICE ADDR\"\n");
		return;
	}

	deviceAddr = argv[GRIB_CMD_SUB];

	if(GRIB_CMD_ARG1 < argc)
	{
		option = argv[GRIB_CMD_ARG1];

		if(STRNCASECMP(option, "SERVER", STRLEN("SERVER"))==0)
		{//shbaek: Create OneM2M Server Resource
			optAuth = REGI_OPT_CREATE_RESOURCE;
		}
	}

	if(GRIB_CMD_ARG2 < argc)
	{
		option = argv[GRIB_CMD_ARG2];

		if(STRNCASECMP(option, "NEW", STRLEN("NEW"))==0)
		{//shbaek: Regi Hub Self
			optAuth |= REGI_OPT_PW_OVER_WRITE;
		}

		if(STRNCASECMP(option, "RE", STRLEN("RE"))==0)
		{//shbaek: Regi Hub Self
			optAuth |= REGI_OPT_PW_RE_USED;
		}
	}

	Grib_DeviceRegi(deviceAddr, optAuth);
	return;
}

void Grib_MenuDeRegi(int argc, char **argv)
{
	int   delOneM2M = FALSE;
	char* deviceID  = NULL;
	char* option    = NULL;

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# USAGE: grib deregi \"DEVICE ID\"\n");
		return;
	}

	deviceID = argv[GRIB_CMD_SUB];

	if(GRIB_CMD_ARG1 < argc)
	{
		option = argv[GRIB_CMD_ARG1];

		if(STRNCASECMP(option, "SERVER", STRLEN("SERVER"))==0)
		{//shbaek: Regi Hub Self
			delOneM2M = TRUE;
		}

		if(STRNCASECMP(option, "ALL", STRLEN("ALL"))==0)
		{//shbaek: Delete All Local Device Info

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

		Grib_BleGetDeviceInfo(&rowDeviceInfo);
	}
	if(STRCASECMP(subMenu, "clean") == 0)
	{
		Grib_BleCleanAll();
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

	if(STRCASECMP(subMenu, "raw") == 0)
	{
		char* deviceAddr = argv[GRIB_CMD_ARG1];
		char* charHandle = argv[GRIB_CMD_ARG2];
		char* strBuff = argv[GRIB_CMD_ARG3];

		char* hexBuff = NULL;

		Grib_BleMsgInfo bleMsg;

		MEMSET(&bleMsg, 0x00, sizeof(bleMsg));

		hexBuff = (char*)MALLOC(STRLEN(strBuff)*2+1);
		Grib_StrToHex(strBuff, hexBuff);

		STRNCPY(bleMsg.addr, deviceAddr, STRLEN(deviceAddr));
		STRNCPY(bleMsg.handle, charHandle, STRLEN(charHandle));

		bleMsg.sendMsg = hexBuff;
		bleMsg.label = "TEST";

		Grib_BleSendRaw(&bleMsg);
	}

	if(STRCASECMP(subMenu, "char") == 0)
	{
		char* deviceAddr = argv[GRIB_CMD_ARG1];
		char* findHandle = argv[GRIB_CMD_ARG2];

		char* pipeFile = NULL;

		Grib_BleMsgInfo bleMsg;

		MEMSET(&bleMsg, 0x00, sizeof(bleMsg));

		if(GRIB_CMD_ARG3 < argc)
		{
			if(0<STRLEN(argv[GRIB_CMD_ARG3]))pipeFile = argv[GRIB_CMD_ARG3];
			//else pipeFile = "TEST_PIPE";
		}

		STRNCPY(bleMsg.addr, deviceAddr, STRLEN(deviceAddr));
		STRNCPY(bleMsg.findHandle, findHandle, STRLEN(findHandle));

		bleMsg.pipe = pipeFile;
		bleMsg.peerType = BLE_PEER_TYPE_PUBLIC;
		bleMsg.label = "TEST";

		Grib_BleGetCharHandler(&bleMsg);

		GRIB_LOGD("# MENU-BLE: [MAIN: %s] [SUB: %s] DONE ...\n", argv[GRIB_CMD_MAIN], argv[GRIB_CMD_SUB]);
	}

	return;
}

void Grib_MenuDb(int argc, char **argv)
{
	const char* FUNC = "MENU-DB";

	int   i = 0;
	int	  iRes = GRIB_ERROR;
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
		Grib_GetConfigDB();
		Grib_DbClose();
		return;
	}
	if(STRCASECMP(subMenu, "drop") == 0)
	{
		Grib_DbDrop();
		Grib_DbClose();
		return;
	}
	if(STRCASECMP(subMenu, "cache") == 0)
	{
		int iLoop = 0;
		int iCacheCount = 0;
		const int MAX_LOOP_COUNT = 0;

		Grib_DbCacheRiAll cacheRiAll;
		Grib_DbCacheRiAll* pCacheRiAll;
		Grib_DbRowCacheRi*  pRowCacheRi = NULL;

LOOP_TEST:
		GRIB_LOGD("\n");
		GRIB_LOGD("# LOOP COUNT : %d\n", iLoop);
		Grib_ShowCurrDateTime();
		iRes = Grib_DbGetCacheRiAll(&cacheRiAll);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# GET CACHE RI INFO FAIL !!!\n");
			return;
		}
		pCacheRiAll = &cacheRiAll;
		iCacheCount = pCacheRiAll->cacheCount;
		for(i=0; i<iCacheCount; i++)
		{
			pRowCacheRi = pCacheRiAll->ppRowCacheRi[i];
			GRIB_LOGD("\n");
			GRIB_LOGD(GRIB_1LINE_DASH);
			GRIB_LOGD("# RESOURCE ID   : %s\n", pRowCacheRi->rid);
			//GRIB_LOGD("# RESOURCE TYPE : %d\n", pRowCacheRi->rtype);
			//GRIB_LOGD("# RESOURCE NAME : %s\n", pRowCacheRi->rname);
			//GRIB_LOGD("# PARENTS ID    : %s\n", pRowCacheRi->pid);
			GRIB_LOGD("# URI           : %s\n", pRowCacheRi->uri);
		}
		//GRIB_LOGD("\n");

		Grib_DoubleFree((void **)pCacheRiAll->ppRowCacheRi, pCacheRiAll->cacheCount);

		iLoop++;
		SLEEP(1);

		if(iLoop<MAX_LOOP_COUNT) goto LOOP_TEST;
	}
	if(STRCASECMP(subMenu, "device") == 0)
	{
		int   x = 0;
		int   iDeviceCount = 0;
		int   iFuncCount = 0;

		pDbAll = &dbAll;
		MEMSET(pDbAll, 0x00, sizeof(Grib_DbAll));
		iRes = Grib_DbToMemory(&dbAll);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# GET DEVICE INFO FAIL !!!\n");
			return;
		}

		iDeviceCount = pDbAll->deviceCount;

		for(i=0; i<iDeviceCount; i++)
		{
			pRowDeviceInfo = pDbAll->ppRowDeviceInfo[i];
			iFuncCount = pRowDeviceInfo->deviceFuncCount;

			GRIB_LOGD("\n");
			GRIB_LOGD(GRIB_1LINE_DASH);
			GRIB_LOGD("# DEVICE ID     : %s\n", pRowDeviceInfo->deviceID);
			GRIB_LOGD("# DEVICE ADDR   : %s\n", pRowDeviceInfo->deviceAddr);
			GRIB_LOGD("# REPORT CYCLE  : %d\n", pRowDeviceInfo->reportCycle);

			for(x=0; x<iFuncCount; x++)
			{
				pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[x];
				GRIB_LOGD("# FUNC INFO[%d/%d]: [%20s] [%s]\n", x+1, iFuncCount, 
					Grib_FuncAttrToStr(pRowDeviceFunc->funcAttr),	pRowDeviceFunc->funcName);
			}
		}
		//GRIB_LOGD("\n");

		Grib_DoubleFree((void **)pDbAll->ppRowDeviceInfo, pDbAll->deviceCount);
		return;
	}
	if(STRCASECMP(subMenu, "delete") == 0)
	{
		deviceID 	= GRIB_CMD_ARG1[argv];

		if(STRCASECMP(deviceID, "all") == 0)
		{//shbaek: Delete All ...
			iRes = Grib_DbDelDeviceAll();
			if(iRes == GRIB_ERROR)
			{
				Grib_ErrLog(FUNC, "DELETE DEVICE ALL ERROR !!!");
				return;
			}
		}
		else
		{//shbaek: Need Device ID
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
				return;
			}
		}
		return;
	}

	Grib_ErrLog(FUNC, "INVALID ARG !!!");

	return;
}

void Grib_MenuXM2M(int argc, char **argv)
{
	const char* FUNC = "# GRIB-xM2M:";

	int 	iRes = GRIB_ERROR;

	char* 	mainMenu = NULL;
	char* 	subMenu  = NULL;

	char* 	argNM = NULL;
	char* 	argURI = NULL;
	char* 	argCON = NULL;
	char*   deviceID = NULL;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

#ifdef FEATURE_CAS
	char pAuthKey[GRIB_MAX_SIZE_AUTH_KEY] = {'\0', };
#endif

	Grib_ConfigInfo* pConfigInfo = NULL;

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

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("%s GET CONFIG ERROR !!!\n", FUNC);
		return;
	}

#ifdef FEATURE_CAS
	//shbaek: CAS Lib Init & Get Certification
	iRes = Grib_CasInit(pConfigInfo->hubID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s CAS INIT FAIL !!!\n", FUNC);
		return;
	}

	iRes = Grib_AuthGetPW(deviceID, pAuthKey);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("%s %s GET AUTH PASSWORD ERROR !!!\n", FUNC, deviceID);

		iRes = Grib_CasGetAuthKey(deviceID, pAuthKey);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("%s %s GET AUTH KEY ERROR !!!\n", FUNC, deviceID);
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
		deviceID 	= GRIB_CMD_ARG1[argv];
		GRIB_LOGD("# CREATE APP ENTITY: %s\n", deviceID);

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

		STRINIT(reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(reqParam.xM2M_NM, argNM, STRLEN(argNM));

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
		deviceID 	= GRIB_CMD_ARG1[argv];
		GRIB_LOGD("# RETRIEVE APP ENTITY: %s\n", deviceID);

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

		iRes = Grib_AppEntityRetrieve(&reqParam, &resParam);
		GRIB_LOGD("\n");
		GRIB_LOGD(GRIB_1LINE_SHARP);

		if(iRes == GRIB_DONE)
		{
			strUpper(mainMenu);
			strUpper(subMenu);

			GRIB_LOGD("# RETRIEVE APP ENTITY RECV[%d]:\n%s\n", STRLEN(resParam.http_RecvData), resParam.http_RecvData);
		}
		else
		{
			GRIB_LOGD("# RETRIEVE APP ENTITY: %s [%d]\n", resParam.http_ResMsg, resParam.http_ResNum);			
		}
		
		GRIB_LOGD(GRIB_1LINE_SHARP);
	}
	if(STRCASECMP(subMenu, "dae") == 0)
	{
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m dae \"DEVICE ID\"\n");
			return;
		}
		deviceID 	= GRIB_CMD_ARG1[argv];
		GRIB_LOGD("# DELETE APP ENTITY: %s\n", deviceID);

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

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

		deviceID	= GRIB_CMD_ARG1[argv];
		argURI		= GRIB_CMD_ARG2[argv];
		argNM		= GRIB_CMD_ARG3[argv];

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

		STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(reqParam.xM2M_URI, argURI, STRLEN(argURI));

		STRINIT(reqParam.xM2M_NM, sizeof(reqParam.xM2M_NM));
		STRNCPY(reqParam.xM2M_NM, argNM, STRLEN(argNM));

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

		deviceID	= GRIB_CMD_ARG1[argv];
		argURI		= GRIB_CMD_ARG2[argv];

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

		STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(reqParam.xM2M_URI, argURI, STRLEN(argURI));

		iRes = Grib_ContainerRetrieve(&reqParam, &resParam);
		GRIB_LOGD("\n");
		GRIB_LOGD(GRIB_1LINE_SHARP);

		if(iRes == GRIB_DONE)
		{
			strUpper(mainMenu);
			strUpper(subMenu);

			GRIB_LOGD("# RETRIEVE CONTAINER RECV[%d]:\n%s\n", STRLEN(resParam.http_RecvData), resParam.http_RecvData);
		}
		else
		{
			GRIB_LOGD("# RETRIEVE CONTAINER: %s [%d]\n", resParam.http_ResMsg, resParam.http_ResNum);			
		}
		
		GRIB_LOGD(GRIB_1LINE_SHARP);
	}
	if(STRCASECMP(subMenu, "dcnt") == 0)
	{
		if(argc < GRIB_CMD_ARG2+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m dcnt \"DEVICE ID\" \"URI\"\n");
			return;
		}

		deviceID	= GRIB_CMD_ARG1[argv];
		argURI		= GRIB_CMD_ARG2[argv];

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

		STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		STRNCPY(reqParam.xM2M_URI, argURI, STRLEN(argURI));

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
		deviceID 	= GRIB_CMD_ARG1[argv];
		argURI 		= GRIB_CMD_ARG2[argv];
		argCON 		= GRIB_CMD_ARG3[argv];

		GRIB_LOGD("# CREATE INSTANCE: %s[CON:%s]\n", argURI, argCON);

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

		STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s", argURI);

		STRINIT(reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF));
		SNPRINTF(reqParam.xM2M_CNF, sizeof(reqParam.xM2M_CNF), "%s:0", HTTP_CONTENT_TYPE_TEXT);

		STRINIT(reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON));
		SNPRINTF(reqParam.xM2M_CON, sizeof(reqParam.xM2M_CON), "%s", argCON);

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
		deviceID 	= GRIB_CMD_ARG1[argv];
		argURI 		= GRIB_CMD_ARG2[argv];

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

		STRINIT(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI));
		SNPRINTF(reqParam.xM2M_URI, sizeof(reqParam.xM2M_URI), "%s", argURI);

		iRes = Grib_ContentInstanceRetrieve(&reqParam, &resParam);

		GRIB_LOGD("\n");
		GRIB_LOGD(GRIB_1LINE_SHARP);

		if(iRes == GRIB_DONE)
		{
			strUpper(mainMenu);
			strUpper(subMenu);

			GRIB_LOGD("# RETRIEVE CONTENT INST RECV[%d]:\n%s\n", STRLEN(resParam.http_RecvData), resParam.http_RecvData);
		}
		else
		{
			GRIB_LOGD("# RETRIEVE CONTENT INSTANCE: %s [%d]\n", resParam.http_ResMsg, resParam.http_ResNum);			
		}
		
		GRIB_LOGD(GRIB_1LINE_SHARP);
	}

	return;
}

void Grib_MenuAuth(int argc, char **argv)
{
	const char* FUNC = "# GRIB-AUTH:";

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
		GRIB_LOGD("%s GATEWAY ID: %s\n", FUNC, pID);

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
		GRIB_LOGD("%s DEVICE ID: %s\n", FUNC, pID);

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
		GRIB_LOGD("%s DEVICE ID: %s\n", FUNC, pID);

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
		GRIB_LOGD("%s GATEWAY ID: %s\n", FUNC, pID);

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

void Grib_MenuTest(int argc, char **argv)
{
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* subMenu = NULL;

	char* deviceID = NULL;

	subMenu = GRIB_CMD_SUB[argv];

	if(STRCASECMP(subMenu, "smd") == 0)
	{
		char  smdBuff[HTTP_MAX_SIZE_RECV_MSG] = {0x00, };

		if(STRLEN(argv[GRIB_CMD_ARG1]) <= 0)
		{
			GRIB_LOGD("# IN-VALID DEVICE ID !!!\n");
			GRIB_LOGD("# INPUT DEVICE ID: %s[%d]\n", argv[GRIB_CMD_ARG1], STRLEN(argv[GRIB_CMD_ARG1]));
			return;
		}
		else
		{
			deviceID = argv[GRIB_CMD_ARG1];
		}
		Grib_SmdGetDeviceInfo(deviceID, smdBuff);
		GRIB_LOGD("# SEMANTIC DESCRIPTOR[%d]:\n%s\n", STRLEN(smdBuff), smdBuff);

		return;
	}

	if(STRCASECMP(subMenu, "print") == 0)
	{
		//long stackLimit = Grib_GetStackLimit();

			printf("%c[1;32mGREEN COLOR PRINT%c[0m\n",27, 27);
			printf("%c[1;33mYELLOW COLOR PRINT%c[0m\n",27, 27);

            printf("NORMAL COLOR PRINT"); 

		return;
	}

	if(STRCASECMP(subMenu, "ebase64") == 0)
	{
		char  encBuff[HTTP_MAX_SIZE_RECV_MSG] = {0x00, };
		char* srcText = NULL;

		if(STRLEN(argv[GRIB_CMD_ARG1]) <= 0)
		{
			srcText = 	"{\r\n" \
						"    \"exec_id\" :\"0123456789abcdefABCDEF_~+-=\",\r\n" \
						"    \"data\" : \"1\",\r\n" \
						"}";
		}
		else
		{
			srcText = argv[GRIB_CMD_ARG1];
		}
		Grib_Base64Encode(srcText, encBuff, GRIB_NOT_USED);

		GRIB_LOGD(GRIB_1LINE_SHARP);
		GRIB_LOGD("# SRC[%d]:\n%s\n", STRLEN(srcText), srcText);
		GRIB_LOGD(GRIB_1LINE_SHARP);
		GRIB_LOGD("\n");
		GRIB_LOGD(GRIB_1LINE_SHARP);
		GRIB_LOGD("# ENC[%d]:\n%s\n", STRLEN(encBuff), encBuff);
		GRIB_LOGD(GRIB_1LINE_SHARP);
		
		return;
	}

	if(STRCASECMP(subMenu, "dbase64") == 0)
	{
		char  decBuff[HTTP_MAX_SIZE_RECV_MSG] = {0x00, };
		char* srcText = NULL;

		if(STRLEN(argv[GRIB_CMD_ARG1]) <= 0)
		{
			srcText = "R3JpYiBUZXN0IEJhc2UgNjQgVGV4dA=="; //shbaek: "Grib Test Base 64 Text"
		}
		else
		{
			srcText = argv[GRIB_CMD_ARG1];
		}
		Grib_Base64Decode(srcText, decBuff, GRIB_NOT_USED);

		GRIB_LOGD(GRIB_1LINE_SHARP);
		GRIB_LOGD("# SRC[%d]:\n%s\n", STRLEN(srcText), srcText);
		GRIB_LOGD(GRIB_1LINE_SHARP);
		GRIB_LOGD("\n");
		GRIB_LOGD(GRIB_1LINE_SHARP);
		GRIB_LOGD("# DEC[%d]:\n%s\n", STRLEN(decBuff), decBuff);
		GRIB_LOGD(GRIB_1LINE_SHARP);
		
		return;
	}

	if(STRCASECMP(subMenu, "crc") == 0)
	{
		int i = 0;
		byte crc = NULL;
		char* sNum = argv[GRIB_CMD_ARG1];

		for(i=0; i<STRLEN(sNum); i++)
		{
			GRIB_LOGD("# CRC[%d/%d]: 0x%x + 0x%x", i+1, STRLEN(sNum), crc, sNum[i]);
			crc += sNum[i];
			GRIB_LOGD(" = 0x%x\n", crc);
		}

	}

	if(STRCASECMP(subMenu, "bin") == 0)
	{
		int   binSize = 0;
		char* hexBuff = argv[GRIB_CMD_ARG1];
		char* binBuff = NULL;

		binSize = STRLEN(hexBuff)/2;

		binBuff = (char*) MALLOC(binSize);

		Grib_HexToBin(hexBuff, binBuff, STRLEN(hexBuff));

		GRIB_LOGD("# HEX BUFF: %s [%d]\n", hexBuff, STRLEN(hexBuff));
		Grib_PrintOnlyHex(binBuff, binSize);
	}

	if(STRCASECMP(subMenu, "cmd") == 0)
	{
		const char* FUNC = "CMD-TEST";

		const char* JSON0 = "application/json:0";
		const char* JSON1 = "application/json:1";
		const char* TEXT0 = "text/plain:0";
		const char* TEXT1 = "text/plain:1";

		const char* JCON0 = "{\r\n" \
						   "    \"exec_id\": \"0123456789abcdefABCDEF_~+-=\",\r\n" \
						   "    \"data\" : \"1\",\r\n" \
						   "}";
		const char* JCON1 = "ew0KICAgICJleGVjX2lkIiA6IjAxMjM0NTY3ODlhYmNkZWZBQkNERUZffistPSIsDQogICAgImRhdGEiIDogIjEiLA0KfQ==";
		const char* TCON0 = "1";
		const char* TCON1 = "MQ==";

		OneM2M_ResParam resParam;
		char* arg1 = NULL;

		if(GRIB_CMD_ARG1 < argc)
		{
			arg1 = argv[GRIB_CMD_ARG1];
		}
		else
		{
			arg1 = "0";
		}

		GRIB_LOGD("# %s: SUB MENU: %s\n", FUNC, arg1);

		MEMSET(&resParam, 0x00, sizeof(OneM2M_ResParam));

		if(STRCASECMP(arg1, "JSON0") == 0)
		{
			STRNCPY(resParam.xM2M_ContentInfo, JSON0, STRLEN(JSON0));
			STRNCPY(resParam.xM2M_Content, JCON0, STRLEN(JCON0));
		}
		if(STRCASECMP(arg1, "JSON1") == 0)
		{
			STRNCPY(resParam.xM2M_ContentInfo, JSON1, STRLEN(JSON1));
			STRNCPY(resParam.xM2M_Content, JCON1, STRLEN(JCON1));
		}
		if(STRCASECMP(arg1, "TEXT0") == 0)
		{
			STRNCPY(resParam.xM2M_ContentInfo, TEXT0, STRLEN(TEXT0));
			STRNCPY(resParam.xM2M_Content, TCON0, STRLEN(TCON0));
		}
		if(STRCASECMP(arg1, "TEXT1") == 0)
		{
			STRNCPY(resParam.xM2M_ContentInfo, TEXT1, STRLEN(TEXT1));
			STRNCPY(resParam.xM2M_Content, TCON1, STRLEN(TCON1));
		}

		Grib_CmdRequestParser(&resParam);

		Grib_InfoLog(FUNC, resParam.xM2M_ContentInfo);
		Grib_InfoLog(FUNC, resParam.cmdReq_ExecID);
		Grib_InfoLog(FUNC, resParam.xM2M_Content);
		GRIB_LOGD("# %s: DONE  ...\n", FUNC);

		return;
	}

	if(STRCASECMP(subMenu, "dns") == 0)
	{
		char* url = argv[GRIB_CMD_ARG1];
		hostent* pHost = NULL;
		int i = 0;

		pHost = gethostbyname(url);
		if(!pHost)
		{
			Grib_ErrLog("TEST-DNS", "GET HOST NAME FAIL !!!");
			return;
		}

		for(i=0; pHost->h_addr_list[i]!=NULL; i++)
		{
			const char* ipAddr = inet_ntoa( *(struct in_addr*)pHost->h_addr_list[i]);

			GRIB_LOGD("# TEST-DNS: HOST[%02d] NAME: %s\n", i, pHost->h_name);
			GRIB_LOGD("# TEST-DNS: HOST[%02d] ADDR: %s\n", i, ipAddr);
			GRIB_LOGD("\n");
		}

		//inet_ntoa( *(struct in_addr*)pHost->h_addr_list[i])

		return;
	}

#ifdef FEATURE_CAS
	if(STRCASECMP(subMenu, "cas") == 0)
	{
		Grib_CasTest(argc, argv);
		GRIB_LOGD("# TEST-CAS: DONE ...\n");

		return;
	}
#endif

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
		printf(GRIB_1LINE_SHARP);
		printf("# HUB VERSION : %s\n", GRIB_HUB_VERSION);
		printf("# COMPILE TIME: %s-%s\n", __DATE__, __TIME__);
		printf(GRIB_1LINE_SHARP);

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
