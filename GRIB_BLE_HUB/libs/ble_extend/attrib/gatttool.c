/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#ifdef HAVE_CONFIG_H

#include <config.h>

#endif

#include <errno.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <btio/btio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#include "lib/uuid.h"
#include "att.h"
#include "gattrib.h"
#include "gatt.h"
#include "gatttool.h"


#include "grib_ble_extend.h"


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
gchar *opt_src = NULL;
gchar *opt_dst = NULL;
gchar *opt_dst_type = NULL;
gchar *opt_value = NULL;
gchar *opt_sec_level = NULL;
bt_uuid_t *opt_uuid = NULL;
int opt_start = 0x0001;
int opt_end = 0xffff;
int opt_handle = -1;
int opt_mtu = 0;
int opt_psm = 0;
gboolean opt_primary = FALSE;
gboolean opt_characteristics = FALSE;
gboolean opt_char_read = FALSE;
gboolean opt_listen = FALSE;
gboolean opt_char_desc = FALSE;
gboolean opt_char_write = FALSE;
gboolean opt_char_write_req = FALSE;
gboolean opt_interactive = FALSE;
GMainLoop *event_loop;
gboolean got_error = FALSE;
GSourceFunc operation;


GOptionEntry primary_char_options[] = {
	{ "start", 's' , 0, G_OPTION_ARG_INT, &opt_start,
		"Starting handle(optional)", "0x0001" },
	{ "end", 'e' , 0, G_OPTION_ARG_INT, &opt_end,
		"Ending handle(optional)", "0xffff" },
	{ NULL },
};

GOptionEntry char_rw_options[] = {
	{ "handle", 'a' , 0, G_OPTION_ARG_INT, &opt_handle,
		"Read/Write characteristic by handle(required)", "0x0001" },
	{ "value", 'n' , 0, G_OPTION_ARG_STRING, &opt_value,
		"Write characteristic value (required for write operation)",
		"0x0001" },
	{NULL},
};

GOptionEntry gatt_options[] = {
	{ "primary", 0, 0, G_OPTION_ARG_NONE, &opt_primary,
		"Primary Service Discovery", NULL },
	{ "characteristics", 0, 0, G_OPTION_ARG_NONE, &opt_characteristics,
		"Characteristics Discovery", NULL },
	{ "char-read", 0, 0, G_OPTION_ARG_NONE, &opt_char_read,
		"Characteristics Value/Descriptor Read", NULL },
	{ "char-write", 0, 0, G_OPTION_ARG_NONE, &opt_char_write,
		"Characteristics Value Write Without Response (Write Command)",
		NULL },
	{ "char-write-req", 0, 0, G_OPTION_ARG_NONE, &opt_char_write_req,
		"Characteristics Value Write (Write Request)", NULL },
	{ "char-desc", 0, 0, G_OPTION_ARG_NONE, &opt_char_desc,
		"Characteristics Descriptor Discovery", NULL },
	{ "listen", 0, 0, G_OPTION_ARG_NONE, &opt_listen,
		"Listen for notifications and indications", NULL },
	{ "interactive", 'I', G_OPTION_FLAG_IN_MAIN, G_OPTION_ARG_NONE,
		&opt_interactive, "Use interactive mode", NULL },
	{ NULL },
};

GOptionEntry options[] = {
	{ "adapter", 'i', 0, G_OPTION_ARG_STRING, &opt_src,
		"Specify local adapter interface", "hciX" },
	{ "device", 'b', 0, G_OPTION_ARG_STRING, &opt_dst,
		"Specify remote Bluetooth address", "MAC" },
	{ "addr-type", 't', 0, G_OPTION_ARG_STRING, &opt_dst_type,
		"Set LE address type. Default: public", "[public | random]"},
	{ "mtu", 'm', 0, G_OPTION_ARG_INT, &opt_mtu,
		"Specify the MTU size", "MTU" },
	{ "psm", 'p', 0, G_OPTION_ARG_INT, &opt_psm,
		"Specify the PSM for GATT/ATT over BR/EDR", "PSM" },
	{ "sec-level", 'l', 0, G_OPTION_ARG_STRING, &opt_sec_level,
		"Set security level. Default: low", "[low | medium | high]"},
	{ NULL },
};

