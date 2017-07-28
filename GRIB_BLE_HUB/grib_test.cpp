/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Include File
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include "grib_test.h"

#include <iostream>
#include <vector>


using namespace std;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
typedef struct
{
	char* reqName;
	char* reqTime;
	int   reqState;
}GribReqInfo;

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** **********
shbaek: Global Variable
********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
std::vector<GribReqInfo> gReqList;
std::vector<int> gIntList;

int Grib_TestMain(int argc, char **argv)
{
	int iRes = GRIB_ERROR;
	int iSize = 0;
	const char* FUNC_TAG = "TEST-MAIN";

	GribReqInfo defaultReq[] =
	{
		{"Req Name - 1", "20170101T010101", 0},
		{"Req Name - 2", "20170202T020202", 0}
	};

	GribReqInfo argReq;
	char tempTime[GRIB_MAX_SIZE_TIME_STR] = {'\0', };

	argReq.reqName = argv[GRIB_CMD_ARG1];
	iRes = Grib_GetCurrDateTime(tempTime);
	if(iRes==GRIB_DONE)argReq.reqTime = tempTime;
	argReq.reqState = ATOI(argv[GRIB_CMD_ARG2]);

	for(int i=0; i<(sizeof(defaultReq)/sizeof(GribReqInfo)); i++)
	{
		gReqList.push_back(defaultReq[i]);
	}
	iSize = gReqList.size();
	GRIB_LOGD("# %s: DEFAULT REQ SIZE: %d\n", FUNC_TAG, iSize);

	gReqList.push_back(argReq);
	GRIB_LOGD("# %s: ADD REQ SIZE: %d\n", FUNC_TAG, iSize);

	for(std::vector<GribReqInfo>::iterator req=gReqList.begin(); req!=gReqList.end(); req++)	
	{
		GRIB_LOGD("\n# ##### ##### ##### ##### ##### ##### ##### #####\n");

		GRIB_LOGD("# %s: REQ NAME : %s\n", FUNC_TAG, req->reqName);
		GRIB_LOGD("# %s: REQ TIME : %s\n", FUNC_TAG, req->reqTime);
		GRIB_LOGD("# %s: REQ STATE: %d\n", FUNC_TAG, req->reqState);
	}

	return GRIB_DONE;


}

const char *json=
    "{"
    "\"foo\": [\"bar\", \"baz\"],"
    "\"\": 0,"
    "\"a/b\": 1,"
    "\"c%d\": 2,"
    "\"e^f\": 3,"
    "\"g|h\": 4,"
    "\"i\\\\j\": 5,"
    "\"k\\\"l\": 6,"
    "\" \": 7,"
    "\"m~n\": 8"
    "}";

const char *tests[12] = {"","/foo","/foo/0","/","/a~1b","/c%d","/e^f","/g|h","/i\\j","/k\"l","/ ","/m~0n"};

