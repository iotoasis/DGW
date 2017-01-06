/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_ble.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int gDebugBle = FALSE;
int gBleTombStone = FALSE;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

int Grib_BleConfig(void)
{
	int iRes = GRIB_ERROR;
	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("GET CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	gDebugBle  = pConfigInfo->debugBLE;
	gBleTombStone = pConfigInfo->tombStoneBLE;

	if(gDebugBle)GRIB_LOGD("# BLE CONFIG DEBUG LOG: %d\n", gDebugBle);
	if(gDebugBle)GRIB_LOGD("# BLE CONFIG TOMBSTONE: %d\n", gBleTombStone);

	return GRIB_SUCCESS;
}

void Grib_BleTombStone(Grib_BleLogInfo* pLogInfo)
{
	time_t sysTimer;
	struct tm *sysTime;

	sysTimer = time(NULL);
	sysTime  = localtime(&sysTimer);

	int   iFD = 0;
    char  pLogFilePath[SIZE_1K] = {'\0', };
	char* pMsg = NULL;

	if(gBleTombStone != TRUE)
	{//shbaek: Use Not TombStone
		return;
	}

    SNPRINTF(pLogFilePath, sizeof(pLogFilePath), "%s/BLE_%04d%02d%02d_%02d%02d%02d_%s.log", BLE_FILE_PATH_LOG_ROOT, 
												 sysTime->tm_year+1900, sysTime->tm_mon+1, sysTime->tm_mday,
												 sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec, pLogInfo->blePipe);

	GRIB_LOGD("# %s-TOMB: # STONE NAME: %s\n", pLogInfo->blePipe, pLogFilePath);

	iFD = open(pLogFilePath, O_WRONLY|O_CREAT, 0666);

	pMsg = "BLE NAME : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->blePipe, STRLEN(pLogInfo->blePipe));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "BLE ADDR : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleAddr, STRLEN(pLogInfo->bleAddr));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "REQ BUFF : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleSendMsg, STRLEN(pLogInfo->bleSendMsg));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "RES BUFF : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleRecvMsg, STRLEN(pLogInfo->bleRecvMsg));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "ERR CODE : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleErrorMsg, STRLEN(pLogInfo->bleErrorMsg));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	close(iFD);
}

