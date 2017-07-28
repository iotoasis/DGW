#ifndef __GRIB_AUTH_H__
#define __GRIB_AUTH_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Include File
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_define.h"
#include "grib_config.h"
#include "grib_util.h"
#include "grib_http.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define AUTH_REGI_OPT_PW_OVER_WRITE								1
#define AUTH_REGI_OPT_PW_RE_USED									2


#define AUTH_HTTP_ATTR_FORMAT										"Authorization: Basic deviceId:%s"

#define AUTH_DEFAULT_DEVICE_PW									"GRIB_DEFAULT_PW"

#define AUTH_HEAD_FORMAT_GATEWAY_REGI							\
		"POST /si/dev_t2_reg HTTP/1.1\r\n"							\
		"Host: %s:%d\r\n"											\
		"Accept:application/json\r\n"								\
		"Content-Type: application/json\r\n"						\
		"Content-Length: %d\r\n"									\
		"\r\n"

#define AUTH_BODY_FORMAT_GATEWAY_REGI							\
		"{\r\n"														\
		"    \"_devId\": \"%s\"\r\n"								\
		"}\r\n"														\
		"\r\n"

#define AUTH_HEAD_FORMAT_DEVICE_REGI								\
		"POST /si/dev_t1_reg HTTP/1.1\r\n"							\
		"Host: %s:%d\r\n"											\
		"Accept:application/json\r\n"								\
		"Content-Type: application/json\r\n"						\
		"Content-Length: %d\r\n"									\
		"\r\n"

#define AUTH_BODY_FORMAT_DEVICE_REGI								\
		"{\r\n"														\
		"    \"_devId\": \"%s\",\r\n"								\
		"    \"_pwd\": \"%s\"\r\n"									\
		"}\r\n"														\
		"\r\n"

#define AUTH_HEAD_FORMAT_DEVICE_INFO								\
		"POST /si/dev_inf HTTP/1.1\r\n"								\
		"Host: %s:%d\r\n"											\
		"Accept:application/json\r\n"								\
		"Content-Type: application/json\r\n"						\
		"Content-Length: %d\r\n"									\
		"\r\n"

#define AUTH_BODY_FORMAT_DEVICE_INFO								\
		"{\r\n"														\
		"    \"_devId\": \"%s\"\r\n"								\
		"}\r\n"														\
		"\r\n"

#define AUTH_HEAD_FORMAT_DEVICE_DEREGI							\
		"POST /si/dev_del HTTP/1.1\r\n"								\
		"Host: %s:%d\r\n"											\
		"Accept:application/json\r\n"								\
		"Content-Type: application/json\r\n"						\
		"Content-Length: %d\r\n"									\
		"\r\n"

#define AUTH_BODY_FORMAT_DEVICE_DEREGI							\
		"{\r\n"														\
		"    \"_devId\": \"%s\"\r\n"								\
		"}\r\n"														\
		"\r\n"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Function Prototype
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_AuthSetServerConfig(void);

int Grib_AuthGatewayRegi(char* gatewayID);
int Grib_AuthDeviceRegi(char* deviceID, char* devicePW);

int Grib_AuthGetPW(char* deviceID, char* devicePW);
int Grib_AuthDeviceInfo(char* deviceID, char* deviceInfo);
int Grib_AuthDeviceDeRegi(char* deviceID);

#endif
