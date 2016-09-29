#ifndef __GRIB_CONFIG_H__
#define __GRIB_CONFIG_H__

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: MANDATORY DATA
//# ########## ########## ########## ########## ########## ########## ########## ##########
//shbaek: [USER-MOD] Device's Info
#define DEVICE_TYPE                 "ONDB"//shbaek: Sensor? Device? Another?
#define PREFIX_NAME                 ""      //shbaek: Function Prefix Name
#define DEVICE_FUNC                 "SmartLight" //shbaek: Function Main Name
#define SUFFIX_NAME                 "00"      //shbaek: Function Suffix Name
#define DEVICE_INDEX                "000"   //shbaek: NNN Type Index

#define DEVICE_FUNC_COUNT           1       //shbaek: Device's Function Count
#define REPORT_CYCLE_TIME           30      //shbaek: Report Cycle Time(sec)
#define DELAY_LOOP                  0     //shbaek: Main Loop Delay

#define USE_MY_LOOP                 false    //shbaek: Detect Type Sensor[true/false]

#define GRIB_DEBUG                  false   //shbaek: Debug Flag[true/false]

//# ########## ########## ########## ########## ########## ########## ########## ##########
//shbaek: PIN NUMBER
//# ########## ########## ########## ########## ########## ########## ########## ##########
#define BT_TX                       2
#define BT_RX                       3
#define BT_STATE                    12
#define MYLED                       13  //shbaek: Aduino Uno Default LED

#define SENSOR_PIN                  4
#define SENSOR_VALUE_MIN            0
#define SENSOR_VALUE_MAX            1
#define SENSOR_VALUE_INVALID        -1

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: Auto Define
//# ########## ########## ########## ########## ########## ########## ########## ##########
#define DEVICE_LOC                  "TBD"
#define DEVICE_DESC                 "TBD"
#define FUNC_NAME                   "Power"
#define DEVICE_ID                   DEVICE_TYPE "_" PREFIX_NAME DEVICE_FUNC SUFFIX_NAME "_" DEVICE_INDEX
#define DEVICE_NICK_NAME            DEVICE_FUNC SUFFIX_NAME "_" DEVICE_INDEX

#endif

