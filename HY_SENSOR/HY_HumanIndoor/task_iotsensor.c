/*
 * task_iotsensor.c
 *	Modified in "ONSB_Motion01.ino" document use in RTOS.
 *  Edited: Dae Hyeon Yim
 */ 


#include <asf.h>
#include "FreeRTOS.h"
#include "grib_system.h"
#include "arm_math.h"
#include "task_common.h"
#include "wdt.h"

int gData;
char gSensorData[HY_MAX_SIZE_BODY];

int  gDebug = GRIB_DEBUG;

char gSendBuff[MAX_SIZE_BT_BUFF];
char gRecvBuff[MAX_SIZE_BT_BUFF];
 
 #define TASK_IOTSENSOR_STACK_SIZE            (1000)
 #define TASK_IOTSENSOR_PRIORITY        (tskIDLE_PRIORITY + 4) //2)
 
 static void task_iotsensor(void *pvParameters);
 
 char SensingResult[11] = {'\0', };
 
 static xSemaphoreHandle iotsensor_GWrespose;
 
 void task_iotsensor_init(void)
 {
 	vSemaphoreCreateBinary(iotsensor_GWrespose);
 	

 	 //Create the task
 	xTaskCreate(task_iotsensor, (const signed char *const) "tskIotSnsr", TASK_IOTSENSOR_STACK_SIZE, NULL, TASK_IOTSENSOR_PRIORITY, NULL);
 	
 }
 
 int getHumanIndoor(void)
{
    int  currIndex = 0;
    int  warnIndex = 0;
    const char HY_HEAD_SYMBOL[HY_MAX_SIZE_HEAD] = {'H', 'Y', 'U'};

    int  iReadByte = 0;
//     setMyLed(LOW);							//Just No LED

    /* The results of HYU_HumanIndoor Sensor
        The Example of Results: HYU1091O00
								HYU1091X00
        HYU: Header Symbol
        1: Function Type
        09: Sensor ID
        0~3: Status->	0: Initial Step 1 
						1: Initial Step 2
						2: Initial Step 3
						3: run
        O, X: Status: Raw Data(0, X)
        00: Dumy
    */

    if (uart_receive_Available()) // when receiving the BLE message.
    {
        memset(gSensorData, '\0', sizeof(gSensorData));
		memcpy(gSensorData,SensingResult,HY_MAX_SIZE_BODY);
		
		iReadByte=HY_MAX_SIZE_BODY;// To always be satisfied

//         if(gDebug){							// Only For Debug, Requires another serial port
//         Serial.print("# READ: ");
//         Serial.println(gSensorData);
//         Serial.print("# BYTE: ");
//         Serial.println(iReadByte);
//         }

        if (iReadByte == HY_MAX_SIZE_BODY)
        {
            char sensorStatus = HY_STATUS_SENSOR_NONE;
            char sensorValue  = HY_SENSOR_VALUE_DEFAULT;

            sensorStatus = gSensorData[HY_DATA_INDEX_STATUS];
//             if(gDebug){						// Only For Debug, Requires another serial port
//             Serial.print(F("# SENSOR STATUS: "));
//             Serial.println(sensorStatus);
//             }

            switch (sensorStatus)
            {
                case HY_STATUS_SENSOR_NONE:
                case HY_STATUS_SENSOR_1:
                case HY_STATUS_SENSOR_2:
                {   //TBD ...
                    return 1;
                }
                case HY_STATUS_SENSOR_DATA:
                {   //Ready
                    break;
                }
                default:
                {   //shbaek: Error !!!
                    break;
                }
            }

            sensorValue = gSensorData[HY_DATA_INDEX_SENSOR_VALUE];
//             if(gDebug){						// Only For Debug, Requires another serial port
//             Serial.print(F("# SENSOR VALUE: "));
//             Serial.println(sensorValue);
//             }
            if (sensorValue == HY_SENSOR_VALUE_DETECT)
            {   //shbaek: Detect
                gData = true;
            }
            else if (sensorValue == HY_SENSOR_VALUE_NOT_DETECT)
            {   //shbaek: Not Detect
                gData = false;
            }
            else
            {//shbaek: for Error Handle
                    return 1;
            }
        }//if (iReadByte == HY_MAX_SIZE_BODY)
        
    }//if (Serial.available())

    return RES_OK;
}
 int myLoop(void)
 {
	 int ResFlag=0;
	 ResFlag=getHumanIndoor();

	 return ResFlag;
 }
 
 static void task_iotsensor(void *pvParameters)
 {
	 char *bleSendMsg = "AT+RESET\r\n";
	 char *bleTestMsg = "AT\r\n";
	 char  bleRecvMsg[MAX_SIZE_BT_BUFF] = {'\0', };
	 GribFuncParam pParam;
 
	myBleReset();

	pParam.sendMsg = gSendBuff;
	pParam.recvMsg = gRecvBuff;
 
// 	 Serial.println(DEVICE_ID);					// Only For Debug, Requires another serial port
// 	 Serial.println(DEVICE_LOC);
// 	 Serial.println(DEVICE_DESC);
// 	 Serial.println(REPORT_CYCLE_TIME);
// 	 Serial.println(DEVICE_FUNC_COUNT);

	memset(gSendBuff, '\0', sizeof(gSendBuff));
	getBleName(&pParam);
	
	if ( strncmp(pParam.sendMsg, DEVICE_NICK_NAME, strlen(DEVICE_NICK_NAME)) != 0)
	{
		memset(gSendBuff, '\0', sizeof(gSendBuff));
		memset(gRecvBuff, '\0', sizeof(gRecvBuff));
		snprintf(gRecvBuff, sizeof(gRecvBuff), "BLE+NAME=%s", DEVICE_NICK_NAME);
// 		Serial.print(F("# COMMAND MESSAGE : "));
// 		Serial.println(gRecvBuff);

		setBleName(&pParam);
		vTaskDelay(200 / portTICK_RATE_MS);//shbaek: Need More Than 200mSec
	}
	
 	getBleAddr(gSendBuff);
	 
	 memset(gSendBuff, 0x00, sizeof(gSendBuff));
	 memset(gRecvBuff, 0x00, sizeof(gRecvBuff));
	
	portTickType	nIotLoopLastWakeTime;
	portTickType	nIotLoopFrequency = (20UL / portTICK_RATE_MS);	// 20 msec Loop Delay Time

	//shbaek: User Settings
	mySetup();
	
 	for (;;) 
 	{
		nIotLoopLastWakeTime = xTaskGetTickCount();

		int i = 0;
		int iError = RES_ERROR;
		int ResFlag=0;

		#if USE_MY_LOOP //shbaek: User Run Loop
			ResFlag=myLoop();
		#endif

		//# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
		//# shbaek: BLE Receive Message
		//# dhyim: Modified For RTOS
		//# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
		if (uart_receive_Available()>0 && ResFlag==0)//true: BLE Mag received.
		{
			GribFunc pHandler;
			GribFuncParam pParam;

			memset(gSendBuff, 0x00, sizeof(gSendBuff));
			memset(gRecvBuff, 0x00, sizeof(gRecvBuff));

			uart_receive_data(gRecvBuff);

// 			Serial.println();					// Only For Debug, Requires another serial port
// 			if (gDebug) {
// 				Serial.print(F("# BT RECV LEN: "));
// 				Serial.println(strlen(gRecvBuff));
// 			}
// 			Serial.print(F("# BT RECV MSG: "));
// 			Serial.println(gRecvBuff);

			//# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
			//# shbaek: Find Command Handler & Call
			//# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
			pParam.sendMsg = gSendBuff;
			pParam.recvMsg = gRecvBuff;

			pHandler = findHandler(gRecvBuff);
			if (pHandler == NULL)
			{
				//shbaek: I Don't Know...
				snprintf(pParam.sendMsg, MAX_SIZE_BT_BUFF, "%s:%d", RES2STRING(RES_ERROR), GRIB_ERROR_INVALID_COMMAND);
			}
			else
			{
				iError = pHandler(&pParam);
			}
			strncat(pParam.sendMsg, GRIB_BLE_MSG_END_SYMBOL, strlen(GRIB_BLE_MSG_END_SYMBOL)); //shbaek:[TBD] NEED? or NOT?
			uart_send_data(pParam.sendMsg,strlen(pParam.sendMsg));//Send BLE Msg.

// 			Serial.print(F("# BT SEND MSG: "));	// Only For Debug, Requires another serial port
// 			Serial.println(pParam.sendMsg);

		}//shbaek: if (btSerial.available())

 		vTaskDelayUntil(&nIotLoopLastWakeTime, nIotLoopFrequency);
 	}	
	 
 }
 
 //# ########## ########## ########## ########## ########## ########## ########## ##########
 //# shbaek: [USER-MOD] USER FUNCTION
 //# ########## ########## ########## ########## ########## ########## ########## ##########
 void mySetup(void)
 {
	 //shbaek: [USER-MOD] User Init Settings... 
	 //dhyim: no needed...
// 	 pinMode( SENSOR_PIN, INPUT);
// 	 pinMode( MYLED, OUTPUT);

	 gData = 0;
 }

int myReport(GribFuncParam* pParam)
{
	int iError = RES_ERROR;

// 	if (gDebug) {								// Only For Debug, Requires another serial port
// 		Serial.print("# REPORT DATA: ");
// 		Serial.println(gData);
// 	}

	if ( (SENSOR_VALUE_MIN <= gData) && (gData <= SENSOR_VALUE_MAX) )
	{
		iError = RES_OK;
		snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%d", gData);
	}
	else
	{
		iError = RES_ERROR;
		snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%s:%d", RES2STRING(RES_ERROR), GRIB_ERROR_SENSOR);
	}

	return iError;
}
