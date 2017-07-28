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
#define GRIB_DB_INFO_FILE_PATH				"./data/grib_db_info.dat"

#define GRIB_DB_INFO_MYSQL_HOST				"MYSQL_DB_HOST"
#define GRIB_DB_INFO_MYSQL_PORT				"MYSQL_DB_PORT"
#define GRIB_DB_INFO_MYSQL_USER				"MYSQL_DB_USER"
#define GRIB_DB_INFO_MYSQL_PASSWORD			"MYSQL_DB_PASSWORD"

#define MYSQL_MAX_SIZE_QUERY					SIZE_1K

#define MYSQL_ERROR_NUM(x)						mysql_errno(x)
#define MYSQL_ERROR_STR(x)						mysql_error(x)

#define MYSQL_DB_NAME							"grib_iot_db"
#define MYSQL_DB_ENGINE_TYPE					"INNODB"

#define QUERY_DB_CREATE						"CREATE DATABASE " MYSQL_DB_NAME
#define QUERY_DB_USE							"USE " MYSQL_DB_NAME
#define QUERY_DB_DROP							"DROP DATABASE IF EXISTS " MYSQL_DB_NAME

//2 shbaek: DEVICE TABLE ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

#define TABLE_DEVICE_INFO						"device_info_table"

#define INDEX_DEVICE_ID						0
#define INDEX_DEVICE_INTERFACE				1
#define INDEX_DEVICE_ADDR						2
#define INDEX_DEVICE_FCOUNT					3
#define INDEX_DEVICE_LOC						4
#define INDEX_DEVICE_DESC						5
#define INDEX_REPORT_CYCLE					6

#define COLUMN_DEVICE_ID						"device_id"
#define COLUMN_DEVICE_IF						"device_if"
#define COLUMN_DEVICE_ADDR					"device_addr"
#define COLUMN_DEVICE_FCOUNT					"device_fcount"
#define COLUMN_DEVICE_DESC					"device_desc"
#define COLUMN_DEVICE_LOC						"device_loc"
#define COLUMN_REPORT_CYCLE					"report_cycle"

#define QUERY_CREATE_DEVICE_INFO				"CREATE TABLE " TABLE_DEVICE_INFO "("\
												COLUMN_DEVICE_ID 		" VARCHAR(128) NOT NULL,"\
												COLUMN_DEVICE_IF		" INT,"\
												COLUMN_DEVICE_ADDR 		" VARCHAR(128) NOT NULL,"\
												COLUMN_DEVICE_FCOUNT 	" INT NOT NULL,"\
												COLUMN_DEVICE_LOC 		" TEXT,"\
												COLUMN_DEVICE_DESC 		" TEXT,"\
												COLUMN_REPORT_CYCLE		" INT,"\
												"PRIMARY KEY(`" COLUMN_DEVICE_ID "`)"\
												")ENGINE=" MYSQL_DB_ENGINE_TYPE

#define QUERY_INSERT_DEVICE_INFO				"INSERT INTO "   TABLE_DEVICE_INFO " VALUES(\"%s\", %d, \"%s\", %d, \"%s\", \"%s\", %d)"
#define QUERY_SELECT_DEVICE_INFO				"SELECT * FROM " TABLE_DEVICE_INFO " WHERE " COLUMN_DEVICE_ID "=\"%s\""
#define QUERY_SELECT_DEVICE_INFO_ALL			"SELECT * FROM " TABLE_DEVICE_INFO
#define QUERY_DELETE_DEVICE_INFO				"DELETE FROM " 	 TABLE_DEVICE_INFO " WHERE " COLUMN_DEVICE_ID "=\"%s\""


//2 shbaek: FUNCTION TABLE ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

#define TABLE_DEVICE_FUNC						"device_func_table"

#define INDEX_FUNC_NAME						1
#define INDEX_FUNC_ATTR						2

