/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_regi.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#ifdef FEATURE_CAS
int Grib_HubRegi(char* pAuthKey)
#else
int Grib_HubRegi(void)
#endif
{
	int i = 0;
	int iRes = GRIB_ERROR;

	const int   HUB_FUNC_COUNT = 2;
	const char* HUB_FUNC_NAME[HUB_FUNC_COUNT] = {ONEM2M_URI_CONTENT_HUB, ONEM2M_URI_CONTENT_DEVICE};

	Grib_ConfigInfo* pConfigInfo = NULL;
	Grib_DbRowDeviceInfo  rowDeviceInfo;
	Grib_DbRowDeviceFunc* pRowDeviceFunc = NULL;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	MEMSET(&rowDeviceInfo, 0x00, sizeof(rowDeviceInfo));
	MEMSET(&reqParam, 0x00, sizeof(reqParam));
	MEMSET(&resParam, 0x00, sizeof(resParam));

	pConfigInfo = Grib_GetConfigInfo();
	if(pConfigInfo == NULL)
	{
		GRIB_LOGD("GET CONFIG ERROR !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# HUB REGI: ID[%d]: %s\n", STRLEN(pConfigInfo->hubID), pConfigInfo->hubID);

	rowDeviceInfo.deviceInterface = DEVICE_IF_TYPE_INTERNAL;

	STRINIT(rowDeviceInfo.deviceID, sizeof(rowDeviceInfo.deviceID));
	STRNCPY(rowDeviceInfo.deviceID, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

	rowDeviceInfo.deviceFuncCount = 2;
	rowDeviceInfo.ppRowDeviceFunc = (Grib_DbRowDeviceFunc**)MALLOC(rowDeviceInfo.deviceFuncCount*sizeof(Grib_DbRowDeviceFunc*));
	for(i=0; i<rowDeviceInfo.deviceFuncCount; i++)
	{
		rowDeviceInfo.ppRowDeviceFunc[i] = (Grib_DbRowDeviceFunc*)MALLOC(sizeof(Grib_DbRowDeviceFunc));
		pRowDeviceFunc = rowDeviceInfo.ppRowDeviceFunc[i];

		STRINIT(pRowDeviceFunc->funcName, sizeof(pRowDeviceFunc->funcName));
		STRNCPY(pRowDeviceFunc->funcName, HUB_FUNC_NAME[i], STRLEN(HUB_FUNC_NAME[i]));
	}


#ifdef FEATURE_CAS
	iRes = Grib_CreateOneM2MTree(&rowDeviceInfo, pAuthKey);
#else
	iRes = Grib_CreateOneM2MTree(&rowDeviceInfo);
#endif

	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# CREATE ONEM2M TREE ERROR\n");
		goto FINAL;
	}

FINAL:

	Grib_DbFreeRowFunc(rowDeviceInfo.ppRowDeviceFunc, rowDeviceInfo.deviceFuncCount);
 	Grib_DbClose();

	return iRes;
}

int Grib_DeviceRegi(char* deviceAddr, int optAuth)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	
	Grib_DbRowDeviceInfo  rowDeviceInfo;
	Grib_DbRowDeviceFunc* pRowDeviceFunc = NULL;

#ifdef FEATURE_CAS
	char pAuthKey[CAS_AUTH_KEY_SIZE] = {'\0', };
#endif

	if(deviceAddr == NULL)
	{
		GRIB_LOGD("# DEVICE BLE ADDR IS NULL !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# DEVICE REGI OPT: %d\n", optAuth);

	MEMSET(&rowDeviceInfo, GRIB_INIT, sizeof(rowDeviceInfo));
	STRNCPY(rowDeviceInfo.deviceAddr, deviceAddr, STRLEN(deviceAddr));

	GRIB_LOGD("# ##### ##### ##### ##### CREATE DATABASE  ##### ##### ##### #####\n");
	iRes = Grib_DbCreate();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# NEED NOT DB CREATE\n");
		Grib_DbClose();
	}
//	Grib_DbClose();

	GRIB_LOGD("# ##### ##### ##### ##### GET BLE DEVICE INFO ##### ##### ##### #####\n");
	iRes = Grib_BleDeviceInfo(&rowDeviceInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# GET BLE DEVICE INFO ERROR\n");
		goto FINAL;
	}

	if(optAuth == AUTH_REGI_OPT_PW_OVER_WRITE)
	{
		GRIB_LOGD("# TRY AUTH REGI ...\n");

		iRes = Grib_AuthDeviceRegi(rowDeviceInfo.deviceID, GRIB_NOT_USED);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# AUTH REGI FAIL ...\n");
		}
	}

