#ifndef __GRIB_LOG_H__
#define __GRIB_LOG_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>

#include "grib_define.h"
#include "grib_util.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
void Grib_PrintHex(const char* LABEL, char* pHexBuff, int iSize);
void Grib_PrintOnlyHex(char* pHexBuff, int iSize);

inline void Grib_DebugLog(const char* LABEL, const char* LOG)
{
	GRIB_LOGD("# %s: %c[1;34m%s%c[0m\n", LABEL, 27, LOG, 27);
}

inline void Grib_InfoLog(const char* LABEL, const char* LOG)
{
	GRIB_LOGD("# %s: %c[1;32m%s%c[0m\n", LABEL, 27, LOG, 27);
}

inline void Grib_WarrLog(const char* LABEL, const char* LOG)
{
	GRIB_LOGD("# %s: %c[1;33m%s%c[0m\n", LABEL, 27, LOG, 27);
}

inline void Grib_ErrLog(const char* LABEL, const char* LOG)
{
	GRIB_LOGD("# %s: %c[1;31m%s%c[0m\n", LABEL, 27, LOG, 27);
}

#endif
