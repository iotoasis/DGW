#ifndef __GRIB_THREAD_H__
#define __GRIB_THREAD_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include <time.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>

#include "grib_define.h"
#include "grib_onem2m.h"

#include "grib_db.h"
#include "grib_ble.h"

#include "grib_regi.h"

#ifdef FEATURE_CAS
#include "grib_cas.h"
#endif
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define WATCHDOG_STATUS_NONE						0
#define WATCHDOG_STATUS_DONE						1

typedef struct
{
	pthread_t 		controlThreadID;
	pthread_t 		reportThreadID;
	pthread_attr_t	threadAttr;
	pthread_cond_t	threadCond;
	pthread_mutex_t	threadMutex;

	int controlThreadStatus;
	int reportThreadStatus;
	int bleContinualError;
	int watchdogStatus;

#ifdef FEATURE_CAS
	char authKey[GRIB_MAX_SIZE_AUTH_KEY];
#endif

	Grib_DbRowDeviceInfo*  pRowDeviceInfo;

}Grib_DeviceThreadInfo;

typedef struct
{
	pthread_t 		threadID;
	pthread_attr_t	threadAttr;
	pthread_cond_t	threadCond;
	pthread_mutex_t	threadMutex;

}Grib_HubThreadInfo;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int   Grib_SetThreadConfig(void);

//3 shbaek: BLE Thread
int   Grib_ThreadStart(void);
void* Grib_ReportThread(void* threadArg);
void *Grib_ControlThread(void* threadArg);



#endif
