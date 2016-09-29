/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "include/grib_ble.h"

using namespace std;
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
static struct hci_dev_info di; 
static volatile int signal_received = 0;

int gDebugBle = FALSE;
int gTombStone = FALSE;
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

static void sigint_handler(int sig)
{
	signal_received = sig;
}

static int read_flags(uint8_t *flags, const uint8_t *data, size_t size)
{
	size_t offset;

	if (!flags || !data)
		return -EINVAL;

	offset = 0;
	while (offset < size) {
		uint8_t len = data[offset];
		uint8_t type;

		/* Check if it is the end of the significant part */
		if (len == 0)
			break;

		if (len + offset > size)
			break;

		type = data[offset + 1];

		if (type == FLAGS_AD_TYPE) {
			*flags = data[offset + 2];
			return 0;
		}

		offset += 1 + len;
	}

	return -ENOENT;
}

static int check_report_filter(uint8_t procedure, le_advertising_info *info)
{
	uint8_t flags;

	/* If no discovery procedure is set, all reports are treat as valid */
	if (procedure == 0)
		return 1;

	/* Read flags AD type value from the advertising report if it exists */
	if (read_flags(&flags, info->data, info->length))
		return 0;

	switch (procedure) {
	case 'l': /* Limited Discovery Procedure */
		if (flags & FLAGS_LIMITED_MODE_BIT)
			return 1;
		break;
	case 'g': /* General Discovery Procedure */
		if (flags & (FLAGS_LIMITED_MODE_BIT | FLAGS_GENERAL_MODE_BIT))
			return 1;
		break;
	default:
		fprintf(stderr, "Unknown discovery procedure\n");
	}

	return 0;
}

static int eir_parse_name(uint8_t *eir, size_t eir_len, char *buf, size_t buf_len)
{
	size_t offset;

	offset = 0;
	while(offset < eir_len) 
	{
		uint8_t field_len = eir[0];
		size_t name_len;

		/* Check for the end of EIR */
		if (field_len == 0) break;
		if (offset + field_len > eir_len) goto failed;

		switch (eir[1]) 
		{
			case EIR_NAME_SHORT:
			case EIR_NAME_COMPLETE:
				name_len = field_len - 1;
				if (name_len > buf_len) goto failed;
	
				memcpy(buf, &eir[2], name_len);
				return 1;
		}

		offset += field_len + 1;
		eir += field_len + 1;	
	}

failed:
	snprintf(buf, buf_len, "(unknown)");
	return 0;
}



static int print_advertising_devices(int dd, uint8_t filter_type, int iScanCount)
{
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	struct sigaction sa;
	socklen_t olen;
	int len;
	int iCount = 0;
	int iMaxCount = 100;

#if (FEATURE_IGNORE_DUPLICATE_ADDR == ON)
	int idx = 0;
	int noGet = TRUE;
	bdaddr_t** ppAddrList;
#endif

	if(0 < iScanCount)
	{
		iMaxCount = iScanCount;
	}

	olen = sizeof(of);
	if (getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0) {
		printf("Could not get socket options\n");
		return -1;
	}

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if (setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0) {
		printf("Could not set socket options\n");
		return -1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

#if (FEATURE_IGNORE_DUPLICATE_ADDR == ON)
	ppAddrList = (bdaddr_t**) MALLOC(sizeof(bdaddr_t*) * iMaxCount);
	for(idx=0; idx<iMaxCount; idx++)
	{
		ppAddrList[idx] = (bdaddr_t*) MALLOC(sizeof(bdaddr_t));
		MEMSET(ppAddrList[idx], 0x00, sizeof(bdaddr_t));
	}
#endif

	while(iCount < iMaxCount)
	{
		evt_le_meta_event *meta;
		le_advertising_info *info;
		char addr[18];

		while ((len = read(dd, buf, sizeof(buf))) < 0) 
		{
			if (errno == EINTR && signal_received == SIGINT) 
			{
				len = 0;
				goto done;
			}

			if (errno == EAGAIN || errno == EINTR)
			{
				continue;
			}
			goto done;
		}

		ptr = buf + (1 + HCI_EVENT_HDR_SIZE);
		len -= (1 + HCI_EVENT_HDR_SIZE);

		meta = (evt_le_meta_event *) ptr;

		if (meta->subevent != 0x02)
			goto done;

		/* Ignoring multiple reports */
		info = (le_advertising_info *) (meta->data + 1);

#if (FEATURE_IGNORE_DUPLICATE_ADDR == ON)
		noGet = TRUE;
		for(idx=0; idx<iCount; idx++)
		{
			if(memcmp(ppAddrList[idx], &info->bdaddr, sizeof(bdaddr_t)) == 0)
			{//shbaek: Find Duplicated Item
				noGet = FALSE;
			}
		}

		if(noGet == TRUE)
		{//shbaek: Take Item
			memcpy(ppAddrList[iCount], &info->bdaddr, sizeof(bdaddr_t));
		}
		else
		{//shbaek: Throw Trash
			iCount++;
			continue;
		}
#endif
		if (check_report_filter(filter_type, info)) 
		{
			char name[30];

			memset(name, 0, sizeof(name));

			ba2str(&info->bdaddr, addr);
			if(eir_parse_name(info->data, info->length, name, sizeof(name) - 1))
			{
				printf("%s %s\n", addr, name);
				iCount++;
			}
		}
	}

done:
	setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));

