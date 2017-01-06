#ifndef __GRIB_SDA_H__
#define __GRIB_SDA_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include "grib_define.h"
#include "grib_http.h"
#include "grib_util.h"


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define SDA_MAX_DEVICE_INFO 										(HTTP_MAX_SIZE)

#define SDA_GET_DEVICE_INFO_FORMAT_RAW							\
		"GET http://%s:%d/sda/deviceinfo/deviceinfo?p=%s\r\n"

#define SDA_GET_DEVICE_INFO_FORMAT_HTTP							\
		"GET /sda/deviceinfo/deviceinfo?p=%s HTTP/1.1\r\n"			\
		"Host: %s:%d\r\n"											\
		"Content-Type:" HTTP_CONTENT_TYPE_JSON "\r\n"				\
		"Accept:" HTTP_CONTENT_TYPE_JSON "\r\n"						\
		"\r\n"

/*
		"http://166.104.112.43:20080/sda/deviceinfo/deviceinfo?p=ONSB_BleScanner01_001"
*/

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
void Grib_SdaSetDebug(int iDebug);
int  Grib_SdaSetServerConfig(void);

int  Grib_SdaGetDeviceInfo(char* deviceID, char* deviceInfo);

#endif
