#ifndef __ASN1_H__
#define __ASN1_H__

#include <stdint.h>

/* ASN.1 tag values */
#define EASY_ASN1_EOC               0
#define EASY_ASN1_BOOLEAN           1
#define EASY_ASN1_INTEGER           2
#define EASY_ASN1_BIT_STRING        3
#define EASY_ASN1_OCTET_STRING      4
#define EASY_ASN1_NULL              5
#define EASY_ASN1_OBJECT            6
#define EASY_ASN1_OBJECT_DESCRIPTOR 7
#define EASY_ASN1_EXTERNAL          8
#define EASY_ASN1_REAL              9
#define EASY_ASN1_ENUMERATED        10
#define EASY_ASN1_UTF8STRING        12
#define EASY_ASN1_SEQUENCE          16
#define EASY_ASN1_SET               17
#define EASY_ASN1_NUMERICSTRING     18
#define EASY_ASN1_PRINTABLESTRING   19 /* printable subset of ascii */
#define EASY_ASN1_T61STRING         20
#define EASY_ASN1_TELETEXSTRING     20 /* alias */
#define EASY_ASN1_VIDEOTEXSTRING    21
#define EASY_ASN1_IA5STRING         22 /* 7-bit ascii */
#define EASY_ASN1_UTCTIME           23
#define EASY_ASN1_GENERALIZEDTIME   24
#define EASY_ASN1_GRAPHICSTRING     25
#define EASY_ASN1_ISO64STRING       26
#define EASY_ASN1_VISIBLESTRING     26 /* alias */
#define EASY_ASN1_GENERALSTRING     27
#define EASY_ASN1_UNIVERSALSTRING   28
#define EASY_ASN1_BMPSTRING         30 /* 2-byte unicode with zeros */
/* x509 and spki use these */
#define ESAY_X509_ASN1_SEQUENCE     0x30
#define ESAY_X509_ASN1_SET          0x31
#define ESAY_X509_CONTEXT0          0xA0 // EXPLICIT Version DEFAULT v1
#define ESAY_X509_CONTEXT1          0xA1 // IMPLICIT Issuser UniqueIdentifier OPTIONAL
#define ESAY_X509_CONTEXT2          0xA2 // IMPLICIT Subject UniqueIdentifier OPTIONAL
#define ESAY_X509_CONTEXT3          0xA3 // EXPLICIT Extensions OPTIONAL

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
ENDECODE_API void   easy_asn1_parse(const uint8_t* data, size_t len, size_t offset, size_t level, easy_asn1_tree_st** node);

ENDECODE_API size_t easy_asn1_encode_length(size_t length, uint8_t* out);
ENDECODE_API size_t easy_asn1_serialize(easy_asn1_tree_st* node, uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif