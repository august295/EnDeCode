#ifndef __ASN1_H__
#define __ASN1_H__

#include <stdint.h>

/* ASN.1 tag values */
#define EASY_ASN1_EOC               0x0
#define EASY_ASN1_BOOLEAN           0x1
#define EASY_ASN1_INTEGER           0x2
#define EASY_ASN1_BIT_STRING        0x3
#define EASY_ASN1_OCTET_STRING      0x4
#define EASY_ASN1_NULL              0x5
#define EASY_ASN1_OBJECT            0x6
#define EASY_ASN1_OBJECT_DESCRIPTOR 0x7
#define EASY_ASN1_EXTERNAL          0x8
#define EASY_ASN1_REAL              0x9
#define EASY_ASN1_ENUMERATED        0xA
#define EASY_ASN1_EMBEDDED_DV       0xB
#define EASY_ASN1_UTF8STRING        0xC
#define EASY_ASN1_RELATIVE_OID      0xD
#define EASY_ASN1_TIME              0xE
#define EASY_ASN1_SEQUENCE          0x10
#define EASY_ASN1_SET               0x11
#define EASY_ASN1_NUMERICSTRING     0x12
#define EASY_ASN1_PRINTABLESTRING   0x13 /* printable subset of ascii */
#define EASY_ASN1_T61STRING         0x14
#define EASY_ASN1_TELETEXSTRING     0x14 /* alias */
#define EASY_ASN1_VIDEOTEXSTRING    0x15
#define EASY_ASN1_IA5STRING         0x16 /* 7-bit ascii */
#define EASY_ASN1_UTCTIME           0x17
#define EASY_ASN1_GENERALIZEDTIME   0x18
#define EASY_ASN1_GRAPHICSTRING     0x19
#define EASY_ASN1_ISO64STRING       0x1A
#define EASY_ASN1_VISIBLESTRING     0x1A /* alias */
#define EASY_ASN1_GENERALSTRING     0x1B
#define EASY_ASN1_UNIVERSALSTRING   0x1C
#define EASY_ASN1_CHARACTER_STRING  0x1D
#define EASY_ASN1_BMPSTRING         0x1E /* 2-byte unicode with zeros */
#define EASY_ASN1_TIME_OF_DAY       0x20
#define EASY_ASN1_DATE_TIME         0x21
#define EASY_ASN1_DURATION          0x22
#define EASY_ASN1_OID_IRI           0x23
#define EASY_ASN1_RELATIVE_OID_IRI  0x24
/* x509 and spki use these */
#define ESAY_X509_ASN1_SEQUENCE     0x30
#define ESAY_X509_ASN1_SET          0x31
#define ESAY_X509_CONTEXT0          0xA0 // EXPLICIT Version DEFAULT v1
#define ESAY_X509_CONTEXT1          0xA1 // IMPLICIT Issuser UniqueIdentifier OPTIONAL
#define ESAY_X509_CONTEXT2          0xA2 // IMPLICIT Subject UniqueIdentifier OPTIONAL
#define ESAY_X509_CONTEXT3          0xA3 // EXPLICIT Extensions OPTIONAL

// clang-format off
// a list of primitive tags. Source: http://en.wikipedia.org/wiki/Distinguished_Encoding_Rules#DER_encoding
static uint8_t PrimitiveTags[] = {
    EASY_ASN1_EOC,            EASY_ASN1_BOOLEAN,         EASY_ASN1_INTEGER,          EASY_ASN1_NULL,            EASY_ASN1_OBJECT,          EASY_ASN1_REAL,
    EASY_ASN1_ENUMERATED,     EASY_ASN1_UTF8STRING,      EASY_ASN1_RELATIVE_OID,     EASY_ASN1_NUMERICSTRING,   EASY_ASN1_PRINTABLESTRING, EASY_ASN1_T61STRING,
    EASY_ASN1_VIDEOTEXSTRING, EASY_ASN1_IA5STRING,       EASY_ASN1_UTCTIME,          EASY_ASN1_GENERALIZEDTIME, EASY_ASN1_GRAPHICSTRING,   EASY_ASN1_GRAPHICSTRING,
    EASY_ASN1_GENERALSTRING,  EASY_ASN1_UNIVERSALSTRING, EASY_ASN1_CHARACTER_STRING, EASY_ASN1_BMPSTRING
};
// clang-format on

typedef struct easy_asn1_string_st
{
    uint8_t  tag;
    size_t   length;
    uint8_t* value;
} easy_asn1_string_st;

typedef struct easy_asn1_tree_st
{
    struct easy_asn1_string_st value;
    size_t                     offset;
    size_t                     level;
    size_t                     children_size;
    struct easy_asn1_tree_st** children;
} easy_asn1_tree_st;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C" {
#endif

ENDECODE_API void easy_asn1_init_string(easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_init_tree(easy_asn1_tree_st* node);

ENDECODE_API void easy_asn1_create_string(uint8_t tag, size_t length, uint8_t* value, easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_copy_string(easy_asn1_string_st* src, easy_asn1_string_st* dest);

ENDECODE_API void easy_asn1_free_string(easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_free_tree(easy_asn1_tree_st* node);

ENDECODE_API void easy_asn1_print_string(easy_asn1_string_st* str, size_t print_value);
ENDECODE_API void easy_asn1_print_tree(easy_asn1_tree_st* node);

ENDECODE_API char*  easy_asn1_tag_name(uint8_t tag);
ENDECODE_API size_t easy_asn1_parse_tag(const uint8_t* data, uint8_t* tag);
ENDECODE_API size_t easy_asn1_parse_length(const uint8_t* data, size_t* length);
ENDECODE_API size_t easy_asn1_parse_string(const uint8_t* data, easy_asn1_string_st* str);
ENDECODE_API size_t easy_asn1_parse_primit(uint8_t tag);
ENDECODE_API size_t easy_asn1_parse_predict(const uint8_t* data, size_t start, size_t length);
ENDECODE_API void   easy_asn1_parse(const uint8_t* data, size_t len, size_t offset, size_t level, easy_asn1_tree_st** node);

ENDECODE_API size_t easy_asn1_encode_length(size_t length, uint8_t* out);
ENDECODE_API size_t easy_asn1_serialize(easy_asn1_tree_st* node, uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif