#include "grib_system.h"

SoftwareSerial btSerial(BT_TX, BT_RX);


//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: [USER-MOD] USER FUNCTION PROTOTYPE
//# ########## ########## ########## ########## ########## ########## ########## ##########
int myReport(GribFuncParam* pParam);

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: GLOBAL VARIABLE
//# ########## ########## ########## ########## ########## ########## ########## ##########
int gData;
int gDebug = GRIB_DEBUG;
char gSendBuff[MAX_SIZE_BT_BUFF];
char gRecvBuff[MAX_SIZE_BT_BUFF];

/*
    shbaek: [USER-MOD] User Function's Name & Attribute
           Function Name -> OneM2M's Container Name.
*/
GribFuncInfo gFuncInfo[DEVICE_FUNC_COUNT] =
{
    {FUNC_NAME,       FUNC_ATTR_USE_REPORT},
};

//shbaek: Command & Handler
GribCmdHandler gCmdHandler[] =
{
    //shbeak: GRIB Command & Handler
    {"GET+DEVID?",   getDeviceID},
    {"SET+DEVID=",   setDeviceID},
    {"GET+LOC?",     getDeviceLoc},
    {"SET+LOC=",     setDeviceLoc},
    {"GET+DESC?",    getDeviceDesc},
    {"GET+FCOUNT?",  getFuncCount},
    {"GET+FNAME?",   getFuncName},
    {"GET+FATTR?",   getFuncAttr},
    {"GET+CYCLE?",   getReportCycle},
    {"BLE+NAME?",    getBleName},
    {"BLE+NAME=",    setBleName},
    {"SET+LOG=",     setLogFlag},
    {"MY+REBOOT",    myReboot},

    /*  shbeak: [USER-MOD] User Command & Handler Matching Table.
                Remember, Command Naming Rule:
                GET+"Func Name" or SET+"Func Name"
                If Have Control Attr, Must Need SET+"Func Name"
                If Have Report Attr, Must Need GET+"Func Name"
                and If Have All Attr, Must Need Both.
    */
    {"GET+" FUNC_NAME "?",     myReport},

    //shbeak: Last Point. Do Not Modify
    {NULL,           NULL}
};

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: [USER-MOD] USER FUNCTION
//# ########## ########## ########## ########## ########## ########## ########## ##########
void mySetup(void)
{
    //shbaek: [USER-MOD] User Init Settings...
    pinMode( SENSOR_PIN, INPUT);
    pinMode( MYLED, OUTPUT);

    gData = 0;
}

int getMotion(void)
{
    int iValue = 0;

    iValue = digitalRead(SENSOR_PIN);
    setMyLed(iValue);

    if (gDebug) {
        Serial.print("# SENSOR VALUE: ");
        Serial.println(iValue);
    }
    return iValue;
}

int myReport(GribFuncParam* pParam)
{
    int iError = RES_ERROR;

    if (gDebug) {
        Serial.print("# REPORT DATA: ");
        Serial.println(gData);
    }

    if ( (SENSOR_VALUE_MIN <= gData) && (gData <= SENSOR_VALUE_MAX) )
    {
        iError = RES_OK;
        snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%d", gData);
        gData = false;
    }
    else
    {
        iError = RES_ERROR;
        snprintf(pParam->sendMsg, MAX_SIZE_BT_BUFF, "%s:%d", RES2STRING(RES_ERROR), GRIB_ERROR_SENSOR);
    }

    return iError;
}

