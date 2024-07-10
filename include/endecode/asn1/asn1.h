#ifndef __ASN1_H__
#define __ASN1_H__

#include <malloc.h>
#include <stdint.h>

/* ASN.1 tag values */
#define V_ASN1_EOC               0
#define V_ASN1_BOOLEAN           1
#define V_ASN1_INTEGER           2
#define V_ASN1_BIT_STRING        3
#define V_ASN1_OCTET_STRING      4
#define V_ASN1_NULL              5
#define V_ASN1_OBJECT            6
#define V_ASN1_OBJECT_DESCRIPTOR 7
#define V_ASN1_EXTERNAL          8
#define V_ASN1_REAL              9
#define V_ASN1_ENUMERATED        10
#define V_ASN1_UTF8STRING        12
#define V_ASN1_SEQUENCE          16
#define V_ASN1_SET               17
#define V_ASN1_NUMERICSTRING     18
#define V_ASN1_PRINTABLESTRING   19 /* printable subset of ascii */
#define V_ASN1_T61STRING         20
#define V_ASN1_TELETEXSTRING     20 /* alias */
#define V_ASN1_VIDEOTEXSTRING    21
#define V_ASN1_IA5STRING         22 /* 7-bit ascii */
#define V_ASN1_UTCTIME           23
#define V_ASN1_GENERALIZEDTIME   24
#define V_ASN1_GRAPHICSTRING     25
#define V_ASN1_ISO64STRING       26
#define V_ASN1_VISIBLESTRING     26 /* alias */
#define V_ASN1_GENERALSTRING     27
#define V_ASN1_UNIVERSALSTRING   28
#define V_ASN1_BMPSTRING         30 /* 2-byte unicode with zeros */

struct asn1_string_st
{
    uint8_t  tag;
    uint32_t length;
    uint8_t* value;
};

struct asn1_tree_st
{
    struct asn1_string_st value;
    uint32_t              level;
    uint32_t              children_size;
    struct asn1_tree_st** children;
};

const char*    asn1_tag_name(int tag);
const uint8_t* asn1_parse_tag(const uint8_t* data, uint8_t* tag);
const uint8_t* asn1_parse_length(const uint8_t* data, uint32_t* length);

struct asn1_tree_st* asn1_create_node(uint8_t type, uint32_t length, uint8_t* value, uint32_t level);

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

ENDECODE_API const uint8_t*       asn1_parse_string(const uint8_t* data, struct asn1_string_st* str);
ENDECODE_API struct asn1_tree_st* asn1_parse(const uint8_t* data, size_t len, int level);
ENDECODE_API void                 asn1_free_tree(struct asn1_tree_st* node);

#ifdef __cplusplus
}
#endif

#endif