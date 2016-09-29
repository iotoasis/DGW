/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_ble_extend.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

static BleCommArg* gBleCommArg;
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int str2hex(char* srcBuff, char* hexBuff)
{
	int i;
	for(i=0; i<(int)strlen(srcBuff); i++)
	{
		sprintf(hexBuff+i*2, "%02X", *(srcBuff+i));
	}

	return i;
}

void Grib_BleCommSetArg(BleCommArg* pBleCommArg)
{
	gBleCommArg = pBleCommArg;
	return ;
}

int Grib_CheckCriticalMsg(GError* pErrorMsg)
{
	char* isBusyError = NULL;

	if( (pErrorMsg==NULL) || (pErrorMsg->message==NULL) )
	{
		return FALSE;
	}

	isBusyError = strstr(pErrorMsg->message, GRIB_ERROR_STR_BUSY);

	if(isBusyError != NULL)
	{
		return TRUE;
	}
	
	return FALSE;
}

int	Grib_BleCommWritePipe(char* pipeFile, char* pipeMsg)
{
	int iFD;

	if(gBleCommArg != NULL)
	{
		if(gBleCommArg->pipeWrite == TRUE)
		{//shbaek: Need Not Write
			printf("# BLE-EX PIPE WRITE: [PIPE: %s] ALREADY PIPE WRITED\n", pipeFile);
			return 0;
		}
	}

	iFD = open(pipeFile, O_WRONLY);
	if(iFD < 0)
	{
		printf("# BLE-EX PIPE WRITE: [PIPE: %s] PIPE OPEN FAIL !!!\n", pipeFile);
		return -1;
	}

	write(iFD, pipeMsg, strlen(pipeMsg));
	close(iFD);

	if(gBleCommArg != NULL)
	{
		gBleCommArg->pipeWrite = TRUE;
	}

	return 0;
}

int Grib_BleCommLoopQuit(GMainLoop* pEventLoop, GError* pErrorMsg, Grib_BleErrorCode iErrorCode)
{
	int iRes = -1;
	int iReTry = 0;
	const int MAX_PIPE_WRITE_RETRY_COUNT = 10;

	if(gBleCommArg == NULL)
	{
		printf("# BLE-EX LOOP QUIT: ARG IS NULL\n");
		return -1;
	}

	//3 shbaek: Check Criticla Message
	if(Grib_CheckCriticalMsg(pErrorMsg) == TRUE)
	{
		iErrorCode = BLE_ERROR_CODE_CRITICAL;
		g_printerr("# BLE-EX LOOP QUIT: [PIPE: %s] NEED DRIVER RESET !!!\n", gBleCommArg->pPipePath, pErrorMsg->message);
	}
	else
	{
		if(gBleCommArg->isDebug)g_printerr("# BLE-EX LOOP QUIT: [PIPE: %s] [LIB ERROR: %s]\n", gBleCommArg->pPipePath, pErrorMsg->message);
	}	

	//shbaek: Create Error Message
	if( (iErrorCode!=BLE_ERROR_CODE_NONE) || (strlen(gBleCommArg->pRecvBuff)==0) )
	{
		memset(gBleCommArg->pRecvBuff, 0x00, sizeof(gBleCommArg->pRecvBuff));
		snprintf(gBleCommArg->pRecvBuff, sizeof(gBleCommArg->pRecvBuff), "ERROR:%d", iErrorCode);
	}

	do
	{//3 shbaek: Give to Write Chance [MAX_PIPE_WRITE_RETRY_COUNT]
		iRes = Grib_BleCommWritePipe(gBleCommArg->pPipePath, gBleCommArg->pRecvBuff);
		if(iRes == 0)
		{
			break;
		}

		//shbaek: Must be Write Pipe File.
		iReTry++;
		g_printerr("# BLE-EX LOOP QUIT: [PIPE: %s] [RETRY WRITE COUNT: %d]\n", gBleCommArg->pPipePath, iReTry);
		sleep(1);
	}while(iReTry < MAX_PIPE_WRITE_RETRY_COUNT);

	if(pEventLoop != NULL)
	{
		g_main_loop_quit(pEventLoop);
		pEventLoop = NULL;
	}
	
	return 0;
}