#if (FEATURE_IGNORE_DUPLICATE_ADDR == ON)
	if(ppAddrList != NULL)
	{
		for(idx=0; idx<iMaxCount; idx++)
		{
			FREE(ppAddrList[idx]);
		}
		FREE(ppAddrList);
	}
#endif

	if (len < 0)
		return -1;

	return 0;
}

int Grib_BleConfig(void)
{
	int iRes = GRIB_ERROR;
	Grib_ConfigInfo pConfigInfo;

	MEMSET(&pConfigInfo, 0x00, sizeof(Grib_ConfigInfo));

	iRes = Grib_LoadConfig(&pConfigInfo);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("LOAD CONFIG ERROR !!!\n");
		return iRes;
	}

	gDebugBle  = pConfigInfo.debugBLE;
	gTombStone = pConfigInfo.bleTombStone;

	GRIB_LOGD("# BLE CONFIG DEBUG LOG: %d\n", gDebugBle);
	GRIB_LOGD("# BLE CONFIG TOMBSTONE: %d\n", gTombStone);

	return GRIB_SUCCESS;
}

void Grib_BleTombStone(Grib_BleLogInfo* pLogInfo)
{
	time_t sysTimer;
	struct tm *sysTime;

	sysTimer = time(NULL);
	sysTime  = localtime(&sysTimer);

	int   iFD = 0;
    char  pLogFilePath[SIZE_1K] = {'\0', };
	char* pMsg = NULL;

	if(gTombStone != TRUE)
	{//shbaek: Use Not TombStone
		return;
	}

    SNPRINTF(pLogFilePath, sizeof(pLogFilePath), "%s/%04d%02d%02d_%02d%02d%02d_%s.log", BLE_FILE_PATH_LOG_ROOT, 
												 sysTime->tm_year+1900, sysTime->tm_mon+1, sysTime->tm_mday,
												 sysTime->tm_hour, sysTime->tm_min, sysTime->tm_sec, pLogInfo->blePipe);

	GRIB_LOGD("# %s-TOMB: # STONE NAME: %s\n", pLogInfo->blePipe, pLogFilePath);

	iFD = open(pLogFilePath, O_WRONLY|O_CREAT, 0666);

	pMsg = "BLE NAME : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->blePipe, STRLEN(pLogInfo->blePipe));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "BLE ADDR : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleAddr, STRLEN(pLogInfo->bleAddr));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "REQ BUFF : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleSendMsg, STRLEN(pLogInfo->bleSendMsg));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "RES BUFF : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleRecvMsg, STRLEN(pLogInfo->bleRecvMsg));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	pMsg = "ERR CODE : ";
	write(iFD, pMsg, STRLEN(pMsg));
	write(iFD, pLogInfo->bleErrorMsg, STRLEN(pLogInfo->bleErrorMsg));
	write(iFD, GRIB_CRLN, STRLEN(GRIB_CRLN));

	close(iFD);
}

