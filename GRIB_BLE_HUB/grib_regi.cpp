/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "include/grib_regi.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */


int Grib_HubRegi(void)
{
	int i = 0;
	int iRes = GRIB_ERROR;

	const int   HUB_FUNC_COUNT = 2;
	const char* HUB_FUNC_NAME[HUB_FUNC_COUNT] = {ONEM2M_URI_CONTENT_HUB, ONEM2M_URI_CONTENT_DEVICE};

	Grib_ConfigInfo configInfo;
	Grib_DbRowDeviceInfo  rowDeviceInfo;
	Grib_DbRowDeviceFunc* pRowDeviceFunc;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	MEMSET(&configInfo, 0x00, sizeof(configInfo));
	MEMSET(&rowDeviceInfo, 0x00, sizeof(rowDeviceInfo));
	MEMSET(&reqParam, 0x00, sizeof(reqParam));
	MEMSET(&resParam, 0x00, sizeof(resParam));

	iRes = Grib_LoadConfig(&configInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# LOAD CONFIG ERROR !!!\n");
		return iRes;
	}

	GRIB_LOGD("# HUB REGI: [ID: %s]\n", configInfo.hubID);

	STRINIT(rowDeviceInfo.deviceID, sizeof(rowDeviceInfo.deviceID));
	STRNCPY(rowDeviceInfo.deviceID, configInfo.hubID, STRLEN(configInfo.hubID));

	rowDeviceInfo.deviceFuncCount = 2;
	rowDeviceInfo.ppRowDeviceFunc = (Grib_DbRowDeviceFunc**)MALLOC(rowDeviceInfo.deviceFuncCount*sizeof(Grib_DbRowDeviceFunc*));
	for(i=0; i<rowDeviceInfo.deviceFuncCount; i++)
	{
		rowDeviceInfo.ppRowDeviceFunc[i] = (Grib_DbRowDeviceFunc*)MALLOC(sizeof(Grib_DbRowDeviceFunc));
		pRowDeviceFunc = rowDeviceInfo.ppRowDeviceFunc[i];

		STRINIT(pRowDeviceFunc->funcName, sizeof(pRowDeviceFunc->funcName));
		STRNCPY(pRowDeviceFunc->funcName, HUB_FUNC_NAME[i], STRLEN(HUB_FUNC_NAME[i]));
	}

	iRes = Grib_CreateOneM2MTree(&rowDeviceInfo);
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

int Grib_DeviceRegi(char* deviceAddr)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	
	Grib_DbRowDeviceInfo  rowDeviceInfo;
	Grib_DbRowDeviceFunc* pRowDeviceFunc;

	if(deviceAddr == NULL)
	{
		GRIB_LOGD("# DEVICE BLE ADDR IS NULL !!!\n");
		return GRIB_ERROR;
	}

	MEMSET(&rowDeviceInfo, GRIB_INIT, sizeof(Grib_DbRowDeviceInfo));
	STRNCPY(rowDeviceInfo.deviceAddr, deviceAddr, STRLEN(deviceAddr));

	GRIB_LOGD("# ##### ##### ##### ##### CREATE DATABASE  ##### ##### ##### #####\n");
	iRes = Grib_DbCreate();
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# NEED NOT DB CREATE\n");
		Grib_DbClose();
	}
	Grib_DbClose();

	GRIB_LOGD("# ##### ##### ##### ##### GET BLE DEVICE INFO ##### ##### ##### #####\n");
	iRes = Grib_BleDeviceInfo(&rowDeviceInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# GET BLE DEVICE INFO ERROR\n");
		goto FINAL;
	}

	GRIB_LOGD("# ##### ##### ##### ##### CREATE ONEM2M TREE  ##### ##### ##### #####\n");
	iRes = Grib_CreateOneM2MTree(&rowDeviceInfo);
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

int Grib_DeviceDeRegi(char* deviceID)
{
	int iRes = GRIB_ERROR;

	OneM2M_ReqParam reqParam;
	OneM2M_ResParam resParam;

	STRINIT(&reqParam.xM2M_Origin, sizeof(reqParam.xM2M_Origin));
	STRNCPY(&reqParam.xM2M_Origin, deviceID, STRLEN(deviceID));

/*
	GRIB_LOGD("# ##### ##### ##### ##### DELETE APP ENTITY  ##### ##### ##### #####\n");
	iRes = Grib_AppEntityDelete(&reqParam, &resParam);
	if(iRes == GRIB_ERROR)
	{
		GRIB_LOGD("# DELETE APP ENTITY FAIL\n");
	}
*/
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