int Grib_BleCleanAll(void)
{
	int iDBG = gDebugBle;
	const char* FUNC_TAG = "BLE-CLEAN";

	char pFilePath[SIZE_1K] = {'\0', };

	DIR* pDirInfo = NULL;
	struct dirent* pDirEntry = NULL;

	if(access(BLE_FILE_PATH_LOG_ROOT, F_OK) != 0)
	{
		GRIB_LOGD("# %s: CREATE LOG ROOT DIR\n", FUNC_TAG);

		mkdir(BLE_FILE_PATH_LOG_ROOT, 0755);
	}

	if(access(BLE_FILE_PATH_PIPE_ROOT, F_OK) != 0)
	{
		GRIB_LOGD("# %s: CREATE PIPE ROOT DIR\n", FUNC_TAG);
		mkdir(BLE_FILE_PATH_PIPE_ROOT, 0755);

		//shbaek: Need Not Clean.
		return GRIB_DONE;
	}

	pDirInfo = opendir(BLE_FILE_PATH_PIPE_ROOT);
	if(pDirInfo == NULL)
	{//shbaek: Aleady Exist
		GRIB_LOGD("# %s: OPEN PIPE ROOT DIR: %s\n", FUNC_TAG, BLE_FILE_PATH_PIPE_ROOT);
		return GRIB_ERROR;
	}

	while( (pDirEntry=readdir(pDirInfo)) != NULL)
	{
		STRINIT(pFilePath, sizeof(pFilePath));
		SNPRINTF(pFilePath, sizeof(pFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pDirEntry->d_name);
		if(iDBG)GRIB_LOGD("# %s: DIR ENTRY NAME: %s\n", FUNC_TAG, pFilePath);

		unlink(pFilePath);
	}

	GRIB_LOGD("# %s: DONE\n", FUNC_TAG);
	if(pDirInfo != NULL)
	{
		closedir(pDirInfo);
	}
	
	return GRIB_DONE;
}

int Grib_BlePipeReCreate(char *pipeFilePath)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugBle;
	int iReTry = 0;
	const int MAX_RETRY_COUNT_CREATE_PIPE = 10;

	DIR* pDirInfo = NULL;
	const char* FUNC_TAG = "BLE-PIPE";

	if(access(pipeFilePath, F_OK) == 0)
	{
		if(iDBG)GRIB_LOGD("# %s: DELETE OLD PIPE: %s\n", FUNC_TAG, pipeFilePath);
		unlink(pipeFilePath);
	}

	do
	{//3 shbaek: Give to Create Chance [MAX_RETRY_COUNT_CREATE_PIPE]
		iRes = mkfifo(pipeFilePath, 0666);
		if(iRes == GRIB_DONE)
		{//shbaek: Open Success
			break;
		}

		//shbaek: Must be Create Pipe File.
		iReTry++;
		GRIB_LOGD("# %s: CREATE PIPE(%s) FAIL: [RETRY: %d] [MSG: %s(%d)]\n", FUNC_TAG, pipeFilePath, iReTry, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		SLEEP(1);
	}while(iReTry < MAX_RETRY_COUNT_CREATE_PIPE);

	if(iDBG)GRIB_LOGD("# %s: CREATE NEW PIPE: %s\n", FUNC_TAG, pipeFilePath);

	if(pDirInfo != NULL)
	{
		closedir(pDirInfo);
	}

	return iRes;
}

int Grib_BleSendMsg(char* deviceAddr, char *pipeFileName, char* sendBuff, char* recvBuff)
{
	int iRes = GRIB_DONE;
	int iDBG = gDebugBle;
	int iCount = GRIB_INIT;
	int iTotal = GRIB_INIT;
	int iStatus = GRIB_ERROR;
	int pipeFileFD = GRIB_ERROR;

	char pipeFilePath[SIZE_1K] = {'\0', };

	pid_t processID = GRIB_ERROR;

	const char* FUNC_TAG = "BLE-SEND";

	if(deviceAddr==NULL || sendBuff==NULL || recvBuff==NULL)
	{
		GRIB_LOGD("# %s: PARAM IS NULL\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	if(pipeFileName == NULL)
	{//shbaek: for TEST
		pipeFileName = TEST_BLE_PIPE_FILE_NAME;
	}

	if(iDBG)
	{
		GRIB_LOGD("# %s-BLE : DEVICE ADDR   : %s\n", pipeFileName, deviceAddr);
		GRIB_LOGD("# %s-BLE : PIPE FILE     : %s\n", pipeFileName, pipeFileName);
	}
	GRIB_LOGD("# %s-BLE : SEND MSG[%03d]: %s\n", pipeFileName, STRLEN(sendBuff), sendBuff);

	processID = fork();
	if(processID == GRIB_ERROR)
	{
		GRIB_LOGD("# %s-BLE : PROCESS FORK FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("# %s-BLE : FORK PROCESS ID: %d\n", pipeFileName, processID);

	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pipeFileName);
	if(iDBG)GRIB_LOGD("# %s-BLE : PIPE FILE PATH: %s\n", pipeFileName, pipeFilePath);

	switch(processID)
	{
		case 0:
		{
#if (FEATURE_GRIB_BLE_EX==ON)
			//3 shbaek: Jump to Blecomm
			if(iDBG)GRIB_LOGD("# %s-SEND[CHILD]: JUMP BLE EXTEND\n", pipeFileName);
			iRes = execl(BLE_FILE_PATH_BLECOMM_PROGRAM, BLE_FILE_PATH_BLECOMM_PROGRAM, deviceAddr, pipeFilePath, sendBuff, NULL);
#else
			//3 shbaek: Jump to Python
			if(iDBG)GRIB_LOGD("# %s-SEND[CHILD]: JUMP BLE PYTHON\n", pipeFileName);
			iRes = execl(BLE_FILE_PATH_PYTHON_PROGRAM, BLE_FILE_PATH_PYTHON_PROGRAM, BLE_FILE_PATH_PYTHON_SCRIPT, deviceAddr, pipeFilePath, sendBuff, NULL);
#endif
			if(iDBG)GRIB_LOGD("# %s-SEND[CHILD]: DO YOU SEE ME???\n");
			exit(iRes);
			break;
		}

		default:
		{
			int iWaitTry = 0;
			const int WAIT_TRY_MAX = 5;
			const int WAIT_TRY_DELAY = 3;

			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: CREATE PIPE\n", pipeFileName);

			iRes = Grib_BlePipeReCreate(pipeFilePath);
			if(iRes == GRIB_FAIL)
			{
				GRIB_LOGD("# %s-SEND[PARENT]: CREATE PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: OPEN PIPE\n", pipeFileName);
			// Open PIPE File(Use Only Read Buffer)
			pipeFileFD = open(pipeFilePath, O_RDONLY);
			if(pipeFileFD < 0)
			{
				GRIB_LOGD("# %s-SEND[PARENT]: OPEN PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			iTotal = iCount = GRIB_INIT;
			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: WAITING FOR CHILD MSG ...\n", pipeFileName);
			SLEEP(1);
			do
			{
				iCount = read(pipeFileFD, recvBuff+iTotal, BLE_MAX_SIZE_RECV_MSG-iTotal);
				if(iCount < 0)
				{
					GRIB_LOGD("# %s-SEND[PARENT]: READ FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
					break;
				}
				else if(iCount == 0)
				{
					if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: READ DONE\n", pipeFileName);
					break;
				}
				else
				{
					iTotal += iCount;
					if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: READ:%d TOTAL:%d\n", pipeFileName, iCount, iTotal);
				}

			}while(iTotal < BLE_MAX_SIZE_RECV_MSG);
			//iRes = kill(processID, SIGKILL);
			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: WAITING FOR RETURN CHILD[PID:%d]\n", pipeFileName, processID);

			do{
				processID = wait(&iStatus);
				if(processID != GRIB_FAIL)
				{//shbaek: OK
					break;
				}

				if(WAIT_TRY_MAX < iWaitTry)
				{//shbaek: Occur Critical Error, Will Defunct
					GRIB_LOGD("# %s-SEND[PARENT]: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					GRIB_LOGD("# %s-SEND[PARENT]: #####       CRITICAL ERROR          #####\n", pipeFileName);
					GRIB_LOGD("# %s-SEND[PARENT]: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					break;
				}

				iWaitTry++;
				GRIB_LOGD("# %s-SEND[PARENT]: WAIT FAIL RE-TRY: %d\n", pipeFileName, iWaitTry);
				SLEEP(WAIT_TRY_DELAY);
			}while(TRUE);

			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: CHILD IS DONE PID:%d STATUS:%d\n", pipeFileName, processID, iStatus);

			if(0 < pipeFileFD)
			{
				close(pipeFileFD);
				pipeFileFD = GRIB_ERROR;
			}

			break;
		}
	}

	GRIB_LOGD("# %s-BLE : READ MSG[%03d]: %s\n", pipeFileName, STRLEN(recvBuff), recvBuff);

	if(STRNCASECMP(recvBuff, BLE_RESPONSE_STR_ERROR, STRLEN(BLE_RESPONSE_STR_ERROR)) == 0)
	{
		Grib_BleErrorCode iError = (Grib_BleErrorCode) ATOI(&STRCHR(recvBuff, GRIB_COLON)[1]);
		const char* pError = Grib_BleErrorToStr(iError);

		Grib_BleLogInfo bleLogInfo;
		MEMSET(&bleLogInfo, 0x00, sizeof(Grib_BleLogInfo));

		bleLogInfo.blePipe		= pipeFileName;
		bleLogInfo.bleAddr		= deviceAddr;
		bleLogInfo.bleSendMsg	= sendBuff;
		bleLogInfo.bleRecvMsg	= recvBuff;
		bleLogInfo.bleErrorMsg	= pError;

		GRIB_LOGD("# %s-BLE : # ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);

		Grib_BleTombStone(&bleLogInfo);

		GRIB_LOGD("# %s-BLE : # ERROR MSG : %s[%d]\n", pipeFileName, pError, iError);
		GRIB_LOGD("# %s-BLE : # ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);

		if(iError == BLE_ERROR_CODE_CRITICAL)
		{//3 shbaek: HCI DRIVER RESET
			Grib_BleDetourInit();
		}

		iRes = GRIB_FAIL;
	}

	//shbaek: Need Interval Time, After Pipe Close.
	unlink(pipeFilePath);

	return iRes;
}

int Grib_BleDetourInit(void)
{//shbaek: Avoid Permission Error
	int   iRes = GRIB_ERROR;
	int   iSkipCount = 0;

	const char* pCommand	= "sudo ./" BLE_GRIB_HCI_FILE_NAME " " BLE_GRIB_HCI_MENU_INIT;
	char  pLineBuffer[SIZE_1M] = {'\0', };

	iRes = systemCommand(pCommand, pLineBuffer, sizeof(pLineBuffer));
	GRIB_LOGD("# BLE DETOUR INIT RESULT[%d]:\n%s\n", STRLEN(pLineBuffer),pLineBuffer+iSkipCount);
	return iRes;
}

int Grib_BleDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo)
{
	int   i = 0;
	int	  iRes = GRIB_ERROR;
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* pSplitPoint = NULL;

	char* deviceID = NULL;
	char* deviceAddr = NULL;
	
	int    funcCount = 0;

	Grib_DbRowDeviceFunc* pRowDeviceFunc;

	deviceAddr = pRowDeviceInfo->deviceAddr;

	STRINIT(pRowDeviceInfo->deviceID, sizeof(pRowDeviceInfo->deviceID));
	iRes = Grib_BleGetDeviceID(deviceAddr, pRowDeviceInfo->deviceID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET DEVICE ID FAIL: %s\n", pRowDeviceInfo->deviceID);
		return GRIB_FAIL;
	}
	if(STRLEN(pRowDeviceInfo->deviceID) < 3) 
	{
		GRIB_LOGD("# BLE-INFO: DEVICE ID TOO SHORT: %d\n", STRLEN(pRowDeviceInfo->deviceID));
		return GRIB_ERROR;
	}

	deviceID = pRowDeviceInfo->deviceID;
	pRowDeviceInfo->deviceInterface = DEVICE_IF_TYPE_BLE;

	STRINIT(pRowDeviceInfo->deviceLoc, sizeof(pRowDeviceInfo->deviceLoc));
	iRes = Grib_BleGetDeviceLoc(deviceAddr, deviceID, pRowDeviceInfo->deviceLoc);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET DEVICE LOC FAIL: %s\n", pRowDeviceInfo->deviceLoc);
		return GRIB_FAIL;
	}
	//pRowDeviceInfo->deviceLoc;

	STRINIT(pRowDeviceInfo->deviceDesc, sizeof(pRowDeviceInfo->deviceDesc));
	iRes = Grib_BleGetDeviceDesc(deviceAddr, deviceID, pRowDeviceInfo->deviceDesc);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET DEVICE DESC FAIL: %s\n", pRowDeviceInfo->deviceDesc);
		return GRIB_FAIL;
	}
	//pRowDeviceInfo->deviceDesc;

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetReportCycle(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET REPORT CYCLE FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}
	pRowDeviceInfo->reportCycle = ATOI(recvBuff);

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetFuncCount(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET FUNC COUNT FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}
	funcCount = pRowDeviceInfo->deviceFuncCount = ATOI(recvBuff);

	pRowDeviceInfo->ppRowDeviceFunc = (Grib_DbRowDeviceFunc**)MALLOC(funcCount*sizeof(Grib_DbRowDeviceFunc*));
	for(i=0; i<funcCount; i++)
	{
		pRowDeviceInfo->ppRowDeviceFunc[i] = (Grib_DbRowDeviceFunc*)MALLOC(sizeof(Grib_DbRowDeviceFunc));
	}

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetFuncName(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET FUNC NAME FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}

	for(i=0; i<funcCount; i++)
	{
		pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];
		pSplitPoint = Grib_Split(recvBuff, GRIB_COMMA, i);

		STRINIT(pRowDeviceFunc->funcName, sizeof(pRowDeviceFunc->funcName));
		STRNCPY(pRowDeviceFunc->funcName, pSplitPoint, STRLEN(pSplitPoint));
	}

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetFuncAttr(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET FUNC ATTR FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}

	for(i=0; i<funcCount; i++)
	{
		pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];
		pSplitPoint = Grib_Split(recvBuff, GRIB_COMMA, i);

		pRowDeviceFunc->funcAttr = ATOI(pSplitPoint);
	}
	GRIB_LOGD("\n");
	GRIB_LOGD("# ########## ########## ########## ########## ########## ##########\n");
	GRIB_LOGD("# DEVICE ID     : %s\n", pRowDeviceInfo->deviceID);
	GRIB_LOGD("# DEVICE ADDR   : %s\n", pRowDeviceInfo->deviceAddr);
	GRIB_LOGD("# DEVICE LOC    : %s\n", pRowDeviceInfo->deviceLoc);
	GRIB_LOGD("# DEVICE DESC   : %s\n", pRowDeviceInfo->deviceDesc);
	GRIB_LOGD("# REPORT CYCLE  : %d\n", pRowDeviceInfo->reportCycle);
	GRIB_LOGD("# FUNC COUNT    : %d\n", pRowDeviceInfo->deviceFuncCount);
	for(i=0; i<funcCount; i++)
	{
		pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];

		GRIB_LOGD("# FUNC[%d/%d] NAME: %s\n", i+1, funcCount, pRowDeviceFunc->funcName);
		GRIB_LOGD("# FUNC[%d/%d] ATTR: %d [%s]\n", i+1, funcCount, pRowDeviceFunc->funcAttr, Grib_FuncAttrToStr(pRowDeviceFunc->funcAttr));
	}
	GRIB_LOGD("# ########## ########## ########## ########## ########## ##########\n");
	GRIB_LOGD("\n");

	return GRIB_DONE;

}


#define __GRIB_BLE_API__
int Grib_BleGetDeviceID(char* deviceAddr, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pTemp = NULL;
	char* pipeFileName = NULL;
	char pipeFilePath[SIZE_1K] = {'\0', };

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_ID, STRLEN(BLE_CMD_GET_DEVICE_ID));

	pipeFileName = STRDUP(deviceAddr);

	while( (pTemp=STRCHR(pipeFileName, ':')) != NULL )
	{//shbaek: ':' -> '_'
		*pTemp = '_';
	}

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pipeFileName);

	unlink(pipeFilePath);
	FREE(pipeFileName);

	return iRes;
}

int Grib_BleSetDeviceID(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_SET_DEVICE_ID, deviceID);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetDeviceIF(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_IF, STRLEN(BLE_CMD_GET_DEVICE_IF));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}


int Grib_BleGetDeviceLoc(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_LOC, STRLEN(BLE_CMD_GET_DEVICE_LOC));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleSetDeviceLoc(char* deviceAddr, char* deviceID, char* deviceLoc, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_SET_DEVICE_LOC, deviceLoc);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetDeviceDesc(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_DESC, STRLEN(BLE_CMD_GET_DEVICE_DESC));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncCount(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_FUNC_COUNT, STRLEN(BLE_CMD_GET_FUNC_COUNT));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncName(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_FUNC_NAME, STRLEN(BLE_CMD_GET_FUNC_NAME));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncAttr(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_FUNC_ATTR, STRLEN(BLE_CMD_GET_FUNC_ATTR));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetReportCycle(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_REPORT_CYCLE, STRLEN(BLE_CMD_GET_REPORT_CYCLE));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncData(char* deviceAddr, char* deviceID, char* funcName, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_GET_FUNC_DATA, funcName);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleSetFuncData(char* deviceAddr, char* deviceID, char* funcName, char* content, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_SET_FUNC_DATA, funcName, content);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