int Grib_BleCleanAll(void)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugBle;
	const char* FUNC_TAG = "BLE-CLEAN";

	char pFilePath[SIZE_1K] = {'\0', };

	DIR* pDirInfo = NULL;
	struct dirent* pDirEntry = NULL;
	struct stat fileStat;

	if(access(BLE_FILE_PATH_LOG_ROOT, F_OK) != 0)
	{
		GRIB_LOGD("# %s: CREATE LOG ROOT DIR\n", FUNC_TAG);

		mkdir(BLE_FILE_PATH_LOG_ROOT, 0755);
	}

	if(access(BLE_FILE_PATH_PIPE_ROOT, F_OK) != 0)
	{
		GRIB_LOGD("# %s: CREATE PIPE ROOT DIR\n", FUNC_TAG);
		mkdir(BLE_FILE_PATH_PIPE_ROOT, 0755);

		//shbaek: Need Not Clean.
		return GRIB_DONE;
	}

	pDirInfo = opendir(BLE_FILE_PATH_PIPE_ROOT);
	if(pDirInfo == NULL)
	{//shbaek: Aleady Exist
		GRIB_LOGD("# %s: OPEN PIPE ROOT DIR: %s\n", FUNC_TAG, BLE_FILE_PATH_PIPE_ROOT);
		return GRIB_ERROR;
	}

	while(pDirEntry=readdir(pDirInfo))
	{
		STRINIT(pFilePath, sizeof(pFilePath));
		SNPRINTF(pFilePath, sizeof(pFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pDirEntry->d_name);
		if(iDBG)GRIB_LOGD("# %s: DIR ENTRY NAME: %s\n", FUNC_TAG, pFilePath);

		unlink(pFilePath);
	}

FINAL:
	if(iDBG)GRIB_LOGD("# %s: FINAL LABEL: %p\n", FUNC_TAG, pDirInfo);
	if(pDirInfo != NULL)
	{
		closedir(pDirInfo);
	}
	
	return iRes;
}

