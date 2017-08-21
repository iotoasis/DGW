#ifndef __GRIB_HTTP_H__
#define __GRIB_HTTP_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

#include "grib_define.h"
#include "grib_util.h"

#include "grib_log.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Constant
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define HTTP_MAX_SIZE										(SIZE_1K * 128)

#define HTTP_MAX_SIZE_SEND_MSG							HTTP_MAX_SIZE
#define HTTP_MAX_SIZE_RECV_MSG							HTTP_MAX_SIZE

#define HTTP_MAX_SIZE_HEAD								(SIZE_1K * 8) //shbaek: Default 8KB.
#define HTTP_MAX_SIZE_BODY								(HTTP_MAX_SIZE - HTTP_MAX_SIZE_HEAD)

#define HTTP_MAX_SIZE_CHUNKED_HEX_STR					6 //shbaek: [Hex: ~4] [CR+LN]
#define HTTP_MAX_SIZE_IP_STR								GRIB_MAX_SIZE_IP_STR

#define HTTP_TIME_OUT_SEC_CONNECT							10
#define HTTP_TIME_OUT_SEC_RECEIVE							100 //shbaek: Long Polling Time Out -> 90 Sec

#define HTTP_ENC_TYPE_NONE								0
#define HTTP_ENC_TYPE_BASE64								1

#define HTTP_VERSION_1P1									"HTTP/1.1"

#define HTTP_METHOD_POST									"POST"
#define HTTP_METHOD_GET									"GET"
#define HTTP_METHOD_PUT									"PUT"
#define HTTP_METHOD_DELETE								"DELETE"

#define HTTP_TRANS_ENCODE_CHUNK							"Transfer-Encoding: chunked"

#define HTTP_CONTENT_TYPE_TEXT							"text/plain"
#define HTTP_CONTENT_TYPE_XML								"application/xml"
#define HTTP_CONTENT_TYPE_JSON							"application/json"
#define HTTP_CONTENT_TYPE_RDF_XML							"application/rdf+xml"

#define HTTP_CONTENT_TYPE_ONEM2M_RES_XML					"application/vnd.onem2m-res+xml"
#define HTTP_CONTENT_TYPE_ONEM2M_RES_JSON				"application/vnd.onem2m-res+json"

#define HTTP_DEFAULT_SENDER								"UN-KNOWN"
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Type
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

//shbaek: Too Many Type... Reffer To RFC 7231
typedef enum
{
	HTTP_STATUS_CODE_UNKNOWN							= -1,

	HTTP_STATUS_CODE_BASE_INFO						= 100,
	HTTP_STATUS_CODE_CONTINUE							= 100,
	HTTP_STATUS_CODE_SWITCHING_PROTOCOL				= 101,
	HTTP_STATUS_CODE_PROCESSING						= 102,

	HTTP_STATUS_CODE_SUCCESS_BASE					= 200,
	HTTP_STATUS_CODE_OK								= 200,
	HTTP_STATUS_CODE_CREATED							= 201,
	HTTP_STATUS_CODE_ACCEPTED							= 202,
	HTTP_STATUS_CODE_NON_AUTH_INFO					= 203,
	HTTP_STATUS_CODE_NO_CONTENT						= 204,
	HTTP_STATUS_CODE_RESET_CONTENT					= 205,
	HTTP_STATUS_CODE_PARTIAL_CONTENT					= 206,
	HTTP_STATUS_CODE_MULTI_STATUS					= 207,
	HTTP_STATUS_CODE_ALREADY_REPORTED				= 208,
	HTTP_STATUS_CODE_SUCCESS_MAX						= 299,

	HTTP_STATUS_CODE_BASE_REDIRECTION				= 300,
	HTTP_STATUS_CODE_MULTIPLE_CHOICE					= 300,
	HTTP_STATUS_CODE_MOVED_PERMANENTLY				= 301,
	HTTP_STATUS_CODE_FOUND							= 302,
	HTTP_STATUS_CODE_SEE_OTHER						= 303,
	HTTP_STATUS_CODE_NOT_MODIFIED					= 304,
	HTTP_STATUS_CODE_USE_PROXY						= 305,
	HTTP_STATUS_CODE_TEMP_REDIRECT					= 307,
	HTTP_STATUS_CODE_PERMANENT_REDIRECT				= 308,

	HTTP_STATUS_CODE_BASE_CLIENT_ERROR				= 400,
	HTTP_STATUS_CODE_BAD_REQUEST						= 400,
	HTTP_STATUS_CODE_UNAUTHORIZED					= 401,
	HTTP_STATUS_CODE_PAYMENT_REQUIRED				= 402,
	HTTP_STATUS_CODE_FORBIDDEN						= 403,
	HTTP_STATUS_CODE_NOT_FOUND						= 404,
	HTTP_STATUS_CODE_METHOD_NOT_ALLOW				= 405,
	HTTP_STATUS_CODE_NOT_ACCEPT						= 406,
	HTTP_STATUS_CODE_PROXY_AUTH_REQUIRED			= 407,
	HTTP_STATUS_CODE_REQUEST_TIME_OUT				= 408,
	HTTP_STATUS_CODE_CONFLICT							= 409,
	HTTP_STATUS_CODE_GONE								= 410,

	HTTP_STATUS_CODE_BASE_SERVER_ERROR				= 500,
	HTTP_STATUS_CODE_INTERNAL_SERVER_ERROR			= 501,
	HTTP_STATUS_CODE_NOT_IMPLEMENT					= 502,
	HTTP_STATUS_CODE_BAD_GATEWAY						= 503,
	HTTP_STATUS_CODE_SERVICE_UNAVAILABLE			= 504,
	HTTP_STATUS_CODE_INSUFFICIENT_STORAGE			= 507,

	HTTP_STATUS_CODE_MAX
}Http_StatusCode;

typedef struct
{
	char* 	serverIP;
	int		serverPort;

	char* 	sendBuff;
	char*	recvBuff;

	int		statusCode;
	char	statusMsg[GRIB_MAX_SIZE_MIDDLE];

	char*	LABEL;
}Grib_HttpMsgInfo;

typedef struct
{
	char*	httpSender;
	char*	httpSendMsg;
	char	httpErrMsg[GRIB_MAX_SIZE_MIDDLE];
}Grib_HttpLogInfo;


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
void Grib_HttpSetDebug(int iDebug, int iTombStone);

void Grib_HttpTombStone(Grib_HttpLogInfo* pLogInfo);

int  Grib_HttpConnect(char* serverIP, int serverPort);
int  Grib_HttpSendMsg(Grib_HttpMsgInfo* pMsg);
int  Grib_HttpResParser(Grib_HttpMsgInfo* pMsg);


#endif
