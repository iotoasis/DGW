#ifndef __GRIB_TEST_H__
#define __GRIB_TEST_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <time.h>
#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/ioctl.h>
#include <sys/socket.h>


#include "grib_define.h"
#include "grib_util.h"
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//3 shbaek: ##### ##### ##### ##### TEST ONLY ##### ##### ##### ##### 
#define TEST_PLATFORM_SERVER_IP							"192.168.0.7"
#define TEST_PLATFORM_SERVER_PORT							8080

#define TEST_DEVICE_COUNT									2

#define TEST_DEVICE_01_NAME								"DEVICE_01"
#define TEST_DEVICE_01_ADDR								"00:15:83:00:3C:27"
#define TEST_DEVICE_01_DESC								"GRIB_LED_2EA_TEST_DEVICE_01"
#define TEST_DEVICE_01_LOC								"MY OFFICE"
#define TEST_DEVICE_01_FUNC_COUNT							2
#define TEST_DEVICE_01_FUNC_NAME_01						"Led1"
#define TEST_DEVICE_01_FUNC_NAME_02						"Led2"
#define TEST_DEVICE_01_EXE_RSRCID_01						"CONTAINER_332"
#define TEST_DEVICE_01_EXE_RSRCID_02						"CONTAINER_333"

#define TEST_DEVICE_02_NAME								"DEVICE_02"
#define TEST_DEVICE_02_ADDR								"00:15:83:00:42:61"
#define TEST_DEVICE_02_DESC								"GRIB_LED_2EA_TEST_DEVICE_02"
#define TEST_DEVICE_02_LOC								"MY OFFICE"
#define TEST_DEVICE_02_FUNC_COUNT							2
#define TEST_DEVICE_02_FUNC_NAME_01						"Led1"
#define TEST_DEVICE_02_FUNC_NAME_02						"Led2"
#define TEST_DEVICE_02_EXE_RSRCID_01						"CONTAINER_338"
#define TEST_DEVICE_02_EXE_RSRCID_02						"CONTAINER_339"


#define TEST_DEVICE_HOME_ADDR								"00:15:83:00:41:71"


#define TEST_BLE_PIPE_FILE_NAME							"TEST_PIPE_FILE"

#define TEST_SDA_DEVICE_ID								"ONSB_BleScanner01_001"

#define TEST_BASE64_ENC_SRC								"Grib Test Base 64 Text"
#define TEST_BASE64_DEC_SRC								"R3JpYiBUZXN0IEJhc2UgNjQgVGV4dA=="


int Grib_TestMain(int argc, char **argv);
int Grib_TestCJson(int argc, char **argv);


#endif