int Grib_BlePipeReCreate(char *pipeFilePath)
{
	int iRes = GRIB_ERROR;
	int iDBG = gDebugBle;
	int iReTry = 0;
	const int MAX_RETRY_COUNT_CREATE_PIPE = 10;

	DIR* pDirInfo = NULL;
	struct dirent* pDirEntry = NULL;
	const char* FUNC_TAG = "BLE-PIPE";

	if(access(pipeFilePath, F_OK) == 0)
	{
		if(iDBG)GRIB_LOGD("# %s: DELETE OLD PIPE: %s\n", FUNC_TAG, pipeFilePath);
		unlink(pipeFilePath);
	}

	do
	{//3 shbaek: Give to Create Chance [MAX_RETRY_COUNT_CREATE_PIPE]
		iRes = mkfifo(pipeFilePath, 0666);
		if(iRes == GRIB_DONE)
		{//shbaek: Open Success
			break;
		}

		//shbaek: Must be Create Pipe File.
		iReTry++;
		GRIB_LOGD("# %s: CREATE PIPE(%s) FAIL: [RETRY: %d] [MSG: %s(%d)]\n", FUNC_TAG, pipeFilePath, iReTry, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		SLEEP(1);
	}while(iReTry < MAX_RETRY_COUNT_CREATE_PIPE);

	if(iDBG)GRIB_LOGD("# %s: CREATE NEW PIPE: %s\n", FUNC_TAG, pipeFilePath);

FINAL:
	if(iDBG)GRIB_LOGD("# %s: FINAL LABEL: %p\n", FUNC_TAG, pDirInfo);
	if(pDirInfo != NULL)
	{
		closedir(pDirInfo);
	}

	return iRes;
}

int Grib_BleSendMsg(char* deviceAddr, char *pipeFileName, char* sendBuff, char* recvBuff)
{
	int iRes = GRIB_DONE;
	int iDBG = gDebugBle;
	int iCount = GRIB_INIT;
	int iTotal = GRIB_INIT;
	int iStatus = GRIB_ERROR;
	int pipeFileFD = GRIB_ERROR;

	char pipeFilePath[SIZE_1K] = {NULL, };

	pid_t processID = GRIB_ERROR;

	const char* FUNC_TAG = "BLE-SEND";

	if(deviceAddr==NULL || sendBuff==NULL || recvBuff==NULL)
	{
		GRIB_LOGD("# %s: PARAM IS NULL\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	if(pipeFileName == NULL)
	{//shbaek: for TEST
		pipeFileName = TEST_BLE_PIPE_FILE_NAME;
	}

	if(iDBG)
	{
		GRIB_LOGD("# %s-SEND: DEVICE ADDR   : %s\n", pipeFileName, deviceAddr);
		GRIB_LOGD("# %s-SEND: PIPE FILE     : %s\n", pipeFileName, pipeFileName);
	}
	GRIB_LOGD("# %s-SEND: SEND MSG[%03d]: %s\n", pipeFileName, STRLEN(sendBuff), sendBuff);

	processID = fork();
	if(processID == GRIB_ERROR)
	{
		GRIB_LOGD("# %s-SEND: PROCESS FORK FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
		return GRIB_ERROR;
	}
	if(iDBG)GRIB_LOGD("# %s-SEND: FORK PROCESS ID: %d\n", pipeFileName, processID);

	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pipeFileName);
	if(iDBG)GRIB_LOGD("# %s-SEND: PIPE FILE PATH: %s\n", pipeFileName, pipeFilePath);

	switch(processID)
	{
		case 0:
		{
			char* pCMD = NULL;

#if (FEATURE_GRIB_BLE_EX==ON)
			//3 shbaek: Jump to Blecomm
			if(iDBG)GRIB_LOGD("# %s-SEND[CHILD]: JUMP BLE EXTEND\n", pipeFileName);
			iRes = execl(BLE_FILE_PATH_BLECOMM_PROGRAM, BLE_FILE_PATH_BLECOMM_PROGRAM, deviceAddr, pipeFilePath, sendBuff, NULL);
#else
			//3 shbaek: Jump to Python
			if(iDBG)GRIB_LOGD("# %s-SEND[CHILD]: JUMP BLE PYTHON\n", pipeFileName);
			iRes = execl(BLE_FILE_PATH_PYTHON_PROGRAM, BLE_FILE_PATH_PYTHON_PROGRAM, BLE_FILE_PATH_PYTHON_SCRIPT, deviceAddr, pipeFilePath, sendBuff, NULL);
#endif
			if(iDBG)GRIB_LOGD("# %s-SEND[CHILD]: DO YOU SEE ME???\n");
			exit(iRes);
			break;
		}

		default:
		{
			int iWaitTry = 0;
			const int WAIT_TRY_MAX = 5;
			const int WAIT_TRY_DELAY = 3;

			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: CREATE PIPE\n", pipeFileName);

			iRes = Grib_BlePipeReCreate(pipeFilePath);
			if(iRes == GRIB_FAIL)
			{
				GRIB_LOGD("# %s-SEND[PARENT]: CREATE PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: OPEN PIPE\n", pipeFileName);
			// Open PIPE File(Use Only Read Buffer)
			pipeFileFD = open(pipeFilePath, O_RDONLY);
			if(pipeFileFD < 0)
			{
				GRIB_LOGD("# %s-SEND[PARENT]: OPEN PIPE FAIL: %s[%d]\n", pipeFileName, LINUX_ERROR_STR, LINUX_ERROR_NUM);
				return GRIB_ERROR;
			}

			iTotal = iCount = GRIB_INIT;
			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: WAITING FOR CHILD MSG ...\n", pipeFileName);
			SLEEP(1);
			do
			{
				iCount = read(pipeFileFD, recvBuff+iTotal, BLE_MAX_SIZE_RECV_MSG-iTotal);
				if(iCount < 0)
				{
					GRIB_LOGD("# %s-SEND[PARENT]: READ FAIL: %s[%d]\n", LINUX_ERROR_STR, LINUX_ERROR_NUM);
					break;
				}
				else if(iCount == 0)
				{
					if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: READ DONE\n", pipeFileName);
					break;
				}
				else
				{
					iTotal += iCount;
					if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: READ:%d TOTAL:%d\n", pipeFileName, iCount, iTotal);
				}

			}while(iTotal < BLE_MAX_SIZE_RECV_MSG);
			//iRes = kill(processID, SIGKILL);
			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: WAITING FOR RETURN CHILD[PID:%d]\n", pipeFileName, processID);

			do{
				processID = wait(&iStatus);
				if(processID != GRIB_FAIL)
				{//shbaek: OK
					break;
				}

				if(WAIT_TRY_MAX < iWaitTry)
				{//shbaek: Occur Critical Error, Will Defunct
					GRIB_LOGD("# %s-SEND[PARENT]: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					GRIB_LOGD("# %s-SEND[PARENT]: #####       CRITICAL ERROR          #####\n", pipeFileName);
					GRIB_LOGD("# %s-SEND[PARENT]: ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);
					break;
				}

				iWaitTry++;
				GRIB_LOGD("# %s-SEND[PARENT]: WAIT FAIL RE-TRY: %d\n", pipeFileName, iWaitTry);
				SLEEP(WAIT_TRY_DELAY);
			}while(TRUE);

			if(iDBG)GRIB_LOGD("# %s-SEND[PARENT]: CHILD IS DONE PID:%d STATUS:%d\n", pipeFileName, processID, iStatus);

			if(0 < pipeFileFD)
			{
				close(pipeFileFD);
				pipeFileFD = NULL;
			}

			break;
		}
	}

	GRIB_LOGD("# %s-SEND: READ MSG[%03d]: %s\n", pipeFileName, STRLEN(recvBuff), recvBuff);

	if(STRNCASECMP(recvBuff, BLE_RESPONSE_STR_ERROR, STRLEN(BLE_RESPONSE_STR_ERROR)) == 0)
	{
		Grib_BleErrorCode iError = (Grib_BleErrorCode) ATOI(&STRCHR(recvBuff, GRIB_COLON)[1]);
		const char* pError = Grib_BleErrorToStr(iError);

		Grib_BleLogInfo bleLogInfo;
		MEMSET(&bleLogInfo, 0x00, sizeof(Grib_BleLogInfo));

		bleLogInfo.blePipe		= pipeFileName;
		bleLogInfo.bleAddr		= deviceAddr;
		bleLogInfo.bleSendMsg	= sendBuff;
		bleLogInfo.bleRecvMsg	= recvBuff;
		bleLogInfo.bleErrorMsg	= pError;

		GRIB_LOGD("# %s-SEND: # ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);

		Grib_BleTombStone(&bleLogInfo);

		GRIB_LOGD("# %s-SEND: # ERROR MSG : %s[%d]\n", pipeFileName, pError, iError);
		GRIB_LOGD("# %s-SEND: # ##### ##### ##### ##### ##### ##### #####\n", pipeFileName);

		if(iError == BLE_ERROR_CODE_CRITICAL)
		{//3 shbaek: HCI DRIVER RESET
			Grib_BleDetourInit();
		}

		iRes = GRIB_FAIL;
	}

	//shbaek: Need Interval Time, After Pipe Close.
	unlink(pipeFilePath);

	return iRes;
}

int Grib_BleDetourInit(void)
{//shbaek: Avoid Permission Error
	int   iRes = GRIB_ERROR;
	int   iSkipCount = 0;

	char* pCommand	= "sudo ./grib ble sinit";
	char  pLineBuffer[SIZE_1M] = {'\0', };

	iRes = systemCommand(pCommand, pLineBuffer, sizeof(pLineBuffer));
	GRIB_LOGD("# BLE DETOUR INIT RESULT[%d]:\n%s\n", STRLEN(pLineBuffer),pLineBuffer+iSkipCount);
	return iRes;
}

int Grib_BleDeviceInit(void)
{
	int dev_id, sock;
	int i,ctl, err;

	GRIB_LOGD("# BLE DEVICE INIT START\n");
	
	// Setting the stdout to line buffered, this forces a flush on every '\n' (newline). 
	// This will ensure that the python program consuming the output will not have any buffer issues. 
	setvbuf(stdout, (char *) NULL, _IOLBF, 0);
	
	// Opening a HCI socket 
	if ((ctl = socket(AF_BLUETOOTH, SOCK_RAW, BTPROTO_HCI)) < 0) 
	{
		perror("Can't open HCI socket.");
		return GRIB_ERROR;
	}

	// Connecting to the device and attempting to get the devices info, if this errors, 
	// it implies the usb is not plugged in correctly or we may have a bad/dodgy bluetooth usb 
	if (ioctl(ctl, HCIGETDEVINFO, (void *) &di)) 
	{
		perror("Can't get device info: Make sure the bluetooth usb is properly inserted. ");
		return GRIB_ERROR;
	}
	
	// Assigning the devices id 
	dev_id = di.dev_id;

	GRIB_LOGD("# HCI DEVICE DOWN\n");
	// Stop HCI device (e.g - bluetooth usb) - we are doing this to reset the adapter 
	if (ioctl(ctl, HCIDEVDOWN, dev_id) < 0)
	{
		fprintf(stderr, "Can't down device hci%d: %s (%d)\n", dev_id, strerror(errno), errno);
		return GRIB_ERROR;
	}

	GRIB_LOGD("# HCI DEVICE UP\n");
	// Start HCI device (e.g - bluetooth usb)
	if (ioctl(ctl, HCIDEVUP,dev_id) < 0)
	{
		if (errno == EALREADY) return 0;
		fprintf(stderr, "Can't init device hci%d: %s (%d)\n",dev_id, strerror(errno), errno);
		return GRIB_ERROR;
	}

	hci_close_dev(sock);
	GRIB_LOGD("# BLE DEVICE INIT DONE\n");
  	return 0;
}

int Grib_BleDeviceScan(int iScanCount)
{
	int dev_id, sock;
	int i, err;
	
	// Setting the stdout to line buffered, this forces a flush on every '\n' (newline). 
	// This will ensure that the python program consuming the output will not have any buffer issues. 
	setvbuf(stdout, (char *) NULL, _IOLBF, 0);

	Grib_BleDeviceInit();

	GRIB_LOGD("# BLE DEVICE SCAN START\n\n");

	// Assigning the devices id 
	dev_id = di.dev_id;

	// Opening the ble device adapter so we can start scanning for iBeacons  
	sock = hci_open_dev( dev_id );
	
	// Ensure that no error occured whilst opening the socket 
	if (dev_id < 0 || sock < 0)
	{
		perror("opening socket");
		return GRIB_ERROR;
	}
	
	// Setting the scan parameters 
	err = hci_le_set_scan_parameters(sock, 0x01, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 10000);
	if (err < 0) 
	{
		perror("Set scan parameters failed");
		return GRIB_ERROR;
	}
	
	// hci bluetooth library call - enabling the scan 
	err = hci_le_set_scan_enable(sock, 0x01, 0x00 , 10000);
	if (err < 0) 
	{
		perror("Enable scan failed");
		return GRIB_ERROR;
	}

	// Our own print funtion based off the bluez print function 
	err = print_advertising_devices(sock, 0, iScanCount);
	if (err < 0)
	{
		perror("Could not receive advertising events");
		return GRIB_ERROR;
	}

	// hci bluetooth library call - disabling the scan 
	err = hci_le_set_scan_enable(sock, 0x00, 0x01, 10000);
	if (err < 0)
	{
		perror("Disable scan failed");
		return GRIB_ERROR;
	}

	hci_close_dev(sock);
	GRIB_LOGD("# BLE DEVICE SCAN DONE\n");

  	return 0;
}

int Grib_BleDeviceInfo(Grib_DbRowDeviceInfo* pRowDeviceInfo)
{
	int   i = 0;
	int   iDBG = gDebugBle;
	int	  iRes = GRIB_ERROR;
	char  recvBuff[BLE_MAX_SIZE_RECV_MSG+1] = {'\0', };
	char* pSplitPoint = NULL;

	char* deviceID = NULL;
	char* deviceAddr = NULL;
	
	int    funcCount = 0;

	Grib_DbRowDeviceFunc* pRowDeviceFunc;

	deviceAddr = pRowDeviceInfo->deviceAddr;

	STRINIT(pRowDeviceInfo->deviceID, sizeof(pRowDeviceInfo->deviceID));
	iRes = Grib_BleGetDeviceID(deviceAddr, pRowDeviceInfo->deviceID);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET DEVICE ID FAIL: %s\n", pRowDeviceInfo->deviceID);
		return GRIB_FAIL;
	}
	if(STRLEN(pRowDeviceInfo->deviceID) < 3) 
	{
		GRIB_LOGD("# BLE-INFO: DEVICE ID TOO SHORT: %d\n", STRLEN(pRowDeviceInfo->deviceID));
		return GRIB_ERROR;
	}

	deviceID = pRowDeviceInfo->deviceID;
	pRowDeviceInfo->deviceInterface = DEVICE_IF_TYPE_BLE;

	STRINIT(pRowDeviceInfo->deviceLoc, sizeof(pRowDeviceInfo->deviceLoc));
	iRes = Grib_BleGetDeviceLoc(deviceAddr, deviceID, pRowDeviceInfo->deviceLoc);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET DEVICE LOC FAIL: %s\n", pRowDeviceInfo->deviceLoc);
		return GRIB_FAIL;
	}
	pRowDeviceInfo->deviceLoc;

	STRINIT(pRowDeviceInfo->deviceDesc, sizeof(pRowDeviceInfo->deviceDesc));
	iRes = Grib_BleGetDeviceDesc(deviceAddr, deviceID, pRowDeviceInfo->deviceDesc);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET DEVICE DESC FAIL: %s\n", pRowDeviceInfo->deviceDesc);
		return GRIB_FAIL;
	}
	pRowDeviceInfo->deviceDesc;

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetReportCycle(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET REPORT CYCLE FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}
	pRowDeviceInfo->reportCycle = ATOI(recvBuff);

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetFuncCount(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET FUNC COUNT FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}
	funcCount = pRowDeviceInfo->deviceFuncCount = ATOI(recvBuff);

	pRowDeviceInfo->ppRowDeviceFunc = (Grib_DbRowDeviceFunc**)MALLOC(funcCount*sizeof(Grib_DbRowDeviceFunc*));
	for(i=0; i<funcCount; i++)
	{
		pRowDeviceInfo->ppRowDeviceFunc[i] = (Grib_DbRowDeviceFunc*)MALLOC(sizeof(Grib_DbRowDeviceFunc));
	}

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetFuncName(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET FUNC NAME FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}

	for(i=0; i<funcCount; i++)
	{
		pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];
		pSplitPoint = Grib_Split(recvBuff, GRIB_COMMA, i);

		STRINIT(pRowDeviceFunc->funcName, sizeof(pRowDeviceFunc->funcName));
		STRNCPY(pRowDeviceFunc->funcName, pSplitPoint, STRLEN(pSplitPoint));
	}

	STRINIT(recvBuff, sizeof(recvBuff));
	iRes = Grib_BleGetFuncAttr(deviceAddr, deviceID, recvBuff);
	if(iRes != GRIB_DONE)
	{
		GRIB_LOGD("# BLE-INFO: GET FUNC ATTR FAIL: %s\n", recvBuff);
		return GRIB_FAIL;
	}

	for(i=0; i<funcCount; i++)
	{
		pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];
		pSplitPoint = Grib_Split(recvBuff, GRIB_COMMA, i);

		pRowDeviceFunc->funcAttr = ATOI(pSplitPoint);
	}
	GRIB_LOGD("\n");
	GRIB_LOGD("# ########## ########## ########## ########## ########## ##########\n");
	GRIB_LOGD("# DEVICE ID     : %s\n", pRowDeviceInfo->deviceID);
	GRIB_LOGD("# DEVICE ADDR   : %s\n", pRowDeviceInfo->deviceAddr);
	GRIB_LOGD("# DEVICE LOC    : %s\n", pRowDeviceInfo->deviceLoc);
	GRIB_LOGD("# DEVICE DESC   : %s\n", pRowDeviceInfo->deviceDesc);
	GRIB_LOGD("# REPORT CYCLE  : %d\n", pRowDeviceInfo->reportCycle);
	GRIB_LOGD("# FUNC COUNT    : %d\n", pRowDeviceInfo->deviceFuncCount);
	for(i=0; i<funcCount; i++)
	{
		pRowDeviceFunc = pRowDeviceInfo->ppRowDeviceFunc[i];

		GRIB_LOGD("# FUNC[%d/%d] NAME: %s\n", i+1, funcCount, pRowDeviceFunc->funcName);
		GRIB_LOGD("# FUNC[%d/%d] ATTR: %d [%s]\n", i+1, funcCount, pRowDeviceFunc->funcAttr, Grib_FuncAttrToStr(pRowDeviceFunc->funcAttr));
	}
	GRIB_LOGD("# ########## ########## ########## ########## ########## ##########\n");
	GRIB_LOGD("\n");

	return GRIB_DONE;

}


#define __GRIB_BLE_API__
int Grib_BleGetDeviceID(char* deviceAddr, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pTemp = NULL;
	char* pipeFileName = NULL;
	char pipeFilePath[SIZE_1K] = {NULL, };

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_ID, STRLEN(BLE_CMD_GET_DEVICE_ID));

	pipeFileName = STRDUP(deviceAddr);

	while( (pTemp=STRCHR(pipeFileName, ':')) != NULL )
	{//shbaek: ':' -> '_'
		*pTemp = '_';
	}

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	STRINIT(pipeFilePath, sizeof(pipeFilePath));
	SNPRINTF(pipeFilePath, sizeof(pipeFilePath), "%s/%s", BLE_FILE_PATH_PIPE_ROOT, pipeFileName);

	unlink(pipeFilePath);
	FREE(pipeFileName);

	return iRes;
}

int Grib_BleSetDeviceID(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_SET_DEVICE_ID, deviceID);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetDeviceIF(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_IF, STRLEN(BLE_CMD_GET_DEVICE_IF));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}


