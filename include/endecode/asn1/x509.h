#ifndef __X509_H__
#define __X509_H__

#include "endecode/asn1/asn1.h"

#define V_X509_ASN1_SEQUENCE 0x30
#define V_X509_ASN1_SET      0x31
#define V_X509_CONTEXT0      0xA0 // EXPLICIT Version DEFAULT v1
#define V_X509_CONTEXT1      0xA1 // IMPLICIT Issuser UniqueIdentifier OPTIONAL
#define V_X509_CONTEXT2      0xA2 // IMPLICIT Subject UniqueIdentifier OPTIONAL
#define V_X509_CONTEXT3      0xA3 // EXPLICIT Extensions OPTIONAL

typedef struct asn1_string_st x509_version;
typedef struct asn1_string_st x509_algorithm;
typedef struct asn1_string_st x509_value;

const char* x509_tag_name(int tag);

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

ENDECODE_API struct asn1_tree_st* x509_parse_cert(const uint8_t* data, size_t len, int level);

#ifdef __cplusplus
}
#endif

#endif