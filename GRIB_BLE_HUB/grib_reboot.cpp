/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Include File
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <linux/limits.h>

#include "grib_define.h"
#include "grib_util.h"
#include "grib_config.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
char MAIN_PATH[PATH_MAX];

void* Grib_RebootTimeThread(void* threadArg)
{
	int iDBG = FALSE;//gDebugThread;
	int intervalTime = 24;
	
	time_t sysTimer;
	struct tm *sysTime;

	Grib_ConfigInfo* pConfigInfo = NULL;

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# RESET-THREAD: GET CONFIG ERROR !!!\n");
		return NULL;
	}

	if(pConfigInfo->resetTimeHour == 0)
	{
		GRIB_LOGD("# RESET-THREAD: SWITCH OFF !!!\n");
		return NULL;
	}

	if( (0<pConfigInfo->resetTimeHour) && (pConfigInfo->resetTimeHour<=24) )
	{
		intervalTime = pConfigInfo->resetTimeHour;		
	}
	GRIB_LOGD("# RESET-THREAD: INTERVAL TIME: %d\n", intervalTime);

	while(TRUE)
	{
		sysTimer = time(NULL);
		sysTime  = localtime(&sysTimer);
		if(iDBG)GRIB_LOGD("# RESET-THREAD: %02d:%02d:%02d\n", sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec);
		SLEEP(10);

		if( ((sysTime->tm_hour%intervalTime)==0) && (sysTime->tm_min==0) )
		{
			char rebootMsg[SIZE_1K] = {'\0', };

			SNPRINTF(rebootMsg, sizeof(rebootMsg), "Time is Up ...\n"
				"Reset Time Hour: %d\n", pConfigInfo->resetTimeHour);
			systemReboot(30, rebootMsg);
		}
	}

	return threadArg;
}

int Grib_MenuRebootConfig(void)
{
	const char* FUNC = "REBOOT-CONFIG";
	const int   MAX_REBOOT_TIME = 24;
	const int   REBOOT_WAIT_SEC = 30;

	int iDBG = FALSE;
	int intervalTime = MAX_REBOOT_TIME;
	
	time_t sysTimer;
	struct tm *sysTime;

	char rebootMsg[SIZE_1K] = {'\0', };

	Grib_ConfigInfo* pConfigInfo = NULL;
	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("# %s: GET CONFIG ERROR !!!\n", FUNC);
		return GRIB_ERROR;
	}

	if(pConfigInfo->resetTimeHour == 0)
	{
		GRIB_LOGD("# %s: %c[1;33mSWITCH OFF !!!%c[0m\n", FUNC, 27, 27);
		return GRIB_ERROR;
	}

	if( (0<pConfigInfo->resetTimeHour) && (pConfigInfo->resetTimeHour<=MAX_REBOOT_TIME) )
	{
		intervalTime = pConfigInfo->resetTimeHour;
	}
	GRIB_LOGD("# %s: %c[1;33mREBOOT CONFIG HOUR: %d%c[0m\n", FUNC, 27, intervalTime, 27);

	while(TRUE)
	{
		sysTimer = time(NULL);
		sysTime  = localtime(&sysTimer);

		if(sysTime->tm_sec == 0)
		{//shbaek: Show Yourself ...
			GRIB_LOGD("\n");
			GRIB_LOGD(GRIB_1LINE_SHARP);
			GRIB_LOGD("# REBOOT-PROCESS: %c[1;33m%02d:%02d:%02d [CONFIG HOUR: %d]%c[0m\n", 27, sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec, intervalTime, 27);
			GRIB_LOGD(GRIB_1LINE_SHARP);
			GRIB_LOGD("\n");
		}

		if( (sysTime->tm_min==0) && (sysTime->tm_hour%intervalTime==0) )
		{//shbaek: It's Time Up !!!
			break;
		}

		SLEEP(1);
	}

#if 1 //shbaek: Do You Wnat Kill Hub?
	{
		const char* CMD_LIST[] = { "sudo kill -9 `pidof " GRIB_PROGRAM_BLE_HUB "`",
									"sudo kill -9 `pidof " GRIB_PROGRAM_GATTTOOL "`"
								  };
		int i = 0;
		char* pCmd = NULL;
		char pBuff[SIZE_1K] = {'\0', };

		for(i=0; i<sizeof(CMD_LIST)/sizeof(pCmd); i++)
		{
			pCmd = (char*) CMD_LIST[i];
			GRIB_LOGD("# %s: %c[1;31m%s%c[0m\n", FUNC, 27, pCmd, 27);
			
			systemCommand(pCmd, pBuff, sizeof(pBuff));
			if(pBuff)
			{
				GRIB_LOGD("# %s: %c[1;31m%s%c[0m\n", FUNC, 27, pBuff, 27);
				GRIB_LOGD("\n");
			}
			SLEEP(1);
		}
	}
#endif

	SNPRINTF(rebootMsg, sizeof(rebootMsg), "Re-Boot by Config ...\n" "Re-Boot Time Hour: %d\n", pConfigInfo->resetTimeHour);
	systemReboot(REBOOT_WAIT_SEC, rebootMsg);

	return GRIB_DONE;
}

int Grib_MenuRebootTimer(int waitTime, char* reason)
{
	const char* FUNC = "REBOOT-TIMER";

	time_t sysTimer;
	struct tm *sysTime;

	char rebootMsg[SIZE_1K] = {'\0', };

	GRIB_LOGD("# %s: %c[1;33mREBOOT TIMER SEC: %d%c[0m\n", FUNC, 27, waitTime, 27);

#if 1 //shbaek: Do You Wnat Kill Hub?
	{
		const char* CMD_LIST[] = { "sudo kill -9 `pidof " GRIB_PROGRAM_BLE_HUB "`",
									"sudo kill -9 `pidof " GRIB_PROGRAM_GATTTOOL "`"
								  };
		int i = 0;
		char* pCmd = NULL;
		char pBuff[SIZE_1K] = {'\0', };

		for(i=0; i<sizeof(CMD_LIST)/sizeof(pCmd); i++)
		{
			pCmd = (char*) CMD_LIST[i];
			GRIB_LOGD("# %s: %c[1;31m%s%c[0m\n", FUNC, 27, pCmd, 27);
			
			systemCommand(pCmd, pBuff, sizeof(pBuff));
			if(pBuff)
			{
				GRIB_LOGD("# %s: %c[1;31m%s%c[0m\n", FUNC, 27, pBuff, 27);
				GRIB_LOGD("\n");
			}
			SLEEP(1);			
		}

	}
#endif

	SNPRINTF(rebootMsg, sizeof(rebootMsg), "%s\nRe-Boot Wait Time: %d\n", reason, waitTime);

	systemReboot(waitTime, rebootMsg);

	return GRIB_DONE;
}

int main(int argc, char **argv)
{
	const char* FUNC = "REBOOT-MAIN";

	char* mainMenu = NULL;
	char* subMenu = NULL;

//	realpath(argv[GRIB_CMD_NAME], MAIN_PATH);
	getcwd(MAIN_PATH, sizeof(MAIN_PATH));
	GRIB_LOGD("\n");
	GRIB_LOGD("# %s: %s\n", FUNC, MAIN_PATH);

	if(STRLEN(argv[GRIB_CMD_MAIN])<=0)
	{
		GRIB_LOGD("# %s: IN-VALID MAIN COMMAND !!!\n", FUNC);
		return GRIB_ERROR;
	}

	mainMenu = argv[GRIB_CMD_MAIN];

	if(STRCASECMP(mainMenu, "CONFIG") == 0)
	{
		Grib_MenuRebootConfig();
		return GRIB_DONE;
	}

	if(STRCASECMP(mainMenu, "TIMER") == 0)
	{
		int waitTime = 1;
		char* rebootMsg = "Don't Ask Me ...";

		if(0<STRLEN(argv[GRIB_CMD_SUB]))
		{
			waitTime = ATOI(argv[GRIB_CMD_SUB]);
		}

		if(0<STRLEN(argv[GRIB_CMD_ARG1]))
		{
			rebootMsg = argv[GRIB_CMD_ARG1];
		}

		Grib_MenuRebootTimer(waitTime, rebootMsg);
		return GRIB_DONE;
	}

	GRIB_LOGD("# %s: IN-VALID MAIN COMMAND !!!\n", FUNC);

	return GRIB_ERROR;
}