int Grib_BleGetDeviceLoc(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_LOC, STRLEN(BLE_CMD_GET_DEVICE_LOC));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleSetDeviceLoc(char* deviceAddr, char* deviceID, char* deviceLoc, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_SET_DEVICE_LOC, deviceLoc);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetDeviceDesc(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_DEVICE_DESC, STRLEN(BLE_CMD_GET_DEVICE_DESC));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncCount(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_FUNC_COUNT, STRLEN(BLE_CMD_GET_FUNC_COUNT));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncName(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_FUNC_NAME, STRLEN(BLE_CMD_GET_FUNC_NAME));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncAttr(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_FUNC_ATTR, STRLEN(BLE_CMD_GET_FUNC_ATTR));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetReportCycle(char* deviceAddr, char* deviceID, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	STRNCPY(sendBuff, BLE_CMD_GET_REPORT_CYCLE, STRLEN(BLE_CMD_GET_REPORT_CYCLE));

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleGetFuncData(char* deviceAddr, char* deviceID, char* funcName, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_GET_FUNC_DATA, funcName);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

int Grib_BleSetFuncData(char* deviceAddr, char* deviceID, char* funcName, char* content, char* recvBuff)
{
	int   iRes = GRIB_ERROR;
	char  sendBuff[BLE_MAX_SIZE_SEND_MSG+1] = {'\0', };
	char* pipeFileName = deviceID;

	STRINIT(sendBuff, sizeof(sendBuff));
	SNPRINTF(sendBuff, sizeof(sendBuff), BLE_CMD_SET_FUNC_DATA, funcName, content);

	iRes = Grib_BleSendMsg(deviceAddr, pipeFileName, sendBuff, recvBuff);

	return iRes;
}