#ifdef FEATURE_CAS
	if(optAuth == AUTH_REGI_OPT_PW_RE_USED)
	{
		iRes = Grib_AuthGetPW(rowDeviceInfo.deviceID, pAuthKey);
	}
	else
	{
		iRes = Grib_CasGetAuthKey(rowDeviceInfo.deviceID, pAuthKey);
	}

	GRIB_LOGD("# DEVICE REGI: %s GET AUTH KEY: %s\n", rowDeviceInfo.deviceID, pAuthKey);
#endif

	GRIB_LOGD("# ##### ##### ##### ##### CREATE ONEM2M TREE  ##### ##### ##### #####\n");

#ifdef FEATURE_CAS
	iRes = Grib_CreateOneM2MTree(&rowDeviceInfo, pAuthKey);
#else
	iRes = Grib_CreateOneM2MTree(&rowDeviceInfo);
#endif

	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# CREATE ONEM2M TREE ERROR\n");
		goto FINAL;
	}

	GRIB_LOGD("# ##### ##### ##### ##### INSERT DEVICE INFO  ##### ##### ##### #####\n");
	iRes = Grib_DbSetDeviceInfo(&rowDeviceInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# INSERT ROW DEVICE INFO ERROR\n");
		goto FINAL;
	}

	GRIB_LOGD("# ##### ##### ##### ##### INSERT DEVICE FUNC  ##### ##### ##### #####\n");
	for(i=0; i<rowDeviceInfo.deviceFuncCount; i++)
	{
		pRowDeviceFunc = rowDeviceInfo.ppRowDeviceFunc[i];

		STRINIT(pRowDeviceFunc->deviceID, DEVICE_MAX_SIZE_ID);
		STRNCPY(pRowDeviceFunc->deviceID, rowDeviceInfo.deviceID, STRLEN(rowDeviceInfo.deviceID));

		iRes = Grib_DbSetDeviceFunc(pRowDeviceFunc);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# INSERT ROW DEVICE FUNC ERROR\n");
			goto FINAL;
		}
	}

FINAL:

	Grib_DbFreeRowFunc(rowDeviceInfo.ppRowDeviceFunc, rowDeviceInfo.deviceFuncCount);
 	Grib_DbClose();

	return iRes;
}

int Grib_DeviceDeRegi(char* deviceID, int delOneM2M)
{
	int iRes = GRIB_ERROR;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

#ifdef FEATURE_CAS
	char pAuthKey[CAS_AUTH_KEY_SIZE] = {'\0', };
#endif

	STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	STRNCPY(&reqParam.xM2M_Origin, deviceID, STRLEN(deviceID));

	if(delOneM2M == TRUE)
	{
		GRIB_LOGD("# ##### ##### ##### ##### DELETE APP ENTITY  ##### ##### ##### #####\n");

#ifdef FEATURE_CAS
		iRes = Grib_CasGetAuthKey(deviceID, pAuthKey);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# DEVICE DEREGI: %s GET AUTH KEY ERROR !!!\n", deviceID);
		}
		reqParam.authKey = pAuthKey;
#endif

		iRes = Grib_AppEntityDelete(&reqParam, &resParam);
		if(iRes == GRIB_ERROR)
		{
			GRIB_LOGD("# DELETE APP ENTITY FAIL\n");
		}
	}
	GRIB_LOGD("# ##### ##### ##### ##### DELETE DEVICE FUNC ##### ##### ##### #####\n");
	iRes = Grib_DbDelDeviceFunc(deviceID);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("# DELETE DEVICE FUNC FAIL\n");
	}
	
	GRIB_LOGD("# ##### ##### ##### ##### DELETE DEVICE INFO ##### ##### ##### #####\n");
	iRes = Grib_DbDelDeviceInfo(deviceID);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("# DELETE DEVICE INFO FAIL\n");
	}

	return iRes;
}

