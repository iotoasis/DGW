#ifndef __GRIB_ONEM2M_H__
#define __GRIB_ONEM2M_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netdb.h>
#include <time.h>

#include "grib_define.h"
#include "grib_config.h"
#include "grib_http.h"
#include "grib_smd.h"
#include "grib_auth.h"

#ifdef FEATURE_CAS
#include "grib_cas.h"
#endif

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Constant
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: 1. Device(=App Entity) Dir
#define ONEM2M_URI_CONTENT_HUB							"hub"
#define ONEM2M_URI_CONTENT_DEVICE							"device"

//shbaek: Fixed URI String
#define ONEM2M_URI_CONTENT_CSE							"/%s/%s"
#define ONEM2M_URI_CONTENT_POLLING_CHANNEL				"pollingchannel"
#define ONEM2M_URI_CONTENT_EXECUTE						"execute"
#define ONEM2M_URI_CONTENT_STATUS							"status"

#define ONEM2M_URI_CONTENT_SUBSCRIPTION					"subscription"
#define ONEM2M_URI_CONTENT_LA								"la"
#define ONEM2M_URI_CONTENT_PCU							"pcu"

#define ONEM2M_URI_CONTENT_SEM_DEC						"semanticDescriptor"


/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Define Fixed Data
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define ONEM2M_FIX_ATTR_ET_DATE							"20991231"
#define ONEM2M_FIX_ATTR_ET_TIME							"122359"

#define ONEM2M_FIX_ATTR_ET								"20991231T235959"
//#define ONEM2M_FIX_ATTR_MNI								INT_MAX
#define ONEM2M_FIX_ATTR_MNI								1000000ULL
#define ONEM2M_FIX_ATTR_MBS								1024000ULL
#define ONEM2M_FIX_ATTR_MIA								3600000ULL
#define ONEM2M_FIX_ATTR_RR								"FALSE"

#define ONEM2M_FIX_ATTR_ENC								"\"net\":[1,2,3,4]"

#define ONEM2M_FIX_ATTR_NU_FORMAT							"http://%s:%d/%s/%s/%s/" \
															ONEM2M_URI_CONTENT_POLLING_CHANNEL "/" ONEM2M_URI_CONTENT_PCU

#define ONEM2M_EXPIRE_TIME_STR_SIZE						GRIB_MAX_SIZE_TIME_STR

#ifdef FEATURE_CAS	//shbaek: TBD
#define CAS_HEAD_FORMAT_ATTR_AUTH_KEY					"Authorization: Basic %s\r\n"
#else
#define CAS_HEAD_FORMAT_ATTR_AUTH_KEY						""
#endif

/* ********** ********** ********** ********** APP ENTITY ********** ********** ********** ********** */

#define ONEM2M_HEAD_FORMAT_APP_ENTITY_CREATE					\
		"POST /%s/%s HTTP/1.1\r\n"						\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_APP_ENTITY_CREATE					\
		"{\r\n"														\
		"    \"ae\":\r\n"											\
		"    {\r\n"													\
		"        \"lbl\": [\"%s\"],\r\n"							\
		"        \"apn\": \"%s\",\r\n"								\
		"        \"api\": \"%s\",\r\n"								\
		"        \"poa\": \"%s:%d\",\r\n"							\
		"        \"rr\": \"%s\",\r\n"								\
		"        \"rn\": \"%s\",\r\n"								\
		"        \"et\": \"%s\"\r\n"								\
		"    }\r\n"													\
		"}\r\n"														\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_APP_ENTITY_RETRIEVE					\
		"GET /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_APP_ENTITY_DELETE					\
		"DELETE /%s/%s/%s HTTP/1.1\r\n"				\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

/* ********** ********** ********** ********** CONTAINER ********** ********** ********** ********** */