#define COLUMN_FUNC_NAME						"func_name"
#define COLUMN_FUNC_EXRI						"execute_rid"
#define COLUMN_FUNC_ATTR						"func_attr"

#define QUERY_CREATE_DEVICE_FUNC				"CREATE TABLE " TABLE_DEVICE_FUNC "("\
												COLUMN_DEVICE_ID 		" VARCHAR(128) NOT NULL,"\
												COLUMN_FUNC_NAME 		" TEXT NOT NULL,"\
												COLUMN_FUNC_ATTR 		" INT,"\
												"FOREIGN KEY (`" COLUMN_DEVICE_ID "`) "\
												"REFERENCES `" TABLE_DEVICE_INFO "` (`" COLUMN_DEVICE_ID "`)"\
												")ENGINE=" MYSQL_DB_ENGINE_TYPE


#define QUERY_INSERT_DEVICE_FUNC				"INSERT INTO "   TABLE_DEVICE_FUNC " VALUES(\"%s\", \"%s\", %d)"
#define QUERY_SELECT_DEVICE_FUNC				"SELECT * FROM " TABLE_DEVICE_FUNC " WHERE " COLUMN_DEVICE_ID "=\"%s\""
#define QUERY_DELETE_DEVICE_FUNC				"DELETE FROM "	 TABLE_DEVICE_FUNC " WHERE " COLUMN_DEVICE_ID "=\"%s\""

//2 shbaek: CONFIG TABLE ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

#define TABLE_CONFIG							"hub_config_table"

#define INDEX_CONFIG_HUB_ID					0
#define INDEX_CONFIG_SI_SERVER_IP				1
#define INDEX_CONFIG_SI_SERVER_PORT			2
#define INDEX_CONFIG_SI_IN_ADDR				3
#define INDEX_CONFIG_SI_CSE_ADDR				4
#define INDEX_CONFIG_AUTH_SERVER_IP			5
#define INDEX_CONFIG_AUTH_SERVER_PORT		6
#define INDEX_CONFIG_SMD_SERVER_IP			7
#define INDEX_CONFIG_SMD_SERVER_PORT			8
#define INDEX_CONFIG_RESET_HOUR				9
#define INDEX_CONFIG_DEBUG_LEVEL				10
#define INDEX_CONFIG_TOMBSTONE				11



#define COLUMN_HUB_ID							"hub_id"
#define COLUMN_SI_SERVER_IP					"si_server_ip"
#define COLUMN_SI_SERVER_PORT					"si_server_port"
#define COLUMN_SI_IN_ADDR						"si_in_name"
#define COLUMN_SI_CSE_ADDR					"si_cse_name"
#define COLUMN_AUTH_SERVER_IP					"auth_server_ip"
#define COLUMN_AUTH_SERVER_PORT				"auth_server_port"
#define COLUMN_SMD_SERVER_IP					"smd_server_ip"
#define COLUMN_SMD_SERVER_PORT				"smd_server_port"
#define COLUMN_RESET_HOUR						"reset_hour"
#define COLUMN_DEBUG_LEVEL					"debug_flag"
#define COLUMN_TOMBSTONE						"tombstone"

#define QUERY_CREATE_CONFIG					"CREATE TABLE " TABLE_CONFIG "("\
												COLUMN_HUB_ID 			" VARCHAR(256) NOT NULL,"\
												COLUMN_SI_SERVER_IP		" VARCHAR(64) NOT NULL,"\
												COLUMN_SI_SERVER_PORT	" INT,"\
												COLUMN_SI_IN_ADDR 		" VARCHAR(128) NOT NULL,"\
												COLUMN_SI_CSE_ADDR 		" VARCHAR(128) NOT NULL,"\
												COLUMN_AUTH_SERVER_IP	" VARCHAR(64) NOT NULL,"\
												COLUMN_AUTH_SERVER_PORT	" INT,"\
												COLUMN_SMD_SERVER_IP	" VARCHAR(64) NOT NULL,"\
												COLUMN_SMD_SERVER_PORT	" INT,"\
												COLUMN_RESET_HOUR		" INT,"\
												COLUMN_DEBUG_LEVEL		" INT,"\
												COLUMN_TOMBSTONE		" INT,"\
												"PRIMARY KEY(`" COLUMN_HUB_ID "`)"\
												")ENGINE=" MYSQL_DB_ENGINE_TYPE

