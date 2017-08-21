/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_ble.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int gDebugBle = FALSE;
int gBleTombStone = FALSE;

char* GATTTOOL_OPT[GRIB_MAX_SIZE_BRIEF];
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

int Grib_BleDetourInit(void)
{//shbaek: Avoid Permission Error
	int   iRes = GRIB_ERROR;
	int   iSkipCount = 0;

	const char* pCommand	= "sudo ./" BLE_GRIB_HCI_FILE_NAME " " BLE_GRIB_HCI_MENU_INIT;
	char  pLineBuffer[SIZE_1M] = {'\0', };

	iRes = systemCommand(pCommand, pLineBuffer, sizeof(pLineBuffer));
	GRIB_LOGD("# BLE DETOUR INIT RESULT[%d]:\n%s\n", STRLEN(pLineBuffer), pLineBuffer+iSkipCount);
	return iRes;
}

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

	gDebugBle  = pConfigInfo->debugLevel;
	gBleTombStone = pConfigInfo->tombStone;

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


	GRIB_LOGD("# %s-BLE<: # ##### ##### ##### ##### ##### ##### #####\n", pLogInfo->blePipe);
	GRIB_LOGD("# %s-BLE<: # LOG NAME: %c[1;31m%s%c[0m\n", pLogInfo->blePipe, 27, pLogFilePath, 27);
	GRIB_LOGD("# %s-BLE<: # LOG MSG : %c[1;31m%s%c[0m\n", pLogInfo->blePipe, 27, pLogInfo->bleErrorMsg, 27);
	GRIB_LOGD("# %s-BLE<: # ##### ##### ##### ##### ##### ##### #####\n", pLogInfo->blePipe);

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
	const char* FUNC = "BLE-CLEAN";

	char pFilePath[SIZE_1K] = {'\0', };

	DIR* pDirInfo = NULL;
	struct dirent* pDirEntry = NULL;

	if(access(BLE_FILE_PATH_LOG_ROOT, F_OK) != 0)
	{
		GRIB_LOGD("# %s: CREATE LOG ROOT DIR\n", FUNC);

		mkdir(BLE_FILE_PATH_LOG_ROOT, 0755);
	}

	if(access(BLE_FILE_PATH_PIPE_ROOT, F_OK) != 0)
	{
		GRIB_LOGD("# %s: CREATE PIPE ROOT DIR\n", FUNC);
		mkdir(BLE_FILE_PATH_PIPE_ROOT, 0755);

		//shbaek: Need Not Clean.
		return GRIB_DONE;
	}

	pDirInfo = opendir(BLE_FILE_PATH_PIPE_ROOT);
	if(pDirInfo == NULL)
	{//shbaek: Aleady Exist
		GRIB_LOGD("# %s: OPEN PIPE ROOT DIR: %s\n", FUNC, BLE_FILE_PATH_PIPE_ROOT);
		return GRIB_ERROR;
	}

	while( (pDirEntry=readdir(pDirInfo)) != NULL)
	{
		STRINIT(pFilePath, sizeof(pFilePath));
		SNPRINTF(pFilePath, sizeof(pFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pDirEntry->d_name);
		if(iDBG)GRIB_LOGD("# %s: DIR ENTRY NAME: %s\n", FUNC, pFilePath);

		unlink(pFilePath);
	}

	GRIB_LOGD("# %s: DONE\n", FUNC);
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
	const char* FUNC = "BLE-PIPE";

	if(access(pipeFilePath, F_OK) == 0)
	{
		if(iDBG)GRIB_LOGD("# %s: DELETE OLD PIPE: %s\n", FUNC, pipeFilePath);
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
		GRIB_LOGD("# %s: CREATE PIPE(%s) FAIL: [RETRY: %d] [MSG: %s(%d)]\n", FUNC, pipeFilePath, iReTry, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		SLEEP(1);
	}while(iReTry < MAX_RETRY_COUNT_CREATE_PIPE);

	if(iDBG)GRIB_LOGD("# %s: CREATE NEW PIPE: %s\n", FUNC, pipeFilePath);

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

	const char* FUNC = "BLE-SEND";
	char hexBuff[BLE_MAX_SIZE_SEND_MSG*2] = {'\0', };

	if(deviceAddr==NULL || sendBuff==NULL || recvBuff==NULL)
	{
		GRIB_LOGD("# %s: PARAM IS NULL\n", FUNC);
		return GRIB_ERROR;
	}

	if(pipeFileName == NULL)
	{//shbaek: for TEST
		pipeFileName = "TEMP_PIPE_FILE";
	}

	if(iDBG)
	{
		GRIB_LOGD("# %s-BLE>: DEVICE ADDR   : %s\n", pipeFileName, deviceAddr);
		GRIB_LOGD("# %s-BLE>: PIPE FILE     : %s\n", pipeFileName, pipeFileName);
	}
	GRIB_LOGD("# %s-BLE>: %s\n", pipeFileName, sendBuff);

	Grib_StrToHex(sendBuff, hexBuff);
	if(iDBG)GRIB_LOGD("# %s-BLE>: TO HEX: %s\n", pipeFileName, hexBuff);

	processID = fork();
	if(processID == GRIB_ERROR)
	{
		GRIB_LOGD("# %s-BLE>: PROCESS FORK FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("# %s-BLE>: FORK PROCESS ID: %d\n", pipeFileName, processID);

	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pipeFileName);
	if(iDBG)GRIB_LOGD("# %s-BLE>: PIPE FILE PATH: %s\n", pipeFileName, pipeFilePath);

	switch(processID)
	{
		case 0:
		{//shbaek: CHILD PROCESS
			//3 shbaek: Jump to Gatttool
			if(iDBG)GRIB_LOGD("# %s-SEND: JUMP BLE EXTEND\n", pipeFileName);
			iRes = execl(BLE_FILE_PATH_GATTTOOL, BLE_FILE_PATH_GATTTOOL, deviceAddr, pipeFilePath, hexBuff, GATTTOOL_OPT, NULL);

			if(iDBG)GRIB_LOGD("# %s-SEND: DO YOU SEE ME???\n");
			exit(iRes);
			break;
		}

		default:
		{//shbaek: PARENT PROCESS
			int iWaitTry = 0;
			const int WAIT_TRY_MAX = 10;
			const int WAIT_TRY_DELAY = 1;

			if(iDBG)GRIB_LOGD("# %s-SEND: CREATE PIPE\n", pipeFileName);

			iRes = Grib_BlePipeReCreate(pipeFilePath);
			if(iRes == GRIB_FAIL)
			{
				GRIB_LOGD("# %s-SEND: CREATE PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			if(iDBG)GRIB_LOGD("# %s-SEND: OPEN PIPE\n", pipeFileName);
			// Open PIPE File(Use Only Read Buffer)
			pipeFileFD = open(pipeFilePath, O_RDONLY);
			if(pipeFileFD < 0)
			{
				GRIB_LOGD("# %s-SEND: OPEN PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			iTotal = iCount = GRIB_INIT;
			if(iDBG)GRIB_LOGD("# %s-SEND: WAITING FOR CHILD MSG ...\n", pipeFileName);
			//SLEEP(1);
			do
			{
				iCount = read(pipeFileFD, recvBuff+iTotal, BLE_MAX_SIZE_RECV_MSG-iTotal);
				if(iCount < 0)
				{
					GRIB_LOGD("# %s-SEND: READ FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
					break;
				}
				else if(iCount == 0)
				{
					if(iDBG)GRIB_LOGD("# %s-SEND: READ DONE\n", pipeFileName);
					break;
				}
				else
				{
					iTotal += iCount;
					if(iDBG)GRIB_LOGD("# %s-SEND: READ:%d TOTAL:%d\n", pipeFileName, iCount, iTotal);
				}

			}while(iTotal < BLE_MAX_SIZE_RECV_MSG);
			//iRes = kill(processID, SIGKILL);
			if(iDBG)GRIB_LOGD("# %s-SEND: WAITING FOR RETURN CHILD[PID:%d]\n", pipeFileName, processID);

			do{
				processID = wait(&iStatus);
				if(processID != GRIB_FAIL)
				{//shbaek: OK
					break;
				}

				if(WAIT_TRY_MAX < iWaitTry)
				{//shbaek: Occur Critical Error, Will Defunct
					GRIB_LOGD("# %s-SEND: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					GRIB_LOGD("# %s-SEND: #####       %c[1;31mCRITICAL ERROR%c[0m          #####\n", pipeFileName, 27, 27);
					GRIB_LOGD("# %s-SEND: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					break;
				}

				iWaitTry++;
				GRIB_LOGD("# %s-SEND: WAIT FAIL RE-TRY: %d\n", pipeFileName, iWaitTry);
				SLEEP(WAIT_TRY_DELAY);
			}while(TRUE);

			if(iDBG)GRIB_LOGD("# %s-SEND: CHILD IS DONE PID:%d STATUS:%d\n", pipeFileName, processID, iStatus);

			if(0 < pipeFileFD)
			{
				close(pipeFileFD);
				pipeFileFD = GRIB_ERROR;
			}

			break;
		}
	}

	GRIB_LOGD("# %s-BLE<: %s\n", pipeFileName, recvBuff);

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

		Grib_BleTombStone(&bleLogInfo);

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

int Grib_BleSendRaw(Grib_BleMsgInfo* pBleMsg)
{
	const char* FUNC = "BLE-RAW";

	int i = 0;
	int iRes = GRIB_DONE;
	int iDBG = gDebugBle;
	int iCount = GRIB_INIT;
	int iTotal = GRIB_INIT;
	int iStatus = GRIB_ERROR;
	int pipeFileFD = GRIB_ERROR;

	char pipeFilePath[SIZE_1K] = {'\0', };

	pid_t processID = GRIB_ERROR;

	char* deviceAddr = NULL;
	char* pipeFileName = NULL;
	char* sendBuff = NULL;
	char* recvBuff = NULL;

	int    argIndex = 0;
	int    argCount = 0;
	char** argValue = NULL;

	char*  optCommand = NULL;
	char   optDevice[GRIB_MAX_SIZE_BRIEF] = {'\0', };
	char   optHandle[GRIB_MAX_SIZE_BRIEF] = {'\0', };
	char   optValue[GRIB_MAX_SIZE_BRIEF] = {'\0', };
	char   optResponse[GRIB_MAX_SIZE_MIDDLE] = {'\0', };
	char*  optPeerType = BLE_GATT_OPT_PEER_PUBLIC;
	char*  optDebug = NULL;
	char*  optNoWait = NULL;
	char*  optNotiEnable = NULL;
	char*  optListen = NULL;

	if(pBleMsg==NULL || pBleMsg->sendMsg==NULL || pBleMsg->recvMsg==NULL)
	{
		Grib_ErrLog(FUNC, "PARAM IS NULL !!!");
		return GRIB_ERROR;
	}

	if(pBleMsg->pipe == NULL)
	{//shbaek: for TEMP
		pBleMsg->pipe = "TEMP_PIPE_FILE";
	}


	//shbaek: for Reduce Access Point
	deviceAddr   = pBleMsg->addr;
	pipeFileName = pBleMsg->pipe;
	sendBuff     = pBleMsg->sendMsg;
	recvBuff     = pBleMsg->recvMsg;

	if(iDBG)
	{
		GRIB_LOGD("# %s-BLE>: DEVICE ADDR   : %s\n", pipeFileName, deviceAddr);
		GRIB_LOGD("# %s-BLE>: PIPE FILE     : %s\n", pipeFileName, pipeFileName);
	}
	GRIB_LOGD("# %s-BLE>: %s\n", pipeFileName, sendBuff);

	argCount = 2; //shbaek: Base Arg Count (Command + NULL)

	optCommand = BLE_GATT_OPT_WRITE_REQ;
	argCount++;

	SNPRINTF(optDevice, sizeof(optDevice), "%s=%s",   BLE_GATT_OPT_DEVICE, pBleMsg->addr);
	argCount++;
	SNPRINTF(optHandle, sizeof(optHandle), "%s=0x%02X", BLE_GATT_OPT_HANDLE, pBleMsg->handle);
	argCount++;
	SNPRINTF(optValue,  sizeof(optValue),  "%s=%s",   BLE_GATT_OPT_VALUE,  pBleMsg->sendMsg);
	argCount++;
	SNPRINTF(optResponse, sizeof(optResponse), "%s=%s", BLE_GATT_OPT_RESPONSE, pBleMsg->pipe);
	argCount++;

	if(pBleMsg->peerType == BLE_PEER_TYPE_RANDOM)
	{
		optPeerType = BLE_GATT_OPT_PEER_RANDOM;
	}
	argCount++;

	if(pBleMsg->debug == TRUE)
	{
		optDebug = BLE_GATT_OPT_DEBUG;
		argCount++;
	}

	if(pBleMsg->notiEnable == TRUE)
	{
		optNotiEnable = BLE_GATT_OPT_CCC_NOTI;
		argCount++;
	}

	if(pBleMsg->noWait == TRUE)
	{
		optNoWait = BLE_GATT_OPT_NO_WAIT;
		argCount++;
	}

	if(pBleMsg->listen == TRUE)
	{
		optListen = BLE_GATT_OPT_LISTEN;
		argCount++;
	}

	argValue = (char**) MALLOC(sizeof(char*) * argCount);
	for(i=0; i<argCount; i++)
	{
		argValue[i] = (char*) MALLOC(sizeof(char*));
		MEMSET(argValue[i], 0x00, sizeof(char*));
	}

	argIndex = 0;
	argValue[argIndex++] = BLE_FILE_PATH_GATTTOOL_EX;
	argValue[argIndex++] = optPeerType;
	argValue[argIndex++] = optDevice;
	argValue[argIndex++] = optCommand;
	argValue[argIndex++] = optHandle;
	argValue[argIndex++] = optValue;
	argValue[argIndex++] = optResponse;

	if(pBleMsg->listen==TRUE) argValue[argIndex++] = optListen;
	if(pBleMsg->notiEnable==TRUE) argValue[argIndex++] = optNotiEnable;
	if(pBleMsg->noWait==TRUE) argValue[argIndex++] = optNoWait;
	if(pBleMsg->debug==TRUE) argValue[argIndex++] = optDebug;
	argValue[argIndex] = NULL;

	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pipeFileName);
	if(iDBG)
	{
		GRIB_LOGD("# %s-BLE>: PIPE FILE PATH: %s\n", pipeFileName, pipeFilePath);
		GRIB_LOGD("# %s-BLE>: ARG COUNT: %d\n", pipeFileName, argCount);
	}

	processID = fork();
	if(processID == GRIB_ERROR)
	{
		GRIB_LOGD("# %s-BLE>: PROCESS FORK FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("# %s-BLE>: FORK PROCESS ID: %d\n", pipeFileName, processID);

	switch(processID)
	{
		case 0:
		{//shbaek: Child
			//3 shbaek: Jump to Gatttool
			if(iDBG)GRIB_LOGD("# %s-SEND: JUMP BLE EXTEND\n", pipeFileName);

			if(pBleMsg->peerType==BLE_PEER_TYPE_RANDOM)
			{
/*
				iRes = execl(BLE_FILE_PATH_GATTTOOL_EX, BLE_FILE_PATH_GATTTOOL_EX, optPeerType, optDevice,
					optCommand, optHandle, optValue, optResponse, optDebug, NULL);
*/
				iRes = execv(BLE_FILE_PATH_GATTTOOL_EX, argValue);
			}
			else
			{
			}

			if(iDBG)Grib_ErrLog(pipeFileName, "DO YOU SEE ME???\n");
			exit(iRes);
			break;
		}

		default:
		{
			int iWaitTry = 0;
			const int WAIT_TRY_MAX = 5;
			const int WAIT_TRY_DELAY = 3;

			if(iDBG)GRIB_LOGD("# %s-SEND: CREATE PIPE\n", pipeFileName);

			iRes = Grib_BlePipeReCreate(pipeFilePath);
			if(iRes == GRIB_FAIL)
			{
				GRIB_LOGD("# %s-SEND: CREATE PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			if(iDBG)GRIB_LOGD("# %s-SEND: OPEN PIPE\n", pipeFileName);
			// Open PIPE File(Use Only Read Buffer)
			pipeFileFD = open(pipeFilePath, O_RDONLY);
			if(pipeFileFD < 0)
			{
				GRIB_LOGD("# %s-SEND: OPEN PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			iTotal = iCount = GRIB_INIT;
			if(iDBG)GRIB_LOGD("# %s-SEND: WAITING FOR CHILD MSG ...\n", pipeFileName);
//			SLEEP(1);
			do
			{
				iCount = read(pipeFileFD, recvBuff+iTotal, BLE_MAX_SIZE_RECV_MSG-iTotal);
				if(iCount < 0)
				{
					GRIB_LOGD("# %s-SEND: READ FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
					break;
				}
				else if(iCount == 0)
				{
					if(iDBG)GRIB_LOGD("# %s-SEND: READ DONE\n", pipeFileName);
					break;
				}
				else
				{
					iTotal += iCount;
					if(iDBG)GRIB_LOGD("# %s-SEND: READ:%d TOTAL:%d\n", pipeFileName, iCount, iTotal);
				}

			}while(iTotal < BLE_MAX_SIZE_RECV_MSG);
			//iRes = kill(processID, SIGKILL);
			if(iDBG)GRIB_LOGD("# %s-SEND: WAITING FOR RETURN CHILD[PID:%d]\n", pipeFileName, processID);

			do{
				processID = wait(&iStatus);
				if(processID != GRIB_FAIL)
				{//shbaek: OK
					break;
				}

				if(WAIT_TRY_MAX < iWaitTry)
				{//shbaek: Occur Critical Error, Will Defunct
					GRIB_LOGD("# %s-SEND: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					GRIB_LOGD("# %s-SEND: #####       %c[1;31mCRITICAL ERROR%c[0m          #####\n", pipeFileName, 27, 27);
					GRIB_LOGD("# %s-SEND: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					break;
				}

				iWaitTry++;
				GRIB_LOGD("# %s-SEND: WAIT FAIL RE-TRY: %d\n", pipeFileName, iWaitTry);
				SLEEP(WAIT_TRY_DELAY);
			}while(TRUE);

			if(iDBG)GRIB_LOGD("# %s-SEND: CHILD IS DONE PID:%d STATUS:%d\n", pipeFileName, processID, iStatus);

			if(0 < pipeFileFD)
			{
				close(pipeFileFD);
				pipeFileFD = GRIB_ERROR;
			}

			break;
		}
	}

	GRIB_LOGD("# %s-BLE<: %s\n", pipeFileName, recvBuff);

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

		Grib_BleTombStone(&bleLogInfo);

		pBleMsg->eCode = iError;

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



#define __GRIB_BLE_DEVICE_API__

int Grib_BleGetDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo)
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
	if( (STRLEN(pRowDeviceInfo->deviceID) < 3) || (STRSTR(pRowDeviceInfo->deviceID, GRIB_STR_ERROR)) )
	{
		GRIB_LOGD("# BLE-INFO: DEVICE ID ERROR: %s\n", pRowDeviceInfo->deviceID);
		return GRIB_ERROR;
	}

	deviceID = pRowDeviceInfo->deviceID;
	pRowDeviceInfo->deviceInterface = DEVICE_IF_TYPE_BLE;

	STRINIT(pRowDeviceInfo->deviceLoc, sizeof(pRowDeviceInfo->deviceLoc));
	STRNCPY(pRowDeviceInfo->deviceLoc, GRIB_STR_NOT_USED, STRLEN(GRIB_STR_NOT_USED));

	STRINIT(pRowDeviceInfo->deviceDesc, sizeof(pRowDeviceInfo->deviceDesc));
	STRNCPY(pRowDeviceInfo->deviceDesc, GRIB_STR_NOT_USED, STRLEN(GRIB_STR_NOT_USED));

#if __NOT_USED__ //shbaek: Loc. Desc. -> TBD
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
#endif

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetReportCycle(deviceAddr, deviceID, recvBuff);
	if( (iRes!=GRIB_DONE) || (STRSTR(pRowDeviceInfo->deviceID, GRIB_STR_ERROR)) )
	{
		GRIB_LOGD("# BLE-INFO: GET REPORT CYCLE FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}
	pRowDeviceInfo->reportCycle = ATOI(recvBuff);

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetFuncCount(deviceAddr, deviceID, recvBuff);
	if( (iRes!=GRIB_DONE) || (STRSTR(pRowDeviceInfo->deviceID, GRIB_STR_ERROR)) )
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
	if( (iRes!=GRIB_DONE) || (STRSTR(pRowDeviceInfo->deviceID, GRIB_STR_ERROR)) )
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
	if( (iRes!=GRIB_DONE) || (STRSTR(pRowDeviceInfo->deviceID, GRIB_STR_ERROR)) )
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


#define __GRIB_BLE_ETC__

int Grib_BleGetCharHandler(Grib_BleMsgInfo* pBleMsg)
{
	const char* FUNC = "BLE-CHAR";
	int iDBG = TRUE;//gDebugBle;
	int iRes = GRIB_ERROR;
	int iCount = 0;

	char  optDevice[GRIB_MAX_SIZE_BRIEF] = {'\0', };
	char  optResponse[GRIB_MAX_SIZE_MIDDLE] = {'\0', };
	char  optFindHandle[GRIB_MAX_SIZE_BRIEF] = {'\0', };

	char  pipeBuff[SIZE_1K] = {0x00,};
	int   pipeFileFD = -1;
	char  pipeFilePath[SIZE_1K] = {'\0', };

	if( (pBleMsg==NULL) || (STRLEN(pBleMsg->addr)==0) )
	{
		GRIB_LOGD("# %s: INVALID PARAM !!!\n", FUNC);
		return GRIB_FAIL;
	}

	//shbaek: Check Info
	if(iDBG)
	{
		GRIB_LOGD("# %s: ADDR: %s\n", FUNC, pBleMsg->addr);
		GRIB_LOGD("# %s: PEER: %d\n", FUNC, pBleMsg->peerType);
		GRIB_LOGD("# %s: PIPE: %s\n", FUNC, pBleMsg->pipe);
	}

	SNPRINTF(optDevice, sizeof(optDevice), "%s=%s",   BLE_GATT_OPT_DEVICE, pBleMsg->addr);

	//shbaek: Check Pipe
	if(pBleMsg->pipe == NULL)
	{
		pBleMsg->pipe = BLE_FILE_NAME_PIPE_TEMP;
	}
	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pBleMsg->pipe);
	SNPRINTF(optResponse, sizeof(optResponse), "%s=%s", BLE_GATT_OPT_RESPONSE, pipeFilePath);

	//shbaek: Check Find Char
	if(STRLEN(pBleMsg->findHandle) != BLE_MAX_SIZE_HANDLE)
	{
		STRINIT(pBleMsg->findHandle, sizeof(pBleMsg->findHandle));
		STRNCPY(pBleMsg->findHandle, BLE_DEFAULT_FIND_CHAR, STRLEN(BLE_DEFAULT_FIND_CHAR));
	}
	STRINIT(optFindHandle, sizeof(optFindHandle));
	SNPRINTF(optFindHandle, sizeof(optFindHandle), "%s=%s", BLE_GATT_OPT_FIND_CHAR, pBleMsg->findHandle);







	//2shbaek: Jump to Gatttool
	iRes = execl(BLE_FILE_PATH_GATTTOOL_EX, BLE_FILE_PATH_GATTTOOL_EX, 
		optDevice, BLE_GATT_OPT_PEER_STR(pBleMsg->peerType), optFindHandle, optResponse, BLE_GATT_OPT_DEBUG, NULL);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: GATTTOOL ERROR !!!\n", FUNC);
		return iRes;
	}
	if(iDBG)GRIB_LOGD("# %s: EXECL DONE\n", FUNC);

	iRes = Grib_BlePipeReCreate(pipeFilePath);
	if(iRes == GRIB_FAIL)
	{
		GRIB_LOGD("# %s: CREATE PIPE FAIL: %s[%d]\n", FUNC, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# %s: CREATE PIPE DONE\n", FUNC);
	// Open PIPE File(Use Only Read Buffer)
	pipeFileFD = open(pipeFilePath, O_RDONLY);
	if(pipeFileFD < 0)
	{
		GRIB_LOGD("# %s: OPEN PIPE FAIL: %s[%d]\n", FUNC, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("# %s: OPEN PIPE DONE\n", FUNC);

	iCount = read(pipeFileFD, pipeBuff, sizeof(pipeBuff));
	if(iDBG)GRIB_LOGD("# %s: READ COUNT: %d\n", FUNC, iCount);

	return iRes;
}

int Grib_BleSendReq(Grib_BleMsgInfo* pBleMsg)
{
	const char* FUNC = "BLE-SENDr";
	int iDBG = TRUE;//gDebugBle;
	int iRes = GRIB_ERROR;
	int iCount = 0;

	char optDevice[GRIB_MAX_SIZE_BRIEF] = {'\0', };
	char optHandle[GRIB_MAX_SIZE_BRIEF] = {'\0', };
	char optValue[GRIB_MAX_SIZE_BRIEF] = {'\0', };

	char  pipeBuff[SIZE_1K] = {0x00,};
	int   pipeFileFD = -1;
	char* pipeFileName = BLE_FILE_PATH_PIPE_ROOT "/" BLE_FILE_NAME_PIPE_TEMP;
	char* pipeFilePath = pipeFileName;
	if(pBleMsg == NULL)
	{
		GRIB_LOGD("# %s: INVALID PARAM !!!\n", FUNC);
		return GRIB_FAIL;
	}

	if(iDBG)GRIB_LOGD("# %s: [ADDR: %s] [HANDLE: %s] [VALUE: %s]\n", FUNC,
		pBleMsg->addr, pBleMsg->handle, pBleMsg->sendMsg);

	SNPRINTF(optDevice, sizeof(optDevice), "%s=%s",   BLE_GATT_OPT_DEVICE, pBleMsg->addr);
	SNPRINTF(optHandle, sizeof(optHandle), "%s=0x%s", BLE_GATT_OPT_HANDLE, pBleMsg->handle);
	SNPRINTF(optValue,  sizeof(optValue),  "%s=%s",   BLE_GATT_OPT_VALUE,  pBleMsg->sendMsg);

	iRes = execl(BLE_FILE_PATH_GATTTOOL_EX, BLE_FILE_PATH_GATTTOOL_EX, optDevice, BLE_GATT_OPT_WRITE_REQ, optHandle, optValue, NULL);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# %s: GATTTOOL ERROR !!!\n", FUNC);
		return iRes;
	}
	if(iDBG)GRIB_LOGD("# %s: EXECL DONE\n", FUNC);

	iRes = Grib_BlePipeReCreate(pipeFileName);
	if(iRes == GRIB_FAIL)
	{
		GRIB_LOGD("# %s: CREATE PIPE FAIL: %s[%d]\n", FUNC, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}

	if(iDBG)GRIB_LOGD("# %s: CREATE PIPE DONE\n", FUNC);
	// Open PIPE File(Use Only Read Buffer)
	pipeFileFD = open(pipeFilePath, O_RDONLY);
	if(pipeFileFD < 0)
	{
		GRIB_LOGD("# %s: OPEN PIPE FAIL: %s[%d]\n", FUNC, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("# %s: OPEN PIPE DONE\n", FUNC);

	iCount = read(pipeFileFD, pipeBuff, sizeof(pipeBuff));
	if(iDBG)GRIB_LOGD("# %s: READ COUNT: %d\n", FUNC, iCount);

	return iRes;
}


int Grib_BleSendMsgTest(char* deviceAddr, char *pipeFileName, char* sendBuff, char* recvBuff)
{
	int iRes = GRIB_DONE;
	int iDBG = gDebugBle;
	int iCount = GRIB_INIT;
	int iTotal = GRIB_INIT;
	int iStatus = GRIB_ERROR;
	int pipeFileFD = GRIB_ERROR;

	char pipeFilePath[SIZE_1K] = {'\0', };

	pid_t processID = GRIB_ERROR;

	const char* FUNC = "BLE-SEND";

	if(deviceAddr==NULL || sendBuff==NULL || recvBuff==NULL)
	{
		GRIB_LOGD("# %s: PARAM IS NULL\n", FUNC);
		return GRIB_ERROR;
	}

	if(pipeFileName == NULL)
	{//shbaek: for TEST
		pipeFileName = "TEMP_PIPE_FILE";
	}

	if(iDBG)
	{
		GRIB_LOGD("# %s-BLE>: DEVICE ADDR   : %s\n", pipeFileName, deviceAddr);
		GRIB_LOGD("# %s-BLE>: PIPE FILE     : %s\n", pipeFileName, pipeFileName);
	}
	GRIB_LOGD("# %s-BLE>: %s\n", pipeFileName, sendBuff);

	processID = fork();
	if(processID == GRIB_ERROR)
	{
		GRIB_LOGD("# %s-BLE>: PROCESS FORK FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("# %s-BLE>: FORK PROCESS ID: %d\n", pipeFileName, processID);

	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pipeFileName);
	if(iDBG)GRIB_LOGD("# %s-BLE>: PIPE FILE PATH: %s\n", pipeFileName, pipeFilePath);

	switch(processID)
	{
		case 0:
		{
			//3 shbaek: Jump to Gatttool
			if(iDBG)GRIB_LOGD("# %s-SEND: JUMP BLE EXTEND\n", pipeFileName);
			if(iDBG)Grib_ShowCurrDateTime();

			iRes = execl(BLE_FILE_PATH_GATTTOOL, BLE_FILE_PATH_GATTTOOL, deviceAddr, pipeFilePath, sendBuff, NULL);

			if(iDBG)GRIB_LOGD("# %s-SEND: DO YOU SEE ME???\n");
			exit(iRes);
			break;
		}

		default:
		{
			int iWaitTry = 0;
			const int WAIT_TRY_MAX = 5;
			const int WAIT_TRY_DELAY = 3;

			const int READ_INTERVAL = 100;
			int READ_TOTAL_TIME = (BLE_GATTTOOL_TIMEOUT+2) * 1000; //shbaek: 2 -> Timming ...

			if(iDBG)GRIB_LOGD("# %s-SEND: CREATE PIPE\n", pipeFileName);

			//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
			iRes = Grib_BlePipeReCreate(pipeFilePath);
			if(iRes == GRIB_FAIL)
			{
				GRIB_LOGD("# %s-SEND: CREATE PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			if(iDBG)GRIB_LOGD("# %s-SEND: OPEN PIPE\n", pipeFileName);
			// Open PIPE File(Use Only Read Buffer)
			pipeFileFD = open(pipeFilePath, O_RDONLY | O_NOCTTY | O_NONBLOCK);
			if(pipeFileFD < 0)
			{
				GRIB_LOGD("# %s-SEND: OPEN PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}
			//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

			//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
			if(iDBG)GRIB_LOGD("# %s-SEND: WAITING FOR RETURN CHILD[PID:%d]\n", pipeFileName, processID);
			do{
				processID = wait(&iStatus);
				if(processID != GRIB_FAIL)
				{//shbaek: OK
					break;
				}

				if(WAIT_TRY_MAX < iWaitTry)
				{//shbaek: Occur Critical Error, Will Defunct
					GRIB_LOGD("# %s-SEND: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					GRIB_LOGD("# %s-SEND: #####       %c[1;31mCRITICAL ERROR%c[0m          #####\n", pipeFileName, 27, 27);
					GRIB_LOGD("# %s-SEND: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					break;
				}

				iWaitTry++;
				GRIB_LOGD("# %s-SEND: WAIT FAIL RE-TRY: %d\n", pipeFileName, iWaitTry);
				SLEEP(WAIT_TRY_DELAY);
			}while(TRUE);
			if(iDBG)Grib_ShowCurrDateTime();
			//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

			//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
			iTotal = iCount = GRIB_INIT;
			MEMSET(recvBuff, 0x00, BLE_MAX_SIZE_RECV_MSG);
			do
			{
				if(iDBG)GRIB_LOGD("# %s-SEND: READ CHILD MSG ...\n", pipeFileName);

				iCount = read(pipeFileFD, recvBuff+iTotal, BLE_MAX_SIZE_RECV_MSG-iTotal);
//				GRIB_LOGD("# %s-SEND: READ COUNT: %d\n", pipeFileName, iCount);

				if(iCount < 0)
				{
					GRIB_LOGD("# %s-SEND: READ FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
					break;
				}
				else if(iCount == 0)
				{
					if(iTotal!=0)
					{//shbaek: Read Something ...
						if(iDBG)GRIB_LOGD("# %s-SEND: READ DONE\n", pipeFileName);
						break;
					}
				}
				else
				{
					iTotal += iCount;
					if(iDBG)GRIB_LOGD("# %s-SEND: READ:%d TOTAL:%d\n", pipeFileName, iCount, iTotal);
				}

//				GRIB_LOGD("# %s-SEND: READ DELAY: %d\n", pipeFileName, READ_TOTAL_TIME);

				if(READ_INTERVAL < READ_TOTAL_TIME)mSleep(READ_INTERVAL);
				else
				{//shbaek: Read Timeout !!!
					if(iTotal == 0)
					{//shbaek: Something Wrong ...
						SNPRINTF(recvBuff, BLE_MAX_SIZE_RECV_MSG, "%s:%d", GRIB_STR_ERROR, BLE_ERROR_CODE_READ_TIMEOUT);
					}
					break;
				}
				READ_TOTAL_TIME -= READ_INTERVAL;

			}while(iTotal < BLE_MAX_SIZE_RECV_MSG);
			//iRes = kill(processID, SIGKILL);
//			Grib_ShowCurrDateTime();
			//shbaek: ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

			if(iDBG)GRIB_LOGD("# %s-SEND: CHILD IS DONE PID:%d STATUS:%d\n", pipeFileName, processID, iStatus);

			if(0 < pipeFileFD)
			{
				close(pipeFileFD);
				pipeFileFD = GRIB_ERROR;
			}

			break;
		}
	}

	GRIB_LOGD("# %s-BLE<: %s\n", pipeFileName, recvBuff);

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

		Grib_BleTombStone(&bleLogInfo);

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