#define ONEM2M_HEAD_FORMAT_CONTAINER_CREATE						\
		"POST /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_CONTAINER_CREATE						\
		"{\r\n"														\
		"    \"cnt\":\r\n"											\
		"    {\r\n"													\
		"        \"lbl\": [\"%s\"],\r\n"							\
		"        \"mni\": \"%llu\",\r\n"							\
		"        \"mbs\": \"%llu\",\r\n"							\
		"        \"mia\": \"%llu\",\r\n"							\
		"        \"rn\": \"%s\",\r\n"								\
		"        \"et\": \"%s\"\r\n"								\
		"    }\r\n"													\
		"}\r\n"														\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_CONTAINER_RETRIEVE					\
		"GET /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_CONTAINER_DELETE						\
		"DELETE /%s/%s/%s HTTP/1.1\r\n"				\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

/* ********** ********** ********** ********** POLLING CHANNEL ********** ********** ********** ********** */

#define ONEM2M_HEAD_FORMAT_POLLING_CHANNEL_CREATE				\
		"POST /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_POLLING_CHANNEL_CREATE				\
		"{\r\n"														\
		"    \"pch\":\r\n"											\
		"    {\r\n"													\
		"        \"lbl\": [\"%s\"],\r\n"							\
		"        \"rn\": \"%s\",\r\n"								\
		"        \"et\": \"%s\"\r\n"								\
		"    }\r\n"													\
		"}\r\n"														\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_SUBSCRIPTION_CREATE					\
		"POST /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_SUBSCRIPTION_CREATE					\
		"{\r\n"														\
		"    \"sub\":\r\n"											\
		"    {\r\n"													\
		"        \"lbl\": [\"%s\"],\r\n"							\
		"        \"enc\":\r\n"										\
		"        {\r\n"												\
		"            %s\r\n"										\
		"        },\r\n"											\
		"        \"nu\": [\"%s\"],\r\n"								\
		"        \"rn\": \"%s\",\r\n"								\
		"        \"et\": \"%s\"\r\n"								\
		"    }\r\n"													\
		"}\r\n"														\
		"\r\n"

/* ********** ********** ********** ********** RUN TIME ********** ********** ********** ********** */

#define ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_CREATE				\
		"POST /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_CONTENT_INSTANCE_CREATE				\
		"{\r\n"														\
		"    \"cin\":\r\n"											\
		"    {\r\n"													\
		"        \"lbl\": [\"%s\"],\r\n"							\
		"        \"et\": \"%s\",\r\n"								\
		"        \"cnf\": \"%s\",\r\n"								\
		"        \"con\": \"%s\"\r\n"								\
		"    }\r\n"													\
		"}\r\n"														\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_CONTENT_INSTANCE_RETRIEVE			\
		"GET /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type:application/vnd.onem2m-res+json\r\n"			\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_HEAD_FORMAT_LONG_POLLING							\
		"GET /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+xml\r\n"					\
		"Content-Type:application/vnd.onem2m-res+xml\r\n"			\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_FORMAT_CONTENT_VALUE_HUB_INFO					\
		"[HUB_VER: %s], "											\
		"[HUB_ID : %s], "											\
		"[HUB_IP : %s]"

#define ONEM2M_HEAD_FORMAT_SEMANTIC_DESCRIPTOR_UPLOAD			\
		"POST /%s/%s/%s HTTP/1.1\r\n"					\
		"Host: %s:%d\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"				\
		"Content-Type: application/vnd.onem2m-res+json; ty=%d\r\n"	\
		"Content-Length: %d\r\n"									\
		CAS_HEAD_FORMAT_ATTR_AUTH_KEY								\
		"X-M2M-Origin: %s\r\n"										\
		"X-M2M-RI: %s\r\n"											\
		"\r\n"

#define ONEM2M_BODY_FORMAT_SEMANTIC_DESCRIPTOR_UPLOAD			\
		"{\r\n"														\
		"    \"smd\":\r\n"											\
		"    {\r\n"													\
		"        \"lbl\": [\"%s\"],\r\n"							\
		"        \"rn\": \"%s\",\r\n"								\
		"        \"et\": \"%s\",\r\n"								\
		"        \"dcrp\": \"%s\",\r\n"								\
		"        \"dsp\": \"%s\"\r\n"								\
		"    }\r\n"													\
		"}\r\n"														\
		"\r\n"


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
	ONEM2M_RESOURCE_TYPE_SEMANTIC_DESCRIPTOR		= 24,
	ONEM2M_RESOURCE_TYPE_MAX
}OneM2M_ResourceType;

typedef struct
{
	char* http_SendDataEx; //shbaek: If Too Large Data, Use This. ex)Semantic Descriptor
	char http_SendData[HTTP_MAX_SIZE_SEND_MSG];

	char http_Method[GRIB_MAX_SIZE_BRIEF];
	int  http_ContentLen;
	char http_ContentType[GRIB_MAX_SIZE_SHORT];
	OneM2M_ResourceType xM2M_ResourceType;

	char xM2M_URI[GRIB_MAX_SIZE_URI];
	char xM2M_Origin[GRIB_MAX_SIZE_SHORT];
	char xM2M_AeName[GRIB_MAX_SIZE_MIDDLE];
	char xM2M_Func[GRIB_MAX_SIZE_SHORT];
	char xM2M_ReqID[GRIB_MAX_SIZE_MIDDLE];
	char xM2M_NM[GRIB_MAX_SIZE_MIDDLE];
	char xM2M_AttrLBL[GRIB_MAX_SIZE_MIDDLE];
	OneM2M_Operation_Type xM2M_OperationType;

	char xM2M_CNF[GRIB_MAX_SIZE_SHORT];
	char xM2M_CON[GRIB_MAX_SIZE_MIDDLE];

#ifdef FEATURE_CAS
	char* authKey;
#endif
}OneM2M_ReqParam;

typedef struct
{
	char http_RecvData[HTTP_MAX_SIZE_RECV_MSG];

//	char http_Version[GRIB_MAX_SIZE_BRIEF];
	int  http_ResNum;
	char http_ResMsg[GRIB_MAX_SIZE_MIDDLE];

//	int  http_ContentLen;
//	char http_ContentType[GRIB_MAX_SIZE_SHORT];
//	char http_ContentLocation[GRIB_MAX_SIZE_SHORT];
//	int  http_ConnectStatus;

	int  xM2M_ResCode;
	char xM2M_ReqID[GRIB_MAX_SIZE_SHORT];
	char xM2M_Origin[GRIB_MAX_SIZE_SHORT];
	char xM2M_Result[GRIB_MAX_SIZE_SHORT];

	char xM2M_RsrcID[GRIB_MAX_SIZE_SHORT];
	char xM2M_PrntID[GRIB_MAX_SIZE_SHORT];
	char xM2M_Content[GRIB_MAX_SIZE_MIDDLE];

	char xM2M_CreateTime[ONEM2M_EXPIRE_TIME_STR_SIZE];
	char xM2M_ExpireTime[ONEM2M_EXPIRE_TIME_STR_SIZE];
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
int Grib_SiSetServerConfig(void);
int Grib_GetAttrExpireTime(char* attrBuff, TimeInfo* pTime);
int Grib_isAvailableExpireTime(char* xM2M_ExpireTime);

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

//2 shbaek: NEED: xM2M_URI, xM2M_Origin, xM2M_CNF[If NULL, Set Default "text/plain:0"], xM2M_CON
int Grib_ContentInstanceCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);
//2 shbaek: NEED: xM2M_URI, xM2M_Origin
int Grib_ContentInstanceRetrieve(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);

//2 shbaek: NEED: xM2M_Origin
int Grib_LongPolling(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);

//2 shbaek: NEED: xM2M_Origin xM2M_Func
int Grib_SubsciptionCreate(OneM2M_ReqParam *pReqParam, OneM2M_ResParam *pResParam);




int Grib_UpdateHubInfo(char* pAuthKey);
int Grib_UpdateDeviceInfo(Grib_DbAll* pDbAll, char* pAuthKey);
int Grib_CreateOneM2MTree(Grib_DbRowDeviceInfo* pRowDeviceInfo, char* pAuthKey);


#endif
