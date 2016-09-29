/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <pthread.h>

#include "include/grib_define.h"
#include "include/grib_onem2m.h"

#include "include/grib_db.h"
#include "include/grib_ble.h"
#include "include/grib_regi.h"
#include "include/grib_thread.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
typedef enum 
{//shbaek: Command Index
	GRIB_CMD_NAME		= 0, //shbaek: ./grib
	GRIB_CMD_MAIN		= 1, //shbaek: start, regi, debug, test, ...
	GRIB_CMD_SUB		= 2, 
	GRIB_CMD_ARG1		= 3,
	GRIB_CMD_ARG2		= 4,	
	GRIB_CMD_ARG3		= 5,
	GRIB_CMD_ARG4		= 5,
	GRIB_CMD_MAX
}Grib_CmdIndex;

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
	int iRes = GRIB_ERROR;
	Grib_ConfigInfo pConfigInfo;

	MEMSET(&pConfigInfo, 0x00, sizeof(Grib_ConfigInfo));

	iRes = Grib_LoadConfig(&pConfigInfo);

	return;
}

void Grib_MenuHub(int argc, char **argv)
{
	//3 shbaek: MAIN FUNCTION
	GRIB_LOGD("# ##### ##### ##### ##### ##### GRIB START ##### ##### ##### ##### #####\n");
	GRIB_LOGD("# COMPILE TIME : %s %s\n", __DATE__, __TIME__);

	Grib_ThreadStart();
	return;
}

void Grib_MenuRegi(int argc, char **argv)
{
	char* deviceAddr = NULL;

	//shbaek: You Must be Set Server Config.
	Grib_SetServerConfig();

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# USAGE: grib regi \"DEVICE ADDR\"\n");
		return;
	}

	deviceAddr = argv[GRIB_CMD_SUB];
	if(STRNCASECMP(deviceAddr, ONEM2M_URI_CONTENT_HUB, STRLEN(ONEM2M_URI_CONTENT_HUB))==0)
	{//shbaek: Regi Hub Self
		Grib_HubRegi();
		return;
	}
	else
	{//shbaek: Regi BLE Device
		Grib_DeviceRegi(deviceAddr);
	}

	return;
}

void Grib_MenuDeRegi(int argc, char **argv)
{
	char* deviceID = NULL;

	//shbaek: You Must be Set Server Config.
	Grib_SetServerConfig();

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# USAGE: grib regi \"DEVICE ID\"\n");
		return;
	}

	deviceID = argv[GRIB_CMD_SUB];
	Grib_DeviceDeRegi(deviceID);
	return;
}