/* JSON Apply Patch tests: */
const char *patches[15][3] =
{
    {"{ \"foo\": \"bar\"}", "[{ \"op\": \"add\", \"path\": \"/baz\", \"value\": \"qux\" }]","{\"baz\": \"qux\",\"foo\": \"bar\"}"},
    {"{ \"foo\": [ \"bar\", \"baz\" ] }", "[{ \"op\": \"add\", \"path\": \"/foo/1\", \"value\": \"qux\" }]","{\"foo\": [ \"bar\", \"qux\", \"baz\" ] }"},
    {"{\"baz\": \"qux\",\"foo\": \"bar\"}"," [{ \"op\": \"remove\", \"path\": \"/baz\" }]","{\"foo\": \"bar\" }"},
    {"{ \"foo\": [ \"bar\", \"qux\", \"baz\" ] }","[{ \"op\": \"remove\", \"path\": \"/foo/1\" }]","{\"foo\": [ \"bar\", \"baz\" ] }"},
    {"{ \"baz\": \"qux\",\"foo\": \"bar\"}","[{ \"op\": \"replace\", \"path\": \"/baz\", \"value\": \"boo\" }]","{\"baz\": \"boo\",\"foo\": \"bar\"}"},
    {"{\"foo\": {\"bar\": \"baz\",\"waldo\": \"fred\"},\"qux\": {\"corge\": \"grault\"}}","[{ \"op\": \"move\", \"from\": \"/foo/waldo\", \"path\": \"/qux/thud\" }]","{\"foo\": {\"bar\": \"baz\"},\"qux\": {\"corge\": \"grault\",\"thud\": \"fred\"}}"},
    {"{ \"foo\": [ \"all\", \"grass\", \"cows\", \"eat\" ] }","[ { \"op\": \"move\", \"from\": \"/foo/1\", \"path\": \"/foo/3\" }]","{ \"foo\": [ \"all\", \"cows\", \"eat\", \"grass\" ] }"},
    {"{\"baz\": \"qux\",\"foo\": [ \"a\", 2, \"c\" ]}","[{ \"op\": \"test\", \"path\": \"/baz\", \"value\": \"qux\" },{ \"op\": \"test\", \"path\": \"/foo/1\", \"value\": 2 }]",""},
    {"{ \"baz\": \"qux\" }","[ { \"op\": \"test\", \"path\": \"/baz\", \"value\": \"bar\" }]",""},
    {"{ \"foo\": \"bar\" }","[{ \"op\": \"add\", \"path\": \"/child\", \"value\": { \"grandchild\": { } } }]","{\"foo\": \"bar\",\"child\": {\"grandchild\": {}}}"},
    {"{ \"foo\": \"bar\" }","[{ \"op\": \"add\", \"path\": \"/baz\", \"value\": \"qux\", \"xyz\": 123 }]","{\"foo\": \"bar\",\"baz\": \"qux\"}"},
    {"{ \"foo\": \"bar\" }","[{ \"op\": \"add\", \"path\": \"/baz/bat\", \"value\": \"qux\" }]",""},
    {"{\"/\": 9,\"~1\": 10}","[{\"op\": \"test\", \"path\": \"/~01\", \"value\": 10}]",""},
    {"{\"/\": 9,\"~1\": 10}","[{\"op\": \"test\", \"path\": \"/~01\", \"value\": \"10\"}]",""},
    {"{ \"foo\": [\"bar\"] }","[ { \"op\": \"add\", \"path\": \"/foo/-\", \"value\": [\"abc\", \"def\"] }]","{\"foo\": [\"bar\", [\"abc\", \"def\"]] }"}
};

/* JSON Apply Merge tests: */
const char *merges[15][3] =
{
    {"{\"a\":\"b\"}", "{\"a\":\"c\"}", "{\"a\":\"c\"}"},
    {"{\"a\":\"b\"}", "{\"b\":\"c\"}", "{\"a\":\"b\",\"b\":\"c\"}"},
    {"{\"a\":\"b\"}", "{\"a\":null}", "{}"},
    {"{\"a\":\"b\",\"b\":\"c\"}", "{\"a\":null}", "{\"b\":\"c\"}"},
    {"{\"a\":[\"b\"]}", "{\"a\":\"c\"}", "{\"a\":\"c\"}"},
    {"{\"a\":\"c\"}", "{\"a\":[\"b\"]}", "{\"a\":[\"b\"]}"},
    {"{\"a\":{\"b\":\"c\"}}", "{\"a\":{\"b\":\"d\",\"c\":null}}", "{\"a\":{\"b\":\"d\"}}"},
    {"{\"a\":[{\"b\":\"c\"}]}", "{\"a\":[1]}", "{\"a\":[1]}"},
    {"[\"a\",\"b\"]", "[\"c\",\"d\"]", "[\"c\",\"d\"]"},
    {"{\"a\":\"b\"}", "[\"c\"]", "[\"c\"]"},
    {"{\"a\":\"foo\"}", "null", "null"},
    {"{\"a\":\"foo\"}", "\"bar\"", "\"bar\""},
    {"{\"e\":null}", "{\"a\":1}", "{\"e\":null,\"a\":1}"},
    {"[1,2]", "{\"a\":\"b\",\"c\":null}", "{\"a\":\"b\"}"},
    {"{}","{\"a\":{\"bb\":{\"ccc\":null}}}", "{\"a\":{\"bb\":{}}}"}
};

