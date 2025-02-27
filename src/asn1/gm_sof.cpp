#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <stdio.h>

#include "endecode/base64/base64.h"
#include "endecode/asn1/gm_sof.h"

BSTR SOF_GetCertInfo(BSTR Base64Cert, SHORT Type)
{
    size_t   Base64CertLen = strlen(Base64Cert);
    size_t   BinCertLen    = BASE64_DECODE_OUT_SIZE(Base64CertLen);
    uint8_t* BinCert       = (uint8_t*)calloc(1, BinCertLen + 1);
    base64_decode(Base64Cert, Base64CertLen, BinCert);

    SM2Certificate* cert = sm2_cert_parse(BinCert, BinCertLen);
    BSTR            info = NULL;
    switch (Type)
    {
    case SGD_CERT_VERSION: 
        info  = (BSTR)calloc(1, cert->tbsCertificate.version.length + 1);
        snprintf(info, cert->tbsCertificate.version.length + 1, "%d", cert->tbsCertificate.version.value[0]);
        break;
    default:
        break;
    }
    sm2_cert_free(cert);

    return info;
}