#define QUERY_INSERT_CONFIG					"INSERT INTO " TABLE_CONFIG " VALUES(\"%s\", \"%s\", %d, \"%s\", \"%s\", \"%s\", %d, \"%s\", %d, %d, %d, %d)"
#define QUERY_UPDATE_CONFIG					"UPDATE " TABLE_CONFIG " SET %s"
#define QUERY_SELECT_CONFIG					"SELECT * FROM " TABLE_CONFIG
#define QUERY_DELETE_CONFIG					"DELETE FROM " TABLE_CONFIG


//2 shbaek: CACHE RI TABLE ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

#define TABLE_CACHE_RI							"resouce_id_table"

#define INDEX_CACHE_RI							0
#define INDEX_CACHE_TY							1
#define INDEX_CACHE_RN							2
#define INDEX_CACHE_PI							3
#define INDEX_CACHE_URI						4

#define COLUMN_RESOUCE_ID						"resouce_id"
#define COLUMN_RESOUCE_TYPE					"resouce_type"
#define COLUMN_RESOUCE_NAME					"resouce_name"
#define COLUMN_PARENTS_ID						"parents_id"
#define COLUMN_URI								"uri"

#define QUERY_CREATE_CACHE_RI					"CREATE TABLE " TABLE_CACHE_RI "("\
												COLUMN_RESOUCE_ID 	" VARCHAR(128) NOT NULL,"\
												COLUMN_RESOUCE_TYPE	" INT,"\
												COLUMN_RESOUCE_NAME 	" VARCHAR(256),"\
												COLUMN_PARENTS_ID 	" VARCHAR(128) NOT NULL,"\
												COLUMN_URI 	" VARCHAR(1024),"\
												"PRIMARY KEY(`" COLUMN_RESOUCE_ID "`)"\
												")ENGINE=" MYSQL_DB_ENGINE_TYPE

#define QUERY_INSERT_CACHE_RI					"INSERT INTO "   TABLE_CACHE_RI " VALUES(\"%s\", %d, \"%s\", \"%s\", \"%s\")"
#define QUERY_SELECT_CACHE_RI					"SELECT * FROM " TABLE_CACHE_RI " WHERE " COLUMN_RESOUCE_ID "=\"%s\""
#define QUERY_SELECT_CACHE_URI				"SELECT * FROM " TABLE_CACHE_RI " WHERE " COLUMN_URI "=\"%s\""
#define QUERY_SELECT_CACHE_RI_ALL				"SELECT * FROM " TABLE_CACHE_RI
#define QUERY_DELETE_CACHE_RI					"DELETE FROM "	 TABLE_CACHE_RI " WHERE " COLUMN_RESOUCE_ID "=\"%s\""
#define QUERY_DELETE_CACHE_RI					"DELETE FROM "	 TABLE_CACHE_RI " WHERE " COLUMN_RESOUCE_ID "=\"%s\""
#define QUERY_DELETE_CACHE_RI_ALL				"DELETE FROM "	 TABLE_CACHE_RI


//2 shbaek: SCAN DEVICE TABLE ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- ----- -----

#define TABLE_BLE_SCAN							"ble_scan_table"

#define INDEX_SCAN_ADDR						0
#define INDEX_SACN_NAME						1
#define INDEX_SCAN_PEER						2
#define INDEX_SCAN_MEMO						3

#define COLUMN_SCAN_ADDR						"scan_addr"
#define COLUMN_SACN_NAME						"scan_name"
#define COLUMN_SCAN_PEER						"peer_type"
#define COLUMN_SCAN_MEMO						"scan_memo"

