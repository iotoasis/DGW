#ifndef __GRIB_CONFIG_H__
#define __GRIB_CONFIG_H__

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: MANDATORY DATA
//# ########## ########## ########## ########## ########## ########## ########## ##########
//shbaek: [USER-MOD] Device's Info
#define DEVICE_TYPE                 "ONSB"//shbaek: Sensor? Device? Another?
#define PREFIX_NAME                 "HY"      //shbaek: Function Prefix Name
#define DEVICE_FUNC                 "HumanIndoor" //shbaek: Function Main Name
#define SUFFIX_NAME                 "01"      //shbaek: Function Suffix Name
#define DEVICE_INDEX                "005"   //shbaek: NNN Type Index

#define DEVICE_FUNC_COUNT           1       //shbaek: Device's Function Count
#define REPORT_CYCLE_TIME           30      //shbaek: Report Cycle Time(sec)
#define DELAY_LOOP                  0     //shbaek: Main Loop Delay

#define USE_MY_LOOP                 true    //shbaek: Detect Type Sensor[true/false]

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
#define FUNC_NAME                   DEVICE_FUNC
#define DEVICE_ID                   DEVICE_TYPE "_" PREFIX_NAME FUNC_NAME SUFFIX_NAME "_" DEVICE_INDEX
#define DEVICE_NICK_NAME            PREFIX_NAME "HumanIn_" DEVICE_INDEX

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: Use Only HYU
//# ########## ########## ########## ########## ########## ########## ########## ##########
#define HY_MAX_SIZE_HEAD            3
#define HY_MAX_SIZE_BODY            7
#define HY_MAX_SIZE_DATA            (HY_MAX_SIZE_HEAD+HY_MAX_SIZE_BODY)
#define HY_MAX_COUNT_READ_LOOP      1000

#define HY_DATA_INDEX_HEAD1         0
#define HY_DATA_INDEX_HEAD2         1
#define HY_DATA_INDEX_HEAD3         2

#define HY_DATA_INDEX_FUNC_TYPE     0
#define HY_DATA_INDEX_SENSOR_ID1    1
#define HY_DATA_INDEX_SENSOR_ID2    2
#define HY_DATA_INDEX_STATUS        3
#define HY_DATA_INDEX_SENSOR_VALUE  4
#define HY_DATA_INDEX_DUMY_1        5
#define HY_DATA_INDEX_DUMY_2        6

#define HY_STATUS_SENSOR_NONE       '0'
#define HY_STATUS_SENSOR_1          '1'
#define HY_STATUS_SENSOR_2          '2'
#define HY_STATUS_SENSOR_DATA       '3'

#define HY_SENSOR_VALUE_DEFAULT     '_'
#define HY_SENSOR_VALUE_DETECT      'O'
#define HY_SENSOR_VALUE_NOT_DETECT  'X'


#endif

