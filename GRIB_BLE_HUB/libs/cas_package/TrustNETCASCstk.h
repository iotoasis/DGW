#ifndef TRUSTNETCASCSTK_H
#define TRUSTNETCASCSTK_H

#if defined (WIN32)
	#if defined (TRUSTNETCASCSTK_EXPORTS)
		#define TRUSTNETCASCSTK_API __declspec(dllexport)
	#else
		#define TRUSTNETCASCSTK_API __declspec(dllimport)
	#endif
#elif defined(LINUX) || (linux) || (AIX)
	//  GCC
	#if defined (TRUSTNETCASCSTK_EXPORTS)
		#define TRUSTNETCASCSTK_API __attribute__((visibility("default")))
	#else
		#define TRUSTNETCASCSTK_API
	#endif
#else
	#define TRUSTNETCASCSTK_API
#endif

#ifdef __cplusplus
	extern "C" {
#endif
		TRUSTNETCASCSTK_API int TK_Init(char *szConfPath);
		TRUSTNETCASCSTK_API int TK_IssueCert(char *szDevId);
		TRUSTNETCASCSTK_API int TK_Sign(char *szDevId, char * szSignSrc, char **ppAuthKey);
		TRUSTNETCASCSTK_API int TK_Final();
#ifdef __cplusplus
}
#endif


#endif