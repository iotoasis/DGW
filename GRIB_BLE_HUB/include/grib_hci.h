#ifndef __GRIB_BLE_H__
#define __GRIB_BLE_H__

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
//#include "grib_util.h"

#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define HCI_DEFAULT_SCAN_COUNT				100
#define HCI_DEVICE_NAME_MAX_SIZE				128

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: HCI TOOL DEPENDENCY
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define FLAGS_AD_TYPE 							0x01
#define FLAGS_LIMITED_MODE_BIT 				0x01
#define FLAGS_GENERAL_MODE_BIT 				0x02

#define EIR_FLAGS                   			0x01  /* flags */
#define EIR_UUID16_SOME             			0x02  /* 16-bit UUID, more available */
#define EIR_UUID16_ALL             			0x03  /* 16-bit UUID, all listed */
#define EIR_UUID32_SOME             			0x04  /* 32-bit UUID, more available */
#define EIR_UUID32_ALL             			0x05  /* 32-bit UUID, all listed */
#define EIR_UUID128_SOME            			0x06  /* 128-bit UUID, more available */
#define EIR_UUID128_ALL             			0x07  /* 128-bit UUID, all listed */
#define EIR_NAME_SHORT              			0x08  /* shortened local name */
#define EIR_NAME_COMPLETE           			0x09  /* complete local name */
#define EIR_TX_POWER                			0x0A  /* transmit power level */
#define EIR_DEVICE_ID               			0x10  /* device ID */

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#endif
