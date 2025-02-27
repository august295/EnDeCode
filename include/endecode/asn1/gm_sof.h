#ifndef __GM_SOF_H__
#define __GM_SOF_H__

#include "endecode/asn1/cert_sm2.h"

typedef char*   BSTR;
typedef int32_t LONG;
typedef int16_t SHORT;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C" {
#endif

ENDECODE_API BSTR SOF_GetCertInfo(BSTR Base64Cert, SHORT Type);

#ifdef __cplusplus
}
#endif

#endif