#if USE_MY_LOOP //shbaek: User Run Loop
void myLoop(void)
{
    GribFuncParam pParam;
    int iValue = 0;

    iValue = getMotion();

    if ( iValue == HIGH )
    {
        gData = true;
    }

    return;
}
#endif
//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: INIT
//# ########## ########## ########## ########## ########## ########## ########## ##########
void setup()
{
    int i = 0;
    int serialSpeed = 9600;
    int btSerialSpeed = 9600;

    char myDeviceID[MAX_SIZE_DEVICE_ID] = {'\0', };
    GribFuncParam pParam;

    Serial.begin(serialSpeed);
    btSerial.begin(btSerialSpeed);
    delay(200);//shbaek: Need More Than 200mSec

    myBleReset();
    delay(500);//shbaek: Need More Than 500mSec

    Serial.println(F("# ##### ##### ##### ##### ##### ##### ##### #####"));
    Serial.println(F("# ADUINO SET-UP START "));
    Serial.println(F("# ##### ##### ##### ##### ##### ##### ##### #####"));
    Serial.print(F("# SERIAL BAUD RATE: "));
    Serial.println(serialSpeed);

    pParam.sendMsg = gSendBuff;
    pParam.recvMsg = gRecvBuff;

    Serial.print(F("# DEVICE ID       : "));
    Serial.println(DEVICE_ID);

    Serial.print(F("# DEVICE LOC      : "));
    Serial.println(DEVICE_LOC);

    Serial.print(F("# DEVICE DESC     : "));
    Serial.println(DEVICE_DESC);

    Serial.print(F("# REPORT CYCLE    : "));
    Serial.println(REPORT_CYCLE_TIME);

    Serial.print(F("# FUNC COUNT      : "));
    Serial.println(DEVICE_FUNC_COUNT);

    for (i = 0; i < DEVICE_FUNC_COUNT; i++)
    {
        Serial.print(F("# FUNC-"));
        Serial.print(i + 1);
        Serial.print(F(" NAME&ATTR: "));
        Serial.print(gFuncInfo[i].funcName);
        Serial.print(F(" ("));
        Serial.print(ATTR2STRING(gFuncInfo[i].funcAttr));
        Serial.println(F(")"));
    }

    Serial.print(F("# DELAY LOOP      : "));
    Serial.println(DELAY_LOOP);

    Serial.print(F("# USE MY LOOP     : "));
    Serial.println(BOOL2STRING(USE_MY_LOOP));

    Serial.print(F("# GRIB DEBUG      : "));
    Serial.println(BOOL2STRING(GRIB_DEBUG));

    memset(gSendBuff, '\0', sizeof(gSendBuff));
    getBleName(&pParam);
    Serial.print(F("# BLE MODULE NAME : "));
    Serial.println(gSendBuff);
    delay(200);

    if ( strncmp(pParam.sendMsg, DEVICE_NICK_NAME, strlen(DEVICE_NICK_NAME)) != 0)
    {
        memset(gSendBuff, '\0', sizeof(gSendBuff));
        memset(gRecvBuff, '\0', sizeof(gRecvBuff));
        snprintf(gRecvBuff, sizeof(gRecvBuff), "BLE+NAME=%s", DEVICE_NICK_NAME);
        Serial.print(F("# COMMAND MESSAGE : "));
        Serial.println(gRecvBuff);

        setBleName(&pParam);
        delay(200);//shbaek: Need More Than 200mSec
    }
    Serial.print(F("# BLE BAUD RATE   : "));
    Serial.println(btSerialSpeed);

    Serial.print(F("# BLE Tx/Rx PIN   : "));
    Serial.print(BT_TX);
    Serial.print("/");
    Serial.println(BT_RX);

    getBleAddr(gSendBuff);
    Serial.print(F("# BLE MODULE ADDR : "));
    Serial.println(gSendBuff);

    Serial.println(F("# ##### ##### ##### ##### ##### ##### ##### #####"));
    Serial.println();

    memset(gSendBuff, 0x00, sizeof(gSendBuff));
    memset(gRecvBuff, 0x00, sizeof(gRecvBuff));

    //shbaek: User Settings
    mySetup();
}

//# ########## ########## ########## ########## ########## ########## ########## ##########
//# shbaek: MAIN
//# ########## ########## ########## ########## ########## ########## ########## ##########
void loop()
{
    int i = 0;
    int iError = RES_ERROR;

#if USE_MY_LOOP //shbaek: User Run Loop
    myLoop();
#endif

    //# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
    //# shbaek: BLE Receive Message
    //# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
    if (btSerial.available())
    {
        GribFunc pHandler;
        GribFuncParam pParam;

        memset(gSendBuff, 0x00, sizeof(gSendBuff));
        memset(gRecvBuff, 0x00, sizeof(gRecvBuff));

        while (btSerial.available())
        {
            gRecvBuff[i] = char(btSerial.read());
            i++;
            //shbaek: If Receive Message Broken, Incread Delay Time
            delay(10);
        }

        Serial.println();
        if (gDebug) {
            Serial.print(F("# BT RECV LEN: "));
            Serial.println(strlen(gRecvBuff));
        }
        Serial.print(F("# BT RECV MSG: "));
        Serial.println(gRecvBuff);

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
        btSerial.write(pParam.sendMsg);

        Serial.print(F("# BT SEND MSG: "));
        Serial.println(pParam.sendMsg);

    }//shbaek: if (btSerial.available())

    //# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
    //# shbaek: Serial Menu[TBD]
    //# ---------- ---------- ---------- ---------- ---------- ---------- ---------- ----------
    if (Serial.available())
    {
        int iCount = 0;

        GribFunc pHandler;
        GribFuncParam pParam;

        memset(gSendBuff, 0x00, sizeof(gSendBuff));
        memset(gRecvBuff, 0x00, sizeof(gRecvBuff));

        iCount = Serial.readBytes(gRecvBuff, sizeof(gRecvBuff));
        gRecvBuff[iCount] = '\0';

        Serial.println();
        if (gDebug) {
            Serial.print(F("# SERIAL INPUT  LEN: "));
            Serial.println(iCount);
        }
        Serial.print("# SERIAL INPUT  MSG: ");
        Serial.println(gRecvBuff);

        pParam.sendMsg = gSendBuff;
        pParam.recvMsg = gRecvBuff;

        pHandler = findHandler(gRecvBuff);
        if (pHandler == NULL)
        {
            //shbaek: AT Command for BLE
            if ( strncasecmp(gRecvBuff, "AT", strlen("AT")) == 0)
            {
                if ( iCount < MAX_SIZE_BT_BUFF - 2)
                {
                    gRecvBuff[iCount + 0] = '\r';
                    gRecvBuff[iCount + 1] = '\n';
                    gRecvBuff[iCount + 2] = '\0';
                    myBleMsg(gRecvBuff, gSendBuff);
                }
            }
            else
            {
                //shbaek: I Don't Know...
                snprintf(pParam.sendMsg, MAX_SIZE_BT_BUFF, "%s:%d", RES2STRING(RES_ERROR), GRIB_ERROR_INVALID_COMMAND);
            }
        }
        else
        {
            iError = pHandler(&pParam);
        }
        Serial.print(F("# SERIAL OUTPUT MSG: "));
        Serial.println(pParam.sendMsg);
    }

    delay(DELAY_LOOP);
}//shbaek: void loop()