const char* pFullSrc=															\
		"POST /herit-in/herit-cse/TEST_HUB01/device/execute HTTP/1.1\r\n"		\
		"Host: 218.153.68.53:8080\r\n"											\
		"Accept:application/vnd.onem2m-res+json\r\n"							\
		"Content-Type: application/vnd.onem2m-res+json; ty=23\r\n"				\
		"Content-Length: 301\r\n"												\
		"X-M2M-Origin: TEST_HUB01\r\n"											\
		"X-M2M-RI: TEST_HUB01_ReqSubsciptionCreate\r\n"							\
		"\r\n"																	\
		"{\r\n"																	\
		"    \"sub\":\r\n"														\
		"    {\r\n"																\
		"        \"lbl\": [\"TEST_HUB01\", \"subscription\"],\r\n"				\
		"        \"enc\":\r\n"													\
		"        {\r\n"															\
		"            \"net\":[1,2,3,4]\r\n"										\
		"        },\r\n"														\
		"        \"nu\": [\"http://218.153.68.53:8080/herit-in/herit-cse/TEST_HUB01/pollingchannel/pcu\"],\r\n"\
		"        \"rn\": \"subscription\",\r\n"									\
		"        \"et\": \"20991231T235959\"\r\n"								\
		"    }\r\n"																\
		"}\r\n"																	\
		"\r\n";

const char* pSubSrc=															\
		"{\r\n"																	\
		"    \"test1\": \"TEST_1\",\r\n"										\
		"    \"sub\":\r\n"														\
		"    {\r\n"																\
		"        \"lbl\": [\"TEST_HUB01\", \"subscription\"],\r\n"				\
		"        \"enc\":\r\n"													\
		"        {\r\n"															\
		"            \"net\":[1,2,3,4]\r\n"										\
		"        },\r\n"														\
		"        \"nu\": [\"http://218.153.68.53:8080/herit-in/herit-cse/TEST_HUB01/pollingchannel/pcu\"],\r\n"\
		"        \"rn\": \"subscription\",\r\n"									\
		"        \"et\": \"20991231T235959\",\r\n"								\
		"        \"test2\": \"TEST_2\"\r\n"										\
		"    },\r\n"															\
		"    \"test3\": \"TEST_3\"\r\n"											\
		"}\r\n"																	\
		"\r\n";


const char *pSubKey[] = {"sub","lbl","enc","net","nu","rn","et"};

int Grib_CJsonPrintType(int iType)
{
	if(iType & cJSON_False)		GRIB_LOGD("# cJSON TYPE: FALSE\n");
	if(iType & cJSON_True)		GRIB_LOGD("# cJSON TYPE: TRUE\n");
	if(iType & cJSON_NULL)		GRIB_LOGD("# cJSON TYPE: NULL\n");
	if(iType & cJSON_Number)	GRIB_LOGD("# cJSON TYPE: NUMBER\n");
	if(iType & cJSON_String)	GRIB_LOGD("# cJSON TYPE: STRING\n");
	if(iType & cJSON_Array)		GRIB_LOGD("# cJSON TYPE: ARRAY\n");
	if(iType & cJSON_Object)	GRIB_LOGD("# cJSON TYPE: OBJECT\n");
	if(iType & cJSON_Raw)		GRIB_LOGD("# cJSON TYPE: RAW\n");

	return GRIB_DONE;
}

