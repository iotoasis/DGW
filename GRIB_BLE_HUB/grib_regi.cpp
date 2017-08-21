/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_regi.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

int Grib_HubRegi(char* pAuthKey)
{
	int i = 0;
	int iRes = GRIB_ERROR;

	const int   HUB_FUNC_COUNT = 2;
	const char* HUB_FUNC_NAME[HUB_FUNC_COUNT] = {ONEM2M_URI_CONTENT_HUB, ONEM2M_URI_CONTENT_DEVICE};
	const int 	HUB_FUNC_ATTR[HUB_FUNC_COUNT] = {FUNC_ATTR_USE_ALL, FUNC_ATTR_USE_REPORT};

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

		pRowDeviceFunc->funcAttr = HUB_FUNC_ATTR[i];
	}

	iRes = Grib_CreateOneM2MTree(&rowDeviceInfo, pAuthKey);
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
	const char* FUNC = "DEVICE-REGI";
	const int 	iDBG = FALSE;

	int i = 0;
	int iRes = GRIB_ERROR;
	
	Grib_DbRowDeviceInfo  rowDeviceInfo;
	Grib_DbRowDeviceFunc* pRowDeviceFunc = NULL;

	char pAuthKey[GRIB_MAX_SIZE_AUTH_KEY] = {'\0', };
	Grib_ConfigInfo* pConfigInfo = NULL;

	if(deviceAddr == NULL)
	{
		GRIB_LOGD("# DEVICE BLE ADDR IS NULL !!!\n");
		return GRIB_ERROR;
	}

	GRIB_LOGD("# DEVICE REGI OPT: %d\n", optAuth);

	MEMSET(&rowDeviceInfo, GRIB_INIT, sizeof(rowDeviceInfo));
	STRNCPY(rowDeviceInfo.deviceAddr, deviceAddr, STRLEN(deviceAddr));

	if(iDBG)GRIB_LOGD("# ##### ##### ##### ##### CREATE DATABASE  ##### ##### ##### #####\n");
	iRes = Grib_DbCreate();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# NEED NOT DB CREATE\n");
		Grib_DbClose();
	}
//	Grib_DbClose();

	GRIB_LOGD("# ##### ##### ##### ##### GET BLE DEVICE INFO ##### ##### ##### #####\n");
	iRes = Grib_BleGetDeviceInfo(&rowDeviceInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# GET BLE DEVICE INFO ERROR\n");
		goto FINAL;
	}

	if(0 < optAuth)
	{
#ifdef FEATURE_CAS

		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# GET CONFIG ERROR !!!\n");
			return GRIB_ERROR;
		}

		//shbaek: CAS Lib Init & Get Certification
		iRes = Grib_CasInit(pConfigInfo->hubID);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# CAS INIT FAIL !!!\n");
			return GRIB_ERROR;
		}
#endif

		//shbaek: You Must be Set Server Config.
		Grib_SiSetServerConfig();

		if(optAuth & REGI_OPT_PW_OVER_WRITE)
		{
			GRIB_LOGD("# TRY AUTH REGI ...\n");

			iRes = Grib_AuthDeviceRegi(rowDeviceInfo.deviceID, GRIB_NOT_USED);
			if(iRes != GRIB_DONE)
			{
				GRIB_LOGD("# AUTH REGI FAIL ...\n");
			}
		}
		else if(optAuth & REGI_OPT_PW_RE_USED)
		{
			iRes = Grib_AuthGetPW(rowDeviceInfo.deviceID, pAuthKey);
		}
#ifdef FEATURE_CAS
		else
		{
			iRes = Grib_CasGetAuthKey(rowDeviceInfo.deviceID, pAuthKey);
		}
#endif
		GRIB_LOGD("# DEVICE REGI: %s GET AUTH KEY: %s\n", rowDeviceInfo.deviceID, pAuthKey);

		if(optAuth & REGI_OPT_CREATE_RESOURCE)
		{
			GRIB_LOGD("# ##### ##### ##### ##### CREATE ONEM2M TREE  ##### ##### ##### #####\n");
			iRes = Grib_CreateOneM2MTree(&rowDeviceInfo, pAuthKey);
			if(iRes != GRIB_DONE)
			{
				GRIB_LOGD("# CREATE ONEM2M TREE ERROR\n");
				goto FINAL;
			}
		}
	}
	else
	{
		GRIB_LOGD("# %c[1;33mDo Not Create OneM2M Tree ...%c[0m\n", 27, 27);
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
	GRIB_LOGD("# ##### ##### ##### ##### DEVICE REGI DONE   ##### ##### ##### #####\n");

	Grib_DbFreeRowFunc(rowDeviceInfo.ppRowDeviceFunc, rowDeviceInfo.deviceFuncCount);
 	Grib_DbClose();

	return iRes;
}

int Grib_DeviceDeRegi(char* deviceID, int delOneM2M)
{
	int iRes = GRIB_ERROR;

#ifdef FEATURE_CAS
	char pAuthKey[GRIB_MAX_SIZE_AUTH_KEY] = {'\0', };
#endif


	if(delOneM2M == TRUE)
	{
		Grib_ConfigInfo* pConfigInfo = NULL;

		OneM2M_ReqParam reqParam;
		OneM2M_ResParam resParam;

		GRIB_LOGD("# ##### ##### ##### ##### DELETE APP ENTITY  ##### ##### ##### #####\n");

		//shbaek: You Must be Set Server Config.
		Grib_SiSetServerConfig();

		pConfigInfo = Grib_GetConfigInfo();
		if(pConfigInfo == NULL)
		{
			GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
			return GRIB_ERROR;
		}

		MEMSET(&reqParam, GRIB_INIT, sizeof(OneM2M_ReqParam));
		MEMSET(&resParam, GRIB_INIT, sizeof(OneM2M_ResParam));

		STRINIT(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
//		SNPRINTF(reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin), "GRIB/%s", pConfigInfo->hubID);
		STRNCPY(reqParam.xM2M_Origin, pConfigInfo->hubID, STRLEN(pConfigInfo->hubID));

		STRINIT(reqParam.xM2M_AeName, sizeof(reqParam.xM2M_AeName));
		STRNCPY(reqParam.xM2M_AeName, deviceID, STRLEN(deviceID));

#ifdef FEATURE_CAS
		//shbaek: CAS Lib Init & Get Certification
		iRes = Grib_CasInit(pConfigInfo->hubID);
		if(iRes != GRIB_DONE)
		{
			GRIB_LOGD("# CAS INIT FAIL !!!\n");
			return GRIB_ERROR;
		}

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