//char	addr[BLE_EX_SIZE_ADDR];
//char	pipe_path[BLE_EX_SIZE_PIPE_PATH];
//char	req_buf[BLE_EX_SIZE_SEND_MSG];
//char 	res_msg[BLE_EX_SIZE_RECV_MSG];
char	hex_buf[BLE_EX_SIZE_SEND_MSG*2];
int 	res_pt;

int   	gDBG = FALSE;
GError* gLibError;
Grib_BleErrorCode gErrorCode = BLE_ERROR_CODE_INTERNAL;
	
BleCommArg gBlecommArg;

static GAttrib* 	gAttrib;
static GIOChannel*	gIOChannel;

static Grib_BleStatus gBleStatus;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#if __NOT_USED__
timer_t gTimer;
struct itimerspec value;
struct sigevent av_sig_spec;

int Grib_SimpleTimerCb(GribTimerParam* pParam)
{

}

void Grib_StartTimer(timer_t* timerID, GribFunc pFunc, long nSecInterval)
{

	av_sig_spec.sigev_notify = SIGEV_SIGNAL;
	av_sig_spec.sigev_signo = SIGRTMIN;

	value.it_value.tv_sec = 0;
	value.it_value.tv_nsec = nSecInterval;

	value.it_interval.tv_sec = 0;
	value.it_interval.tv_nsec = nSecInterval;

	timer_create(CLOCK_REALTIME, &av_sig_spec, timerID);
	timer_settime(*timerID, 0, &value, NULL);
	signal(SIGRTMIN, pFunc);

	return;
}
#endif

int Grib_BleCmdEnd(int iErrorCode)
{
	if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] [eCODE: %d]\n", gBlecommArg.pPipePath, __FUNC_NAME__, iErrorCode);

	if(gBlecommArg.pipeWrite == TRUE)
	{
		g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] ALREADY DONE\n", gBlecommArg.pPipePath, __FUNC_NAME__);
		return 0;
	}

	if(gAttrib != NULL)
	{
		if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] ATTRIB FREE \n", gBlecommArg.pPipePath, __FUNC_NAME__);
		g_attrib_unref(gAttrib);
		opt_mtu = 0;
		gAttrib = NULL;
	}

	return Grib_BleCommLoopQuit(event_loop, gLibError, iErrorCode);
}

static gpointer Grib_ChannelWatcher(GIOChannel *chan, GIOCondition cond, gpointer user_data)
{
	if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] [STATUS: %d] \n", gBlecommArg.pPipePath, __FUNC_NAME__, gBleStatus);

	if(gIOChannel != NULL)
	{
		g_io_channel_shutdown(gIOChannel, FALSE, NULL);
		g_io_channel_unref(gIOChannel);
		gIOChannel = NULL;
	}
	Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);

	return user_data;
}

void Grib_NotifyCb(const guint8 *pdu, guint16 len, gpointer user_data)
{
	uint8_t opCode = 0;

	opCode = pdu[0];
	g_print("# BLE-EX: [PIPE: %s] [FUNC: %s] [CODE: 0x%X]\n", gBlecommArg.pPipePath, __FUNC_NAME__, opCode);

	if( (gBleStatus==BLE_STATUS_DONE) || (gBleStatus==BLE_STATUS_DISCONNECT) )
	{
		g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] BLE_STATUS_DONE \n", gBlecommArg.pPipePath, __FUNC_NAME__);

		Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);
	}
	else
	{
		if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] [STATUS: %d] \n", gBlecommArg.pPipePath, __FUNC_NAME__, gBleStatus);
	}

	return ;
}

void Grib_ResultCb(guint8 status, const guint8 *pdu, guint16 len, gpointer user_data)
{
	if( (gBleStatus==BLE_STATUS_DONE) || (gBleStatus==BLE_STATUS_DISCONNECT) )
	{
		g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] BLE_STATUS_DONE \n", gBlecommArg.pPipePath, __FUNC_NAME__);

		gBleStatus = BLE_STATUS_DISCONNECT;

		Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);
	}
	else
	{
		if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] [STATUS: %d] \n", gBlecommArg.pPipePath, __FUNC_NAME__, gBleStatus);
	}

	return ;
}

void Grib_DestoryCb(gpointer user_data)
{
	if( (gBleStatus==BLE_STATUS_DONE) || (gBleStatus==BLE_STATUS_DISCONNECT) )
	{
		g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] BLE_STATUS_DONE \n", gBlecommArg.pPipePath, __FUNC_NAME__);

		gBleStatus = BLE_STATUS_DISCONNECT;

		Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);
	}
	else
	{
		if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] [STATUS: %d] \n", gBlecommArg.pPipePath, __FUNC_NAME__, gBleStatus);
	}

	return ;
}

void events_handler(const uint8_t *pdu, uint16_t len, gpointer user_data)
{
	GAttrib *attrib = user_data;
	uint8_t *opdu;
	uint8_t opCode = ATT_OP_ERROR;
	uint16_t handle, i, olen = 0;
	size_t plen;

	opCode = pdu[0];
	handle = att_get_u16(&pdu[1]);

	strncat(gBlecommArg.pRecvBuff+res_pt, (const char*)(pdu+BLE_HEADER_SIZE), len-BLE_HEADER_SIZE);
	res_pt = res_pt + len - BLE_HEADER_SIZE;

	gBleStatus = BLE_STATUS_OPERATE;

	if(gDBG)g_print("# BLE-EX: [PIPE: %s] [FUNC: %s] [oCODE: 0x%X]\n", gBlecommArg.pPipePath, __FUNC_NAME__, opCode);

	if( (pdu[len-1]==BLE_MSG_END_SYMBOL) || (len<BLE_HEADER_SIZE+BLE_MSG_MAX_SIZE) )
	{//shbaek: Message End
		gBleStatus = BLE_STATUS_DONE;

		gBlecommArg.pRecvBuff[res_pt-1] = '\0';
		Grib_BleCmdEnd(BLE_ERROR_CODE_NONE);

		return;
	}
	else if(sizeof(gBlecommArg.pRecvBuff) <= res_pt)
	{//shbaek: Invalid Length
		gBleStatus = BLE_STATUS_DONE;

		Grib_BleCmdEnd(BLE_ERROR_CODE_RECV_FAIL);
		return;
	}

	if(opCode == ATT_OP_HANDLE_NOTIFY)
	{
		return;
	}
	else if(opCode == ATT_OP_ERROR)
	{
		gBleStatus = BLE_STATUS_DONE;

		Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);
		return;
	}

	opdu = g_attrib_get_buffer(attrib, &plen);
	olen = enc_confirmation(opdu, plen);
	if(gDBG)g_print("# BLE-EX: [PIPE: %s] [FUNC: %s] [oLEN: %d]\n", gBlecommArg.pPipePath, __FUNC_NAME__, olen);

	if (olen > 0)
		g_attrib_send(attrib, 0, opdu, olen, Grib_ResultCb, &gBlecommArg, NULL);


	return;
}

gboolean listen_start(gpointer user_data)
{
	int i = 0;
	int iRes = 0;
	GAttrib *attrib = (GAttrib *)user_data;
	gBleStatus = BLE_STATUS_LISTEN;

	if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s]\n", gBlecommArg.pPipePath, __FUNC_NAME__);

	iRes = g_attrib_register(attrib, ATT_OP_HANDLE_NOTIFY, GATTRIB_ALL_HANDLES, events_handler, attrib, NULL);
	if(iRes == NULL)
	{
		g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] ATT_OP_HANDLE_NOTIFY REGI ERROR !!!\n", gBlecommArg.pPipePath, __FUNC_NAME__);
		Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);
	}

	iRes = g_attrib_register(attrib, ATT_OP_ERROR, GATTRIB_ALL_HANDLES, events_handler, attrib, NULL);
	if(iRes == NULL)
	{
		g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] ATT_OP_ERROR REGI ERROR !!!\n", gBlecommArg.pPipePath, __FUNC_NAME__);
		Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);
	}


	return FALSE;
}

void connect_cb(GIOChannel *io, GError *err, gpointer user_data)
{
	if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s]\n", gBlecommArg.pPipePath, __FUNC_NAME__);

	if (err) {
		g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s] [MSG: %s]\n", gBlecommArg.pPipePath, __FUNC_NAME__, err->message);
		gLibError = err;
		got_error = TRUE;

		gBleStatus = BLE_STATUS_DISCONNECT;
		Grib_BleCmdEnd(BLE_ERROR_CODE_CONNECT_FAIL);
		return;
	}

	gBleStatus = BLE_STATUS_CONNECT;
	gIOChannel	= io;
	gAttrib    = g_attrib_new(gIOChannel);

	if (opt_listen)
		g_idle_add(listen_start, gAttrib);

	operation(gAttrib);
}

void char_write_req_cb(guint8 status, const guint8 *pdu, guint16 plen,
							gpointer user_data)
{
	if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s]\n", gBlecommArg.pPipePath, __FUNC_NAME__);

	if (status != 0) {
		g_printerr("Characteristic Write Request failed: "
						"%s\n", att_ecode2str(status));
		goto done;
	}

	if (!dec_write_resp(pdu, plen) && !dec_exec_write_resp(pdu, plen)) {
		g_printerr("Protocol error\n");
		goto done;
	}

done:
	if (opt_listen == FALSE)
	{
		Grib_BleCmdEnd(BLE_ERROR_CODE_SEND_FAIL);
	}
}

gboolean characteristics_write_req(gpointer user_data)
{
	GAttrib *attrib = (GAttrib *)user_data;
	uint8_t *value;
	size_t len;

	if(gDBG)g_printerr("# BLE-EX: [PIPE: %s] [FUNC: %s]\n", gBlecommArg.pPipePath, __FUNC_NAME__);

	if (opt_handle <= 0) 
	{
		g_printerr("A valid handle is required\n");
		goto error;
	}

	if (opt_value == NULL || opt_value[0] == '\0') 
	{
		g_printerr("A value is required\n");
		goto error;
	}

	len = gatt_attr_data_from_string(opt_value, &value);
	if (len == 0) 
	{
		g_printerr("Invalid value\n");
		goto error;
	}

	gatt_write_char(attrib, opt_handle, value, len, char_write_req_cb, NULL);

	return FALSE;

error:
	Grib_BleCmdEnd(BLE_ERROR_CODE_INTERNAL);
	return FALSE;
}

int main_command(int argc, char *argv[])
{
	GOptionContext *context;
	GOptionGroup *gatt_group, *params_group, *char_rw_group;
	GError *gerr = NULL;
	GIOChannel *chan;

	int iEarlyError = FALSE;

	opt_dst_type = g_strdup("public");
	opt_sec_level = g_strdup("low");

	context = g_option_context_new(NULL);
	g_option_context_add_main_entries(context, options, NULL);

	// GATT commands 
	gatt_group = g_option_group_new("gatt", "GATT commands",
					"Show all GATT commands", NULL, NULL);
	g_option_context_add_group(context, gatt_group);
	g_option_group_add_entries(gatt_group, gatt_options);

	// Primary Services and Characteristics arguments 
	params_group = g_option_group_new("params",
			"Primary Services/Characteristics arguments",
			"Show all Primary Services/Characteristics arguments",
			NULL, NULL);
	g_option_context_add_group(context, params_group);
	g_option_group_add_entries(params_group, primary_char_options);

	// Characteristics value/descriptor read/write arguments 
	char_rw_group = g_option_group_new("char-read-write",
			"Characteristics Value/Descriptor Read/Write arguments",
			"Show all Characteristics Value/Descriptor Read/Write "
			"arguments",
			NULL, 
			NULL );

	g_option_context_add_group(context, char_rw_group);
	g_option_group_add_entries(char_rw_group, char_rw_options);

	if (g_option_context_parse(context, &argc, &argv, &gerr) == FALSE) 
	{
		g_printerr("# BLE-EX: [FILE: %s] [FUNC: %s]\n", __FILE_NAME__, __FUNC_NAME__);
		g_printerr("# BLECOMM MAIN CMD: %s %s\n", gBlecommArg.pPipePath, gerr->message);
		g_error_free(gerr);
	}

	if (opt_interactive) {
		interactive(opt_src, opt_dst, opt_dst_type, opt_psm);
		goto done;
	}

	if (opt_char_write_req)
		operation = characteristics_write_req;
	else {
		gchar *help = g_option_context_get_help(context, TRUE, NULL);
		g_print("%s\n", help);
		g_free(help);
		got_error = TRUE;
		iEarlyError = TRUE;
		gErrorCode = BLE_ERROR_CODE_INVALID_PARAM;
		goto done;
	}

	if (opt_dst == NULL) {
		g_print("Remote Bluetooth address required\n");
		got_error = TRUE;
		iEarlyError = TRUE;
		gErrorCode = BLE_ERROR_CODE_INVALID_PARAM;
		goto done;
	}

#if __NOT_USED__
	gIOChannel = gatt_connect(opt_src, opt_dst, opt_dst_type, opt_sec_level, opt_psm, opt_mtu, connect_cb);
#endif

	gIOChannel = gatt_connectEx(opt_src, opt_dst, opt_dst_type, opt_sec_level, opt_psm, opt_mtu, connect_cb, 
		NULL, NULL, &gLibError);

	if (gIOChannel == NULL)
	{
		got_error = TRUE;
		iEarlyError = TRUE;

		g_printerr("# %s: %s\n", __FUNC_NAME__, gLibError->message);

		if( (gLibError!=NULL) && (Grib_CheckCriticalMsg(gLibError)==TRUE) )
		{
			gErrorCode = BLE_ERROR_CODE_CRITICAL;
			g_printerr("# %s: BLE_ERROR_CODE_CRITICAL\n", __FUNC_NAME__);
		}
		else
		{
			gErrorCode = BLE_ERROR_CODE_CONNECT_FAIL;
		}

		goto done;
	}
	else
	{
		GIOCondition watchCon = G_IO_HUP;
		g_io_add_watch(gIOChannel, watchCon, (GIOFunc)Grib_ChannelWatcher, NULL);
	}

	event_loop = g_main_loop_new(NULL, FALSE);
	if(event_loop == NULL)
	{
		got_error = TRUE;
		iEarlyError = TRUE;
		gErrorCode = BLE_ERROR_CODE_INTERNAL;
		goto done;	
	}

	g_main_loop_run(event_loop);

	g_main_loop_unref(event_loop);


done:
	g_option_context_free(context);
	g_free(opt_src);
	g_free(opt_dst);
	g_free(opt_uuid);
	g_free(opt_sec_level);

	if(iEarlyError)
	{
		g_printerr("# BLE-EX: OCCUR EARLY ERROR: %d\n", gErrorCode);
		Grib_BleCmdEnd(gErrorCode);
	}

	if (got_error)
	{
		exit(EXIT_FAILURE);
	}
	else
		exit(EXIT_SUCCESS);
}


int main(int argc, char *argv[])
{
	int i = 0;
	char *cmdList[] = 
	{
		(char*)"./gatttool", 
//		(char*)"--adapter=hci1",
		(char*)"-b", 
		gBlecommArg.pAddr, 
		(char*)"--char-write-req", 
		(char*)"-a", 
		(char*)"0x0025", 
		(char*)"-n", 
		hex_buf, 
		(char*)"--listen"
	};

	if(gDBG)
	{
		for(i=0; i<argc; i++)printf("# BLECOMM-EX: ARG[%d/%d]: %s\n", i, argc, argv[i]);
	}

	if(argc < ARG_INDEX_OPTION)
	{
		printf("# BLECOMM-EX: INVALID PARAM ERROR !!!\n");
		Grib_BleCmdEnd(BLE_ERROR_CODE_INVALID_PARAM);
		return -1;
	}

	/* ********** ********** ********** ********** ********** ********** */
	// shbaek: Init & Prepare
	/* ********** ********** ********** ********** ********** ********** */
	res_pt = 0;
	event_loop = NULL;
	gErrorCode = BLE_ERROR_CODE_INTERNAL;
	gBleStatus = BLE_STATUS_NONE;

	memset(hex_buf, 0x00, sizeof(hex_buf));
//	memset(res_msg, 0x00, sizeof(res_msg));
	memset(&gBlecommArg, 0x00, sizeof(BleCommArg));

	strcpy(gBlecommArg.pAddr, 		ARG_INDEX_BLE_ADDR[argv]);
	strcpy(gBlecommArg.pPipePath,	ARG_INDEX_PIPE_PATH[argv]);
	strcpy(gBlecommArg.pSendBuff,	ARG_INDEX_SEND_MSG[argv]);
	gBlecommArg.isDebug = gDBG;

	if(argc == ARG_INDEX_OPTION+1)
	{
		strcpy(gBlecommArg.pOption,ARG_INDEX_OPTION[argv]);
	}

	Grib_BleCommSetArg(&gBlecommArg);

	str2hex(gBlecommArg.pSendBuff, hex_buf);
	gBleStatus = BLE_STATUS_INIT;
	gLibError = NULL;
	/* ********** ********** ********** ********** ********** ********** */


	main_command(sizeof(cmdList)/sizeof(char *), cmdList);

	return 0;
}
