#ifndef __GRIB_DB_H__
#define __GRIB_DB_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#include <mysql.h>
#include <my_global.h>
#include "grib_define.h"
#include "grib_util.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define MYSQL_DB_HOST									"127.0.0.1"
#define MYSQL_DB_USER									"root"
#define MYSQL_DB_PSWD									"mysql"
#define MYSQL_DB_PORT									3306

#define MYSQL_ERROR_NUM(x)								mysql_errno(x)
#define MYSQL_ERROR_STR(x)								mysql_error(x)

#define MYSQL_DB_NAME									"grib_iot_db"
#define MYSQL_DB_ENGINE_TYPE							"INNODB"

#define MYSQL_TABLE_DEVICE_INFO						"device_info_table"
#define MYSQL_TABLE_DEVICE_FUNC						"device_func_table"

#define MYSQL_COLUMN_DEVICE_ID						"device_id"
#define MYSQL_COLUMN_DEVICE_IF						"device_if"
#define MYSQL_COLUMN_DEVICE_ADDR						"device_addr"
#define MYSQL_COLUMN_DEVICE_FCOUNT					"device_fcount"
#define MYSQL_COLUMN_DEVICE_DESC						"device_desc"
#define MYSQL_COLUMN_DEVICE_LOC						"device_loc"
#define MYSQL_COLUMN_REPORT_CYCLE						"report_cycle"

#define MYSQL_COLUMN_FUNC_NAME						"func_name"
#define MYSQL_COLUMN_FUNC_EXRI						"execute_rid"
#define MYSQL_COLUMN_FUNC_ATTR						"func_attr"

#define MYSQL_MAX_SIZE_QUERY							SIZE_1K

#define MYSQL_QUERY_DB_CREATE							"CREATE DATABASE " MYSQL_DB_NAME
#define MYSQL_QUERY_DB_USE							"USE " MYSQL_DB_NAME
#define MYSQL_QUERY_DB_DROP							"DROP DATABASE IF EXISTS " MYSQL_DB_NAME

#define MYSQL_QUERY_CREATE_DEVICE_INFO				"CREATE TABLE " MYSQL_TABLE_DEVICE_INFO "("\
														MYSQL_COLUMN_DEVICE_ID 		" VARCHAR(128) NOT NULL,"\
														MYSQL_COLUMN_DEVICE_IF		" INT ZEROFILL,"\
														MYSQL_COLUMN_DEVICE_ADDR 	" VARCHAR(128) NOT NULL,"\
														MYSQL_COLUMN_DEVICE_FCOUNT 	" INT NOT NULL,"\
														MYSQL_COLUMN_DEVICE_LOC 	" TEXT,"\
														MYSQL_COLUMN_DEVICE_DESC 	" TEXT,"\
														MYSQL_COLUMN_REPORT_CYCLE	" INT ZEROFILL,"\
														"PRIMARY KEY(`" MYSQL_COLUMN_DEVICE_ID "`)"\
														")ENGINE=" MYSQL_DB_ENGINE_TYPE

#define MYSQL_QUERY_CREATE_DEVICE_FUNC				"CREATE TABLE " MYSQL_TABLE_DEVICE_FUNC "("\
														MYSQL_COLUMN_DEVICE_ID 		" VARCHAR(128) NOT NULL,"\
														MYSQL_COLUMN_FUNC_NAME 		" TEXT NOT NULL,"\
														MYSQL_COLUMN_FUNC_EXRI		" VARCHAR(128) NOT NULL,"\
														MYSQL_COLUMN_FUNC_ATTR 		" INT ZEROFILL,"\
														"PRIMARY KEY(`" MYSQL_COLUMN_FUNC_EXRI "`),"\
														"FOREIGN KEY (`" MYSQL_COLUMN_DEVICE_ID "`) "\
														"REFERENCES `" MYSQL_TABLE_DEVICE_INFO "` (`" MYSQL_COLUMN_DEVICE_ID "`)"\
														")ENGINE=" MYSQL_DB_ENGINE_TYPE

