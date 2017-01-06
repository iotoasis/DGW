#ifndef __GRIB_CAS_H__
#define __GRIB_CAS_H__

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Include File
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#include <sys/param.h>

#include "grib_define.h"
#include "grib_util.h"
//#include "grib_onem2m.h"

#include "TrustNETCASCstk.h"
#include "TrustNETCASClient.h"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Define
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
#define CAS_LIB_DIR_PATH							"libs/cas_package"
#define CAS_FIX_AUTH_KEY_SIZE						16 //shbaek: From UNet
#define CAS_AUTH_KEY_SIZE							128 //shbaek: Just In Case ...

#define CAS_SIGN_CERT_FILE_DIR 					"UNETsystem/TrustNETCAS/CertStorage/USER"
#define CAS_SIGN_CERT_FILE_NAME 					"signCert.pem"

#define CAS_SIGN_CERT_BEGIN						"-----BEGIN CERTIFICATE-----"
#define CAS_SIGN_CERT_END							"-----END CERTIFICATE-----"

/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
//shbaek: Function Prototype
/* ********** ********** ********** ********** ********** ********** ********** ********** ********** ********** */
void Grib_CasTest(int argc, char **argv);

int Grib_CasInit(char* hubID);
int	Grib_CasFinal(void);

int Grib_CasGetSignCert(char* pSignCert);
int Grib_CasGetAuthKey(char* devID, char* keyBuff);

#endif
