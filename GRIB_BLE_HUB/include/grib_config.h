#ifndef __GRIB_CONFIG_H__
#define __GRIB_CONFIG_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <ifaddrs.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/resource.h>

#include "grib_define.h"
#include "grib_db.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */


//2 shbaek: DEFAULT CONFIG DATA ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

#define GRIB_DEFAULT_CONFIG_FILE_PATH		"./data/grib_db_default_config.dat"

#define GRIB_CONFIG_HUB_ID					"HUB_ID"

#define GRIB_CONFIG_SI_SERVER_IP				"SI_SERVER_IP"
#define GRIB_CONFIG_SI_SERVER_PORT			"SI_SERVER_PORT"
#define GRIB_CONFIG_SI_IN_NAME				"SI_IN_NAME"
#define GRIB_CONFIG_SI_CSE_NAME				"SI_CSE_NAME"

#define GRIB_CONFIG_AUTH_SERVER_IP			"AUTH_SERVER_IP"
#define GRIB_CONFIG_AUTH_SERVER_PORT			"AUTH_SERVER_PORT"

#define GRIB_CONFIG_SMD_SERVER_IP				"SMD_SERVER_IP"
#define GRIB_CONFIG_SMD_SERVER_PORT			"SMD_SERVER_PORT"

#define GRIB_CONFIG_RESET_TIME_HOUR			"RESET_TIME_HOUR"
#define GRIB_CONFIG_DEBUG_LEVEL				"GRIB_DEBUG_LEVEL"
#define GRIB_CONFIG_TOMBSTONE					"GRIB_TOMBSTONE"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Type Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

typedef struct
{
	int  isLoad;
	char hubID[DEVICE_MAX_SIZE_ID];

	char siServerIP[GRIB_MAX_SIZE_IP_STR];
	unsigned int siServerPort;

	char siInName[GRIB_MAX_SIZE_MIDDLE];
	char siCseName[GRIB_MAX_SIZE_MIDDLE];

	char authServerIP[GRIB_MAX_SIZE_IP_STR];
	unsigned int authServerPort;

	char smdServerIP[GRIB_MAX_SIZE_IP_STR];
	unsigned int smdServerPort;

	int resetTimeHour;
	int debugLevel;
	int tombStone;

}Grib_ConfigInfo;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
Grib_ConfigInfo* Grib_GetConfigInfo(void);
void Grib_ShowConfig(Grib_ConfigInfo* pConfigInfo);
int	 Grib_LoadConfig(Grib_ConfigInfo* pConfigInfo);
int Grib_GetConfigDB(void);

int Grib_SetConfigHub(Grib_ConfigInfo* pConfigInfo);
int Grib_SetConfigSi(Grib_ConfigInfo* pConfigInfo);
int Grib_SetConfigSmd(Grib_ConfigInfo* pConfigInfo);
int Grib_SetConfigEtc(Grib_ConfigInfo* pConfigInfo);

#endif