#define QUERY_CREATE_SCAN_DEVICE				"CREATE TABLE " TABLE_BLE_SCAN "("\
												COLUMN_SCAN_ADDR 	" CHAR(17) NOT NULL,"\
												COLUMN_SACN_NAME	" VARCHAR(128),"\
												COLUMN_SCAN_PEER 	" INT,"\
												COLUMN_SCAN_MEMO 	" VARCHAR(128),"\
												"PRIMARY KEY(`" COLUMN_SCAN_ADDR "`)"\
												")ENGINE=" MYSQL_DB_ENGINE_TYPE

#define QUERY_INSERT_SCAN_DEVICE				"INSERT INTO "   TABLE_BLE_SCAN " VALUES(\"%s\", \"%s\", %d, \"%s\")"
#define QUERY_SELECT_SCAN_DEVICE				"SELECT * FROM " TABLE_BLE_SCAN " WHERE " COLUMN_SCAN_ADDR "=\"%s\""
#define QUERY_SELECT_SCAN_DEVICE_ALL			"SELECT * FROM " TABLE_BLE_SCAN
#define QUERY_DELETE_SCAN_DEVICE				"DELETE FROM "	 TABLE_BLE_SCAN " WHERE " COLUMN_SCAN_ADDR "=\"%s\""
#define QUERY_DELETE_SCAN_DEVICE_ALL			"DELETE FROM "	 TABLE_BLE_SCAN




/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Type Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
typedef struct
{
	MYSQL*		connect;
	MYSQL_RES*	result;

	char* host;
	char* user;
	char* pswd;
	int   port;

	char* db;
	char* errStr;
	int   errNum;
}Grib_SqlInfo;

typedef struct
{
	char  deviceID[DEVICE_MAX_SIZE_ID+1];
	char  funcName[DEVICE_MAX_SIZE_FUNC_NAME+1];
//	char  exRsrcID[DEVICE_MAX_SIZE_EX_RSRCID+1];
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
	int	  rtype;
	char  rid[GRIB_MAX_SIZE_SHORT];
	char  rname[GRIB_MAX_SIZE_MIDDLE];
	char  pid[GRIB_MAX_SIZE_SHORT];
	char  uri[GRIB_MAX_SIZE_DLONG];

}Grib_DbRowCacheRi;

typedef struct
{
	int cacheCount;
	Grib_DbRowCacheRi** ppRowCacheRi;
}Grib_DbCacheRiAll;


typedef struct
{
	int deviceCount;
	Grib_DbRowDeviceInfo** ppRowDeviceInfo;
}Grib_DbAll;


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_DbInfoLoad(void);

int Grib_FreeSqlInfo(Grib_SqlInfo* pSQL);
int Grib_MakeSqlInfo(Grib_SqlInfo* pSQL);
int Grib_ShowSqlInfo(Grib_SqlInfo* pSQL);


int Grib_DbCreate(void);
int Grib_DbDrop(void);

int Grib_DbOpen(void);
int Grib_DbClose(void);

int Grib_DbDisconnect(Grib_SqlInfo* pSqlInfo);
int Grib_DbConnect(Grib_SqlInfo* pSqlInfo);
int Grib_DbQuery(Grib_SqlInfo* pSqlInfo, char* sqlQuery);
int Grib_DbGetRowCount(Grib_SqlInfo* pSQL);


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


int Grib_DbGetCacheCount(void);
int Grib_DbGetCacheInfo(Grib_DbRowCacheRi* pRowCacheRi);
int Grib_DbGetCacheRi(Grib_DbRowCacheRi* pRowCacheRi);
int Grib_DbGetCacheRiAll(Grib_DbCacheRiAll* pCacheRiAll);
int Grib_DbSetCacheRi(Grib_DbRowCacheRi* pRowCacheRi);
int Grib_DbDelCacheRi(char* rid);
int Grib_DbDelCacheRiAll(void);


int Grib_TestDB(int argc, char **argv);

#endif