void Grib_MenuBle(int argc, char **argv)
{
	int	  iRes = GRIB_ERROR;
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* subMenu = NULL;
	char* pSendMsg = NULL;

	char* deviceID = NULL;
	char* deviceAddr = NULL;

	subMenu = GRIB_CMD_SUB[argv];
	
	if(STRCASECMP(subMenu, "init") == 0)
	{//shbaek: Avoid Permission Error
		char* pCommand	= "sudo ./grib ble sinit";
		char  pLineBuffer[SIZE_1M] = {'\0', };
		int   iSkipCount = 0;

		iRes = systemCommand(pCommand, pLineBuffer, sizeof(pLineBuffer));
		GRIB_LOGD("# COMMAND RESULT[%d]:\n\n%s\n", STRLEN(pLineBuffer),pLineBuffer+iSkipCount);
		return;
	}
	if(STRCASECMP(subMenu, "scan") == 0)
	{//shbaek: Avoid Permission Error
		char* pCommand	= "sudo ./grib ble sscan";
		char  pLineBuffer[SIZE_1M] = {'\0', };
		int   iSkipCount = 0;

		GRIB_LOGD("# BLE SCANNING ...\n");
		iRes = systemCommand(pCommand, pLineBuffer, sizeof(pLineBuffer));
		GRIB_LOGD("# COMMAND RESULT[%d]:\n%s\n", STRLEN(pLineBuffer), pLineBuffer+iSkipCount);
		return;
	}
	if(STRCASECMP(subMenu, "sinit") == 0)
	{
		Grib_BleDeviceInit();
	}
	if(STRCASECMP(subMenu, "sscan") == 0)
	{
		int   iScanCount = 100;//shbaek: Default

		if(4 < argc)iScanCount = ATOI(GRIB_CMD_ARG1[argv]);
		//GRIB_LOGD("                             \n");

		Grib_BleDeviceScan(iScanCount);
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
		Grib_DbRowDeviceInfo rowDeviceInfo;
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
			GRIB_LOGD("# DEVICE IF     : %s\n", Grib_InterfaceToStr((Grib_DeviceIfType)pRowDeviceInfo->deviceInterface));
			GRIB_LOGD("# DEVICE ADDR   : %s\n", pRowDeviceInfo->deviceAddr);
			GRIB_LOGD("# DEVICE LOC    : %s\n", pRowDeviceInfo->deviceLoc);
			GRIB_LOGD("# DEVICE DESC   : %s\n", pRowDeviceInfo->deviceDesc);
			GRIB_LOGD("# REPORT CYCLE  : %d\n", pRowDeviceInfo->reportCycle);
			GRIB_LOGD("# DEVICE FCOUNT : %d\n", pRowDeviceInfo->deviceFuncCount);

			for(x=0; x<iFuncCount; x++)
			{
				pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[x];
				GRIB_LOGD("# FUNC INFO[%d/%d]: [%s][%s][0x%x]\n", x+1, iFuncCount, pRowDeviceFunc->funcName, pRowDeviceFunc->exRsrcID, pRowDeviceFunc->funcAttr);
				//GRIB_LOGD("# NAME/RI/ATTR : [%s][%s][0x%x]\n", pRowDeviceFunc->funcName, pRowDeviceFunc->exRsrcID, pRowDeviceFunc->funcAttr);
			}
		}
		GRIB_LOGD("\n");
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
	int 	iRes = GRIB_ERROR;

	char* 	mainMenu = NULL;
	char* 	subMenu  = NULL;

	char* 	argNM = NULL;
	char* 	argURI = NULL;
	char* 	argCON = NULL;
	char* 	argOrigin = NULL;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	//shbaek: You Must be Set Server Config.
	Grib_SetServerConfig();

	if(argc < GRIB_CMD_SUB+1)
	{
		GRIB_LOGD("# NO SUB MENU\n");
		return;
	}

	mainMenu = GRIB_CMD_MAIN[argv];
	subMenu  = GRIB_CMD_SUB[argv];

	if(STRCASECMP(subMenu, "cappentity") == 0)
	{
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m cappentity \"DEVICE ID\"\n");
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
	if(STRCASECMP(subMenu, "rappentity") == 0)
	{
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m rappentity \"DEVICE ID\"\n");
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
	if(STRCASECMP(subMenu, "dappentity") == 0)
	{
		if(argc < GRIB_CMD_ARG1+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m dappentity \"DEVICE ID\"\n");
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
	if(STRCASECMP(subMenu, "ccontainer") == 0)
	{
		if(argc < GRIB_CMD_ARG3+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m ccontainer \"DEVICE ID\" \"URI\" \"NM\"\n");
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
	if(STRCASECMP(subMenu, "rcontainer") == 0)
	{
		if(argc < GRIB_CMD_ARG2+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m rcontainer \"DEVICE ID\" \"URI\"\n");
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
	if(STRCASECMP(subMenu, "dcontainer") == 0)
	{
		if(argc < GRIB_CMD_ARG2+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m dcontainer \"DEVICE ID\" \"URI\"\n");
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
	if(STRCASECMP(subMenu, "cinstance") == 0)
	{
		if(argc < GRIB_CMD_ARG3+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m cinstance \"DEVICE ID\" \"URI\" \"CONTENT\"\n");
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
	if(STRCASECMP(subMenu, "rinstance") == 0)
	{
		if(argc < GRIB_CMD_ARG2+1)
		{
			GRIB_LOGD("# USAGE: grib xm2m rinstance \"DEVICE ID\" \"URI\"\n");
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

void Grib_MenuTest(int argc, char **argv)
{
	int   iDelay = 0;
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* subMenu = NULL;
	char* pSendMsg = NULL;

	char* deviceID = NULL;
	char* deviceAddr = NULL;
	


	subMenu = GRIB_CMD_SUB[argv];
	
	if(STRCASECMP(subMenu, "loop") == 0)
	{
		if(argc < GRIB_CMD_ARG4+1)
		{
			GRIB_LOGD("# USAGE: grib ble send \"DEVICE ADDR\" \"DEVICE ID\" \"SEND MSG\" \"DELAY SEC\" \n");
			return;
		}

		deviceAddr	= GRIB_CMD_ARG1[argv];
		deviceID	= GRIB_CMD_ARG2[argv];
		pSendMsg	= GRIB_CMD_ARG3[argv];
		iDelay		= ATOI(GRIB_CMD_ARG4[argv])+1;

		while(TRUE)
		{
			Grib_BleSendMsg(deviceAddr, deviceID, pSendMsg, recvBuff);
			SLEEP(iDelay);
		}

		//GRIB_LOGD("# SEND MSG[%03d]: %s\n", STRLEN(pSendMsg), pSendMsg);
		//GRIB_LOGD("# RECV MSG[%03d]: %s\n", STRLEN(recvBuff), recvBuff);
	}

	if(STRCASECMP(subMenu, "info") == 0)
	{
		Grib_HubRegi();
		Grib_SetServerConfig();
		Grib_UpdateHubInfo();
		return;
	}

	return;
}

int main(int argc, char **argv)
{
	char* mainMenu = NULL;

	if(argc < GRIB_CMD_MAIN+1)
	{
		Grib_MenuHelp();
		return GRIB_DONE;
	}
	mainMenu = GRIB_CMD_MAIN[argv];

	//for(i=0; i<argc; i++)GRIB_LOGD("# ARG[%d/%d]: %s\n", i, argc, argv[i]);

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
	if(STRCASECMP(mainMenu, "test") == 0)
	{
		Grib_MenuTest(argc, argv);
		return GRIB_DONE;
	}

	GRIB_LOGD("# INVALID MENU\n");
	GRIB_LOGD("\n");

	Grib_MenuHelp();

	return GRIB_ERROR;
}
