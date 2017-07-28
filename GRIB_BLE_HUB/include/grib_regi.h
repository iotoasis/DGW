#ifndef __GRIB_REGI_H__
#define __GRIB_REGI_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "grib_define.h"
#include "grib_util.h"
#include "grib_config.h"

#include "grib_db.h"
#include "grib_ble.h"
#include "grib_onem2m.h"

#ifdef FEATURE_CAS
#include "grib_cas.h"
#endif

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_DeviceRegi(char* deviceAddr, int optAuth);
int Grib_DeviceDeRegi(char* deviceID, int delOneM2M);
int Grib_HubRegi(char* pAuthKey);

#endif
