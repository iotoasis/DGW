#ifndef __GRIB_ONEM2M_H__
#define __GRIB_ONEM2M_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_define.h"
#include "grib_db.h"
#include "grib_util.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Constant
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define ONEM2M_MAX_SIZE_BRIEF								GRIB_MAX_SIZE_BRIEF
#define ONEM2M_MAX_SIZE_SHORT								GRIB_MAX_SIZE_SHORT
#define ONEM2M_MAX_SIZE_MIDDLE							GRIB_MAX_SIZE_MIDDLE
#define ONEM2M_MAX_SIZE_LONG								GRIB_MAX_SIZE_LONG
#define ONEM2M_MAX_SIZE_URI								GRIB_MAX_SIZE_DLONG

#define ONEM2M_MAX_SIZE_IP_STR							GRIB_MAX_SIZE_IP_STR
#define ONEM2M_MAX_SIZE_SEND_MSG							(SIZE_1K)
#define ONEM2M_MAX_SIZE_RECV_MSG							(SIZE_1K * 4)

#define HTTP_TIME_OUT_SEC_CONNECT							5
#define HTTP_TIME_OUT_SEC_RECEIVE							100 //shbaek: Long Polling Time Out -> 90 Sec

#define HTTP_VERSION_1P1									"HTTP/1.1"

#define HTTP_METHOD_POST									"POST"
#define HTTP_METHOD_GET									"GET"
#define HTTP_METHOD_PUT									"PUT"
#define HTTP_METHOD_DELETE								"DELETE"

#define HTTP_CONTENT_TYPE_TEXT							"text/plain"
#define HTTP_CONTENT_TYPE_XML								"application/xml"
#define HTTP_CONTENT_TYPE_JSON							"application/json"
#define HTTP_CONTENT_TYPE_ONEM2M_RES_XML					"application/vnd.onem2m-res+xml"
#define HTTP_CONTENT_TYPE_ONEM2M_RES_JSON				"application/vnd.onem2m-res+json"

//shbaek: 1. Device(=App Entity) Dir
#define ONEM2M_URI_CONTENT_HUB							"hub"
#define ONEM2M_URI_CONTENT_DEVICE							"device"

//shbaek: Fixed URI String
#define ONEM2M_URI_CONTENT_CSE							"/herit-in/herit-cse"
#define ONEM2M_URI_CONTENT_POLLING_CHANNEL				"pollingchannel"
#define ONEM2M_URI_CONTENT_EXECUTE						"execute"
#define ONEM2M_URI_CONTENT_STATUS							"status"

#define ONEM2M_URI_CONTENT_SUBSCRIPTION					"subscription"
#define ONEM2M_URI_CONTENT_LA								"la"
#define ONEM2M_URI_CONTENT_PCU							"pcu"