#define MYSQL_QUERY_INSERT_DEVICE_INFO				"INSERT INTO "   MYSQL_TABLE_DEVICE_INFO " VALUES(\"%s\", %d, \"%s\", %d, \"%s\", \"%s\", %d)"
#define MYSQL_QUERY_SELECT_DEVICE_INFO				"SELECT * FROM " MYSQL_TABLE_DEVICE_INFO " WHERE " MYSQL_COLUMN_DEVICE_ID "=\"%s\""
#define MYSQL_QUERY_SELECT_DEVICE_INFO_ALL			"SELECT * FROM " MYSQL_TABLE_DEVICE_INFO
#define MYSQL_QUERY_DELETE_DEVICE_INFO				"DELETE FROM " 	 MYSQL_TABLE_DEVICE_INFO " WHERE " MYSQL_COLUMN_DEVICE_ID "=\"%s\""

#define MYSQL_QUERY_INSERT_DEVICE_FUNC				"INSERT INTO "   MYSQL_TABLE_DEVICE_FUNC " VALUES(\"%s\", \"%s\", \"%s\", %d)"
#define MYSQL_QUERY_SELECT_DEVICE_FUNC				"SELECT * FROM " MYSQL_TABLE_DEVICE_FUNC " WHERE " MYSQL_COLUMN_DEVICE_ID "=\"%s\""
#define MYSQL_QUERY_DELETE_DEVICE_FUNC				"DELETE FROM "	 MYSQL_TABLE_DEVICE_FUNC " WHERE " MYSQL_COLUMN_DEVICE_ID "=\"%s\""

//1 shbaek: Device Info Table's Column Index
#define INDEX_DEVICE_ID								0
#define INDEX_DEVICE_INTERFACE						1
#define INDEX_DEVICE_ADDR								2
#define INDEX_DEVICE_FCOUNT							3
#define INDEX_DEVICE_LOC								4
#define INDEX_DEVICE_DESC								5
#define INDEX_REPORT_CYCLE							6

//1 shbaek: Device Func Table's Column Index
#define INDEX_FUNC_NAME								1
#define INDEX_FUNC_EXRI								2
#define INDEX_FUNC_ATTR								3


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Type Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

typedef struct
{
	char  deviceID[DEVICE_MAX_SIZE_ID+1];
	char  funcName[DEVICE_MAX_SIZE_FUNC_NAME+1];
	char  exRsrcID[DEVICE_MAX_SIZE_EX_RSRCID+1];
	int   funcAttr;
}Grib_DbRowDeviceFunc;

typedef struct
{
	char  deviceID[DEVICE_MAX_SIZE_ID+1];
	char  deviceAddr[DEVICE_MAX_SIZE_ADDR+1];
	char  deviceLoc[DEVICE_MAX_SIZE_LOC+1];
	char  deviceDesc[DEVICE_MAX_SIZE_DESC+1];
	int   deviceFuncCount;
	int   reportCycle;

	Grib_DeviceIfType  deviceInterface;
	Grib_DbRowDeviceFunc** ppRowDeviceFunc;
}Grib_DbRowDeviceInfo;

typedef struct
{
	int deviceCount;
	Grib_DbRowDeviceInfo** ppRowDeviceInfo;
}Grib_DbAll;




/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_DbCreate(void);
int Grib_DbDrop(void);

int Grib_DbOpen(void);
int Grib_DbClose(void);
int Grib_DbToMemory(Grib_DbAll *pDbAll);

int Grib_DbGetDeviceCount(void);
int Grib_DbGetDeviceInfoAll(Grib_DbRowDeviceInfo** pRowDeviceInfo);
int Grib_DbSetDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo);
int Grib_DbGetDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo);
int Grib_DbDelDeviceInfo(char* deviceID);

int Grib_DbFreeRowFunc(Grib_DbRowDeviceFunc** ppRowDeviceFunc, int iRowCount);
int Grib_DbSetDeviceFunc(Grib_DbRowDeviceFunc* pRowDeviceFunc);
int Grib_DbGetDeviceFunc(char *deviceID, Grib_DbRowDeviceFunc** ppRowDeviceFunc, int *pRowCount);
int Grib_DbDelDeviceFunc(char* deviceID);

#endif
