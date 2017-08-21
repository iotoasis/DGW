/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_hci.h"

using namespace std;
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
static struct hci_dev_info di; 
static volatile int signal_received = 0;

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

Grib_SqlInfo gHciSql;
Grib_SqlInfo* Grib_GetHciSql(void)
{
	const char* FUNC_TAG = "HCI-SQL";

	int iRes = GRIB_ERROR;
	Grib_SqlInfo* pHciSql = &gHciSql;

	Grib_FreeSqlInfo(pHciSql);

	iRes = Grib_MakeSqlInfo(pHciSql);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: MAKE SQL FAIL !!!\n", FUNC_TAG);
		return NULL;
	}

	iRes = Grib_DbConnect(pHciSql);
	if(iRes!=GRIB_DONE)
	{
		GRIB_LOGD("# %s: DB CONNECT FAIL !!!\n", FUNC_TAG);
		return NULL;
	}

	return pHciSql;
}


int Grib_ScanDeviceToDb(Grib_ScanDevInfo* pScanDevInfo)
{
	const char* FUNC_TAG = "HCI-INSERT";

	int i = 0;
	int iCount = 0;
	int iRes = GRIB_ERROR;
	int iDBG = FALSE;

	Grib_SqlInfo* pHciSql = NULL;

	char sqlQuery[MYSQL_MAX_SIZE_QUERY+1] = {'\0', };

	if(pScanDevInfo == NULL)
	{
		GRIB_LOGD("# %s: PARAM NULL ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	pHciSql = Grib_GetHciSql();
	if(pHciSql == NULL)
	{
		GRIB_LOGD("# %s: GET HCI SQL ERROR !!!\n", FUNC_TAG);
		return GRIB_ERROR;
	}

	//2 shbaek: INSERT DATA
	STRINIT(sqlQuery, sizeof(sqlQuery));
	SPRINTF(sqlQuery, QUERY_INSERT_SCAN_DEVICE, pScanDevInfo->addr, pScanDevInfo->name,
								pScanDevInfo->type, pScanDevInfo->memo);

	iRes = Grib_DbQuery(pHciSql, sqlQuery);
	if(iRes!=GRIB_DONE)
	{
		if(iDBG)GRIB_LOGD("# %s: QUERY FAIL: %s [%d]\n", FUNC_TAG, pHciSql->errStr, pHciSql->errNum);
		return GRIB_ERROR;
	}

	return iRes;
}


int Grib_HciInit(void)
{
	int dev_id = 0;
	int sock = 0;
	int ctl = 0;

	GRIB_LOGD("# HCI DEVICE INIT START\n");
	
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
	GRIB_LOGD("# HCI DEVICE INIT DONE\n");
  	return 0;
}

static int Grib_PrintDevice(int dd, uint8_t filter_type, int maxScanCount, int ignoreDulicate)
{
	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	struct sigaction sa;
	socklen_t olen;
	int len;
	int iCount = 0;

	int idx = 0;
	int noGet = TRUE;
	bdaddr_t** ppAddrList;
	Grib_ScanDevInfo scanDevInfo;

	olen = sizeof(of);
	if(getsockopt(dd, SOL_HCI, HCI_FILTER, &of, &olen) < 0)
	{
		printf("Could not get socket options\n");
		return -1;
	}

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if(setsockopt(dd, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
	{
		printf("Could not set socket options\n");
		return -1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	if(ignoreDulicate)
	{
		ppAddrList = (bdaddr_t**) MALLOC(sizeof(bdaddr_t*) * maxScanCount);
		for(idx=0; idx<maxScanCount; idx++)
		{
			ppAddrList[idx] = (bdaddr_t*) MALLOC(sizeof(bdaddr_t));
			MEMSET(ppAddrList[idx], 0x00, sizeof(bdaddr_t));
		}
	}

	while(iCount < maxScanCount)
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

		if(ignoreDulicate)
		{
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
		}
		if (check_report_filter(filter_type, info)) 
		{
			char name[HCI_DEVICE_NAME_MAX_SIZE];

			memset(name, 0, sizeof(name));

			ba2str(&info->bdaddr, addr);
			if(eir_parse_name(info->data, info->length, name, sizeof(name) - 1))
			{
				printf("%s %s\n", addr, name);

				memset(&scanDevInfo, 0x00, sizeof(scanDevInfo));

				scanDevInfo.index = iCount;
				STRNCPY(scanDevInfo.addr, addr, STRLEN(addr));
				STRNCPY(scanDevInfo.name, name, STRLEN(name));
				scanDevInfo.type = info->bdaddr_type;

				Grib_ScanDeviceToDb(&scanDevInfo);

				iCount++;
			}
		}
	}

done:
	setsockopt(dd, SOL_HCI, HCI_FILTER, &of, sizeof(of));

	if(ignoreDulicate)
	{
		if(ppAddrList != NULL)
		{
			for(idx=0; idx<maxScanCount; idx++)
			{
				FREE(ppAddrList[idx]);
			}
			FREE(ppAddrList);
		}
	}

	if (len < 0)
		return -1;

	return 0;
}


int Grib_HciScan(int scanCount, int ignoreDuplicate)
{
	int dev_id = 0;
	int sock = 0;
	int err = 0;

	// Setting the stdout to line buffered, this forces a flush on every '\n' (newline). 
	// This will ensure that the python program consuming the output will not have any buffer issues. 
	setvbuf(stdout, (char *) NULL, _IOLBF, 0);

	Grib_HciInit();

	GRIB_LOGD("# HCI DEVICE SCAN START\n");

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

	GRIB_LOGD("\n# ##### ##### ##### ##### ##### ##### ##### ##### ##### #####\n");

	// Our own print funtion based off the bluez print function 
	err = Grib_PrintDevice(sock, 0, scanCount, ignoreDuplicate);
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
	GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### ##### ##### #####\n\n");
	GRIB_LOGD("# HCI DEVICE SCAN DONE\n");

  	return 0;
}

static int Grib_PrintAdv(int sock, const char* addr, int opt)
{
	const char* FUNC = "PRINT-ADV";

	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	struct sigaction sa;
	socklen_t olen;
	int len;

	int iCount = 0;
	const int LOOP_COUNT = INT_MAX;
	bdaddr_t targetAddr;

	if(STRLEN(addr) != GRIB_MAX_SIZE_BLE_ADDR_STR)
	{
		GRIB_LOGD("# %s: TARGET ADDR INVALID SIZE: %d\n", FUNC, STRLEN(addr));
		return -1;
	}

	if(str2ba(addr, &targetAddr))
	{
		GRIB_LOGD("# %s: TARGET ADDR INVALID DATA: %s\n", FUNC, addr);
		return -1;
	}

	olen = sizeof(of);
	if(getsockopt(sock, SOL_HCI, HCI_FILTER, &of, &olen) < 0)
	{
		printf("# %s: Could not get socket options\n", FUNC);
		return -1;
	}

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if(setsockopt(sock, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
	{
		printf("# %s: Could not set socket options\n", FUNC);
		return -1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	while(iCount < LOOP_COUNT)
	{
		char tempAddr[GRIB_MAX_SIZE_BLE_ADDR_STR+1] = {'\0', };
		char tempName[HCI_DEVICE_NAME_MAX_SIZE] = {'\0', };

		evt_le_meta_event*   meta;
		le_advertising_info* info;

		MEMSET(tempAddr, 0, sizeof(tempAddr));
		MEMSET(tempName, 0, sizeof(tempName));

		while((len = read(sock, buf, sizeof(buf))) < 0) 
		{//shbaek: Error Case
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

		if (meta->subevent != 0x02)//shbaek: Sub Event: LE Advertising Report (0x02)
			goto done;

		info = (le_advertising_info *) (meta->data + 1);

		if(bacmp(&targetAddr, &info->bdaddr) != 0)
		{//shbaek: No Target ...
			continue;
		}

		ba2str(&info->bdaddr, tempAddr);

		GRIB_LOGD("\n");
		Grib_ShowCurrDateTime();
		GRIB_LOGD(GRIB_1LINE_DASH);
/*

		GRIB_LOGD("# [EVT: 0x%02X] [PEER: 0x%02X] [LEN: %d]\n", info->evt_type, info->bdaddr_type, info->length);
		if(eir_parse_name(info->data, info->length, tempName, sizeof(tempName) - 1))
		{
			GRIB_LOGD("# NAME: %s\n", tempName);
		}
		else
*/
		{
			Grib_PrintOnlyHex((char *)ptr, len);
		}
		iCount++;
	}

done:
	setsockopt(sock, SOL_HCI, HCI_FILTER, &of, sizeof(of));

	if (len < 0)
		return -1;

	return 0;
}

static int Grib_PrintAdvOri(int sock, const char* addr, int opt)
{
	const char* FUNC = "PRINT-ADV";

	unsigned char buf[HCI_MAX_EVENT_SIZE], *ptr;
	struct hci_filter nf, of;
	struct sigaction sa;
	socklen_t olen;
	int len;

	int iCount = 0;
	const int LOOP_COUNT = INT_MAX;
	bdaddr_t targetAddr;

	if(STRLEN(addr) != GRIB_MAX_SIZE_BLE_ADDR_STR)
	{
		GRIB_LOGD("# %s: TARGET ADDR INVALID SIZE: %d\n", FUNC, STRLEN(addr));
		return -1;
	}

	if(str2ba(addr, &targetAddr))
	{
		GRIB_LOGD("# %s: TARGET ADDR INVALID DATA: %s\n", FUNC, addr);
		return -1;
	}

	olen = sizeof(of);
	if(getsockopt(sock, SOL_HCI, HCI_FILTER, &of, &olen) < 0)
	{
		printf("# %s: Could not get socket options\n", FUNC);
		return -1;
	}

	hci_filter_clear(&nf);
	hci_filter_set_ptype(HCI_EVENT_PKT, &nf);
	hci_filter_set_event(EVT_LE_META_EVENT, &nf);

	if(setsockopt(sock, SOL_HCI, HCI_FILTER, &nf, sizeof(nf)) < 0)
	{
		printf("# %s: Could not set socket options\n", FUNC);
		return -1;
	}

	memset(&sa, 0, sizeof(sa));
	sa.sa_flags = SA_NOCLDSTOP;
	sa.sa_handler = sigint_handler;
	sigaction(SIGINT, &sa, NULL);

	while(iCount < LOOP_COUNT)
	{
		char tempAddr[GRIB_MAX_SIZE_BLE_ADDR_STR+1] = {'\0', };
		char tempName[HCI_DEVICE_NAME_MAX_SIZE] = {'\0', };

		evt_le_meta_event*   meta;
		le_advertising_info* info;

		MEMSET(tempAddr, 0, sizeof(tempAddr));
		MEMSET(tempName, 0, sizeof(tempName));

		while((len = read(sock, buf, sizeof(buf))) < 0) 
		{//shbaek: Error Case
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

		if (meta->subevent != 0x02)//shbaek: Sub Event: LE Advertising Report (0x02)
			goto done;

		info = (le_advertising_info *) (meta->data + 1);

		if(bacmp(&targetAddr, &info->bdaddr) != 0)
		{//shbaek: No Target ...
			continue;
		}

		ba2str(&info->bdaddr, tempAddr);

		GRIB_LOGD("\n");
		Grib_ShowCurrDateTime();
		GRIB_LOGD(GRIB_1LINE_DASH);
		GRIB_LOGD("# [EVT: 0x%02X] [PEER: 0x%02X] [LEN: %d]\n", info->evt_type, info->bdaddr_type, info->length);

/*
		if(eir_parse_name(info->data, info->length, tempName, sizeof(tempName) - 1))
		{
			GRIB_LOGD("# NAME: %s\n", tempName);
		}
		else
*/
		{
			Grib_PrintOnlyHex((char *)info->data, info->length);
		}
		iCount++;
	}

done:
	setsockopt(sock, SOL_HCI, HCI_FILTER, &of, sizeof(of));

	if (len < 0)
		return -1;

	return 0;
}


int Grib_HciAdv(char* addr, int opt)
{
	const char* FUNC = "HCI-ADV";

	int dev_id = 0;
	int sock = 0;
	int err = 0;

	// Setting the stdout to line buffered, this forces a flush on every '\n' (newline). 
	// This will ensure that the python program consuming the output will not have any buffer issues. 
	setvbuf(stdout, (char *) NULL, _IOLBF, 0);

	Grib_HciInit();

	GRIB_LOGD("# %s: START -> %s\n", FUNC, addr);

	// Assigning the devices id 
	dev_id = di.dev_id;

	// Opening the ble device adapter so we can start scanning for iBeacons  
	sock = hci_open_dev( dev_id );

	// Ensure that no error occured whilst opening the socket 
	if (dev_id < 0 || sock < 0)
	{
		GRIB_LOGD("# %s: opening socket", FUNC);
		return GRIB_ERROR;
	}
	
	// Setting the scan parameters 
	err = hci_le_set_scan_parameters(sock, 0x01, htobs(0x0010), htobs(0x0010), 0x00, 0x00, 10000);
	if (err < 0) 
	{
		GRIB_LOGD("# %s: Set scan parameters failed", FUNC);
		return GRIB_ERROR;
	}
	
	// hci bluetooth library call - enabling the scan 
	err = hci_le_set_scan_enable(sock, 0x01, 0x00 , 10000);
	if (err < 0) 
	{
		GRIB_LOGD("# %s: Enable scan failed", FUNC);
		return GRIB_ERROR;
	}

	GRIB_LOGD("\n# ##### ##### ##### ##### ##### ##### ##### ##### ##### #####\n");

	// Our own print funtion based off the bluez print function 
	err = Grib_PrintAdv(sock, addr, opt);
	if (err < 0)
	{
		GRIB_LOGD("# %s: Could not receive advertising events", FUNC);
		return GRIB_ERROR;
	}

	// hci bluetooth library call - disabling the scan 
	err = hci_le_set_scan_enable(sock, 0x00, 0x01, 10000);
	if (err < 0)
	{
		GRIB_LOGD("# %s: Disable scan failed", FUNC);
		return GRIB_ERROR;
	}

	hci_close_dev(sock);
	GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### ##### ##### #####\n\n");
	GRIB_LOGD("# HCI DEVICE SCAN DONE\n");

  	return 0;
}

int main(int argc, char **argv)
{
	char* mainMenu = NULL;
	char* subMenu = NULL;

	//for(int i=0; i<argc; i++)GRIB_LOGD("# ARG[%d/%d]: %s\n", i, argc, argv[i]);

	if(GRIB_CMD_MAIN < argc)
	{
		mainMenu = GRIB_CMD_MAIN[argv];
	}
	else
	{
		GRIB_LOGD("# INIT? or SCAN?\n");
		return GRIB_ERROR;
	}

	if(STRCASECMP(mainMenu, "init") == 0)
	{
		Grib_HciInit();
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "scan") == 0)
	{
		int scanCount = HCI_DEFAULT_SCAN_COUNT;
		int ignoreDuplicate = TRUE;

		do{
			if(GRIB_CMD_SUB < argc)
			{
				int iCount = 0;

				subMenu = GRIB_CMD_SUB[argv];

				if(STRCASECMP(subMenu, "all") == 0)
				{
					ignoreDuplicate = FALSE;
					scanCount = INT_MAX;
					break;
				}

				iCount = ATOI(subMenu);
				if(0 < iCount)
				{
					scanCount = iCount;
					break;
				}
			}
		}while(FALSE);

		Grib_HciScan(scanCount, ignoreDuplicate);
		return GRIB_DONE;
	}
	if(STRCASECMP(mainMenu, "adv") == 0)
	{
		char* addr = NULL;
		int   opt  = GRIB_NOT_USED;

		if(0<STRLEN(GRIB_CMD_SUB[argv]))
		{
			addr = GRIB_CMD_SUB[argv];

			if(0<STRLEN(GRIB_CMD_ARG1[argv]))opt = ATOI(GRIB_CMD_ARG1[argv]);
			
			Grib_HciAdv(addr, opt);
		}
		else
		{
			GRIB_LOGD("# NEED TARGET ADDRESS !!!\n");
		}
		return GRIB_DONE;

	}

	GRIB_LOGD("# INVALID MENU\n");
	GRIB_LOGD("\n");

	return GRIB_ERROR;
}