int Grib_TestCJson(int argc, char **argv)
{
	int i = 0;
	int iRes = GRIB_ERROR;

	int loopMain = 0;
	int loopSub = 0;

    cJSON* root = NULL;
    cJSON* parent = NULL;
    cJSON* pChild = NULL;
    cJSON* pJson = NULL;

	char* pSearch = NULL;
	char* pValue = NULL;

    GRIB_LOGD("# JSON PARSING TEST\n");
	GRIB_LOGD("# ########## ########## ########## ######### ########## ########## ##########\n");
    GRIB_LOGD("# JSON SOURCE:\n%s\n", pSubSrc);
	GRIB_LOGD("# ########## ########## ########## ######### ########## ########## ##########\n\n");

	pSearch = argv[GRIB_CMD_ARG1];
	GRIB_LOGD("# SCH-INPUT : %s\n", pSearch);

    root = cJSON_Parse(pSubSrc);
	if(root == NULL)
	{
		GRIB_LOGD("# ROOT JSON IS NULL ERROR !!!\n");
		return GRIB_ERROR;
	}
	GRIB_LOGD("# SCH-ROOT  : %p\n", root);

	pJson = cJSONUtils_GetPointer(root, pSearch);
	if(pJson == NULL)
	{
		GRIB_LOGD("# TARGET JSON IS NULL ERROR !!!\n");
		return GRIB_ERROR;
	}
	GRIB_LOGD("# SCH-JSON  : %p\n", pJson);

	GRIB_LOGD("\n");
	Grib_CJsonPrintType(pJson->type);
	GRIB_LOGD("# SCH-KEY   : %s\n", pJson->string);
	GRIB_LOGD("# SCH-STRING: %s\n", pJson->valuestring);
	GRIB_LOGD("# SCH-INT   : %d\n", pJson->valueint);
	GRIB_LOGD("# SCH-DOUBLE: %f\n", pJson->valuedouble);

	GRIB_LOGD("\n");
	pValue = cJSON_Print(pJson);
	GRIB_LOGD("# SCH-VALUE :\n%s\n", pValue);
	


/*
	parent = root;
	while(parent != NULL)
	{
		if(loopMain < 10)loopMain++;
		else break;

		pValue = cJSON_Print(parent);

		GRIB_LOGD("\n");
		GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
   		Grib_CJsonPrintType(parent->type);
   		GRIB_LOGD("# P[%d]-KEY   : %s\n", loopMain, parent->string);
   		GRIB_LOGD("# P[%d]-VALUE : %s\n", loopMain, pValue);
   		if(parent->valuestring!=NULL)GRIB_LOGD("# P[%d]-STRING: %s\n", loopMain, parent->valuestring);
   		if(parent->valueint!=0)GRIB_LOGD("# P[%d]-INT   : %d\n", loopMain, parent->valueint);
		if(parent->valuedouble!=0.0f)GRIB_LOGD("# P[%d]-DOUBLE: %f\n", loopMain, parent->valuedouble);

		if(parent->child!=NULL)
		{
			pChild = parent->child;
			GRIB_LOGD("# Parent's Child ...\n");
		}
		else pChild = NULL;


		// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
		while(pChild != NULL)
		{
			//if(loopSub < 10)loopSub++;
			//else break;

			pValue = cJSON_Print(pChild);

			GRIB_LOGD("\n");
			GRIB_LOGD("# ##### ##### ##### ##### ##### ##### ##### #####\n");
	   		Grib_CJsonPrintType(pChild->type);
	   		GRIB_LOGD("# C[%d]-KEY   : %s\n", loopSub, pChild->string);
	   		GRIB_LOGD("# C[%d]-VALUE : %s\n", loopSub, pValue);
	   		if(parent->valuestring!=NULL)GRIB_LOGD("# C[%d]-STRING: %s\n", loopSub, pChild->valuestring);
	   		if(parent->valueint!=0)GRIB_LOGD("# C[%d]-INT   : %d\n", loopSub, pChild->valueint);
	   		if(parent->valuedouble!=0.0f)GRIB_LOGD("# C[%d]-DOUBLE: %f\n", loopSub, pChild->valuedouble);

			if(pChild->child!=NULL)
			{
				pChild = pChild->child;
		   		GRIB_LOGD("# Child's Child ...\n");
				continue;
			}
			else
			{
				pChild = NULL;
			}

			if(pChild->next!=NULL)
			{
				pChild = pChild->next;
		   		GRIB_LOGD("# Child's Next ...\n");
				continue;
			}
			else
			{
				pChild = NULL;
			}

		}
		// ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####

		if(parent->next!=NULL)
		{
			parent = parent->next;
			GRIB_LOGD("# Parent's Next ...\n");
			continue;
		}
		else
		{
			parent = NULL;
		}
	}
*/

/*
    for (i = 0; i < 12; i++)
    {
        char *output = cJSON_Print(cJSONUtils_GetPointer(root, pSubKey[i]));
        printf("# TEST %d:\n%s:%s\n\n", i + 1, pSubKey[i], output);
        FREE(output);
    }
*/
    cJSON_Delete(root);

	return iRes;
}


/*
int Grib_TestMain(int argc, char **argv)
{
	int i = 0;
	int iRes = GRIB_ERROR;
	int iSize = 0;
	const char* FUNC_TAG = "TEST-MAIN";

	gIntList.reserve(100);

	for(i=1; i<=10; i+=2)gIntList.push_back(i);
	iSize = gIntList.size();
	GRIB_LOGD("# %s: 1ST REQ SIZE: %d\n", FUNC_TAG, iSize);

	for(i=2; i<=10; i+=2)gIntList.push_back(i);
	GRIB_LOGD("# %s: 2ND REQ SIZE: %d\n", FUNC_TAG, iSize);

	for(std::vector<int>::iterator req=gIntList.begin(); req!=gIntList.end(); req++)	
	{
		GRIB_LOGD("\n# ##### ##### ##### ##### ##### ##### ##### #####\n");
		GRIB_LOGD("# %s: VECTOR VALUE: %d\n", FUNC_TAG, *req);
	}

	return GRIB_DONE;


}
*/