/* ********** ********** ********** ********** APP ENTITY ********** ********** ********** ********** */
#define ONEM2M_HEAD_FORMAT_APP_ENTITY_CREATE					\
		"POST /herit-in/herit-cse HTTP/1.1\r\n"						\
		"Host: %s:%d\r\n"											\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"X-M2M-NM: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_APP_ENTITY_CREATE					\
		"{\r\n"														\
		"    \"lbl\": \"%s\",\r\n"									\
		"    \"apn\": \"%s\",\r\n"									\
		"    \"api\": \"%s\",\r\n"									\
		"    \"poa\": \"%s:%d\",\r\n"								\
		"    \"rr\": \"%s\"\r\n"									\
		"}\r\n"														\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_APP_ENTITY_RETRIEVE					\
		"GET /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_APP_ENTITY_DELETE					\
		"DELETE /herit-in/herit-cse/%s HTTP/1.1\r\n"				\
		"Host: %s:%d\r\n"											\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

/* ********** ********** ********** ********** CONTAINER ********** ********** ********** ********** */

#define ONEM2M_HEAD_FORMAT_CONTAINER_CREATE						\
		"POST /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"X-M2M-NM: %s\r\n"											\
		"\r\n"


#define ONEM2M_BODY_FORMAT_CONTAINER_CREATE						\
		"{\r\n"														\
		"    \"lbl\": \"%s\",\r\n"									\
		"    \"et\": \"%s\",\r\n"									\
		"    \"mni\": \"%llu\",\r\n"								\
		"    \"mbs\": \"%llu\",\r\n"								\
		"    \"mia\": \"%llu\"\r\n"									\
		"}\r\n"														\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_CONTAINER_RETRIEVE					\
		"GET /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_CONTAINER_DELETE						\
		"DELETE /herit-in/herit-cse/%s HTTP/1.1\r\n"				\
		"Host: %s:%d\r\n"											\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

/* ********** ********** ********** ********** POLLING CHANNEL ********** ********** ********** ********** */

#define ONEM2M_HEAD_FORMAT_POLLING_CHANNEL_CREATE				\
		"POST /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"X-M2M-NM: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_POLLING_CHANNEL_CREATE				\
		"{\r\n"														\
		"    \"lbl\": \"%s\"\r\n"									\
		"}\r\n"														\
		"\r\n"


#define ONEM2M_HEAD_FORMAT_SUBSCRIPTION_CREATE					\
		"POST /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"X-M2M-NM: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_SUBSCRIPTION_CREATE					\
		"{\r\n"														\
		"    \"lbl\": \"%s\",\r\n"									\
		"    \"et\": \"%s\",\r\n"									\
		"    \"enc\": { \"rss\":[\"1\",\"2\",\"3\",\"4\"] },\r\n"	\
		"    \"nu\": \"%s\"\r\n"									\
		"}\r\n"														\
		"\r\n"

/* ********** ********** ********** ********** RUN TIME ********** ********** ********** ********** */

#define ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_CREATE				\
		"POST /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"


#define ONEM2M_BODY_FORMAT_CONTENT_INSTANCE_CREATE				\
		"{\r\n"														\
		"    \"lbl\": \"%s\",\r\n"									\
		"    \"cnf\": \"%s\",\r\n"									\
		"    \"con\": \"%s\"\r\n"									\
		"}\r\n"														\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_RETRIEVE			\
		"GET /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_LONG_POLLING							\
		"GET /herit-in/herit-cse/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Content-Type:application/vnd.onem2m-res+xml\r\n"			\
		"Accept:application/vnd.onem2m-res+xml\r\n"					\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_FORMAT_CONTENT_VALUE_HUB_INFO					\
		"[HUB_VER: %s], "											\
		"[HUB_ID : %s], "											\
		"[HUB_IP : %s]"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Type
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
typedef enum
{
	ONEM2M_OPERATION_TYPE_UNKNOWN					= -1,
	ONEM2M_OPERATION_TYPE_NONE						= 0,
	ONEM2M_OPERATION_TYPE_CREATE						= 1,
	ONEM2M_OPERATION_TYPE_RETRIEVE					= 2,
	ONEM2M_OPERATION_TYPE_UPDATE						= 3,
	ONEM2M_OPERATION_TYPE_DELETE						= 4,
	ONEM2M_OPERATION_TYPE_NOTIFY						= 5,
	ONEM2M_OPERATION_TYPE_MAX
}OneM2M_Operation_Type;

typedef enum
{
	ONEM2M_RESOURCE_TYPE_UNKNOWN						= -1,
	ONEM2M_RESOURCE_TYPE_NONE							= 0,
	ONEM2M_RESOURCE_TYPE_ACCESS_CONTROL_POLICY		= 1,
	ONEM2M_RESOURCE_TYPE_APP_ENTITY					= 2,
	ONEM2M_RESOURCE_TYPE_CONTAINER					= 3,
	ONEM2M_RESOURCE_TYPE_CONTENT_INSTANCE			= 4,
	ONEM2M_RESOURCE_TYPE_CSE_BASE					= 5,
	ONEM2M_RESOURCE_TYPE_GROUP						= 9,
	ONEM2M_RESOURCE_TYPE_NODE							= 14,
	ONEM2M_RESOURCE_TYPE_POLLING_CHANNEL			= 15,
	ONEM2M_RESOURCE_TYPE_REMOTE_CSE					= 16,
	ONEM2M_RESOURCE_TYPE_REQUEST						= 17,
	ONEM2M_RESOURCE_TYPE_SUBSCRIPTION				= 23,
	ONEM2M_RESOURCE_TYPE_MAX
}OneM2M_ResourceType;

//shbaek: Too Many Type... Reffer To RFC 7231
typedef enum
{
	HTTP_STATUS_CODE_UNKNOWN							= -1,

	HTTP_STATUS_CODE_BASE_INFO						= 100,
	HTTP_STATUS_CODE_CONTINUE							= 100,
	HTTP_STATUS_CODE_SWITCHING_PROTOCOL				= 101,
	HTTP_STATUS_CODE_PROCESSING						= 102,

	HTTP_STATUS_CODE_BASE_SUCCESS					= 200,
	HTTP_STATUS_CODE_OK								= 200,
	HTTP_STATUS_CODE_CREATED							= 201,
	HTTP_STATUS_CODE_ACCEPTED							= 202,
	HTTP_STATUS_CODE_NON_AUTH_INFO					= 203,
	HTTP_STATUS_CODE_NO_CONTENT						= 204,
	HTTP_STATUS_CODE_RESET_CONTENT					= 205,
	HTTP_STATUS_CODE_PARTIAL_CONTENT					= 206,
	HTTP_STATUS_CODE_MULTI_STATUS					= 207,
	HTTP_STATUS_CODE_ALREADY_REPORTED				= 208,

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
	char http_SendData[ONEM2M_MAX_SIZE_SEND_MSG+1];

//	char http_Version[ONEM2M_MAX_SIZE_BRIEF+1];
	char http_Method[ONEM2M_MAX_SIZE_BRIEF+1];
	int  http_ContentLen;
	char http_ContentType[ONEM2M_MAX_SIZE_SHORT+1];
	OneM2M_ResourceType xM2M_ResourceType;

	char xM2M_URI[ONEM2M_MAX_SIZE_URI+1];
	char xM2M_Origin[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_Func[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_ReqID[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_NM[ONEM2M_MAX_SIZE_SHORT+1];
	OneM2M_Operation_Type xM2M_OperationType;

	char xM2M_CNF[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_CON[ONEM2M_MAX_SIZE_MIDDLE+1];
}OneM2M_ReqParam;

typedef struct
{
	char http_RecvData[ONEM2M_MAX_SIZE_RECV_MSG+1];

//	char http_Version[ONEM2M_MAX_SIZE_BRIEF+1];
	int  http_ResNum;
	char http_ResMsg[ONEM2M_MAX_SIZE_SHORT+1];

//	int  http_ContentLen;
//	char http_ContentType[ONEM2M_MAX_SIZE_SHORT+1];
//	char http_ContentLocation[ONEM2M_MAX_SIZE_SHORT+1];
//	int  http_ConnectStatus;

	int  xM2M_ResCode;
	char xM2M_ReqID[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_Origin[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_Result[ONEM2M_MAX_SIZE_SHORT+1];

	char xM2M_RsrcID[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_PrntID[ONEM2M_MAX_SIZE_SHORT+1];
	char xM2M_Content[ONEM2M_MAX_SIZE_MIDDLE+1];
}OneM2M_ResParam;


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Macro
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */

#define ONEM2M_OPERATION_TO_HTTP_METHOD(op)		(op==ONEM2M_OPERATION_TYPE_CREATE?HTTP_METHOD_POST\
													:op==ONEM2M_OPERATION_TYPE_RETRIEVE?HTTP_METHOD_GET\
													:op==ONEM2M_OPERATION_TYPE_UPDATE?HTTP_METHOD_PUT\
													:op==ONEM2M_OPERATION_TYPE_DELETE?HTTP_METHOD_DELETE\
													:op==ONEM2M_OPERATION_TYPE_NOTIFY?HTTP_METHOD_POST\
													:NULL)


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Function Prototype
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
int Grib_SetServerConfig(void);
int Grib_HttpConnect(char* serverIP, int serverPort);

//2 shbaek: NEED: xM2M_NM
int Grib_AppEntityCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);
//2 shbaek: NEED: xM2M_Origin
int Grib_AppEntityRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);
//2 shbaek: NEED: xM2M_Origin
int Grib_AppEntityDelete(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);

//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_NM
int Grib_ContainerCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);
//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContainerRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);
//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContainerDelete(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);

//2 shbaek: NEED: xM2M_Origin
int Grib_PollingChannelCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);
//2 shbaek: NEED: xM2M_Origin xM2M_Func
int Grib_SubsciptionCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);

//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_CNF[If NULL, Set Default "text/plain:0"], xM2M_CON
int Grib_ContentInstanceCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);
//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContentInstanceRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);

//2 shbaek: NEED: xM2M_Origin
int Grib_LongPolling(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);


int Grib_CreateOneM2MTree(Grib_DbRowDeviceInfo* pRowDeviceInfo);

int Grib_UpdateHubInfo(void);
int Grib_UpdateDeviceInfo(Grib_DbAll *pDbAll);

#endif
