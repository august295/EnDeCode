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
#define EASY_ASN1_EMBEDDED_PDV      0xB
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

enum Asn1TagClass
{
    UNIVERSAL        = 0,   // 0x00
    CONSTRUCTED      = 32,  // 0x20
    APPLICATION      = 64,  // 0x40
    CONTEXT_SPECIFIC = 128, // 0x80
    PRIVATE          = 192, // 0xc0
};

// a list of construct tags. Source: http://en.wikipedia.org/wiki/Distinguished_Encoding_Rules#DER_encoding
static uint8_t ConstructTags[] = {
    EASY_ASN1_BIT_STRING,
    EASY_ASN1_OCTET_STRING,
    EASY_ASN1_OBJECT_DESCRIPTOR,
    EASY_ASN1_EXTERNAL,
    EASY_ASN1_EMBEDDED_PDV,
    EASY_ASN1_UTF8STRING,
    EASY_ASN1_SEQUENCE,
    EASY_ASN1_SET,
    EASY_ASN1_NUMERICSTRING,
    EASY_ASN1_PRINTABLESTRING,
    EASY_ASN1_T61STRING,
    EASY_ASN1_TELETEXSTRING,
    EASY_ASN1_VIDEOTEXSTRING,
    EASY_ASN1_IA5STRING,
    EASY_ASN1_UTCTIME,
    EASY_ASN1_GENERALIZEDTIME,
    EASY_ASN1_GRAPHICSTRING,
    EASY_ASN1_ISO64STRING,
    EASY_ASN1_VISIBLESTRING,
    EASY_ASN1_GENERALSTRING,
    EASY_ASN1_UNIVERSALSTRING,
    EASY_ASN1_CHARACTER_STRING,
    EASY_ASN1_BMPSTRING,
    EASY_ASN1_SEQUENCE | CONSTRUCTED,
    EASY_ASN1_SET | CONSTRUCTED,
};

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
    struct easy_asn1_tree_st*  first_child;
    struct easy_asn1_tree_st*  parent;
    struct easy_asn1_tree_st*  prev_sibling;
    struct easy_asn1_tree_st*  next_sibling;
} easy_asn1_tree_st;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * @brief   ASN1 初始化和释放
 *****************************************************************************/
ENDECODE_API void easy_asn1_init_string(easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_init_tree(easy_asn1_tree_st* node);
ENDECODE_API void easy_asn1_free_string(easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_free_tree(easy_asn1_tree_st* node);

/******************************************************************************
 * @brief   ASN1 工具函数
 *****************************************************************************/
ENDECODE_API void easy_asn1_copy_string(easy_asn1_string_st* src, easy_asn1_string_st* dest);

ENDECODE_API easy_asn1_tree_st* easy_asn1_get_tree_item(const easy_asn1_tree_st* node, size_t index);

/******************************************************************************
 * @brief   ASN1 解析
 *****************************************************************************/
ENDECODE_API char*  easy_asn1_tag_name(uint8_t tag);
ENDECODE_API size_t easy_asn1_parse_tag(const uint8_t* data, uint8_t* tag);
ENDECODE_API size_t easy_asn1_parse_length(const uint8_t* data, size_t data_len, size_t* length);
ENDECODE_API size_t easy_asn1_parse_string(const uint8_t* data, size_t data_len, easy_asn1_string_st* str);
ENDECODE_API size_t easy_asn1_parse_construct(uint8_t tag);
ENDECODE_API size_t easy_asn1_parse_predict(const uint8_t* data, size_t data_len);
ENDECODE_API void   easy_asn1_parse(const uint8_t* data, size_t data_len, size_t offset, size_t level, easy_asn1_tree_st** node);

/******************************************************************************
 * @brief   ASN1 创建
 *****************************************************************************/
ENDECODE_API void easy_asn1_create_string(uint8_t tag, size_t length, uint8_t* value, easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_create_node(uint8_t tag, size_t length, uint8_t* value, easy_asn1_tree_st* node);
ENDECODE_API void easy_asn1_push_back_child(easy_asn1_tree_st* node, uint8_t tag, size_t length, uint8_t* value);
ENDECODE_API void easy_asn1_push_back_string_child(easy_asn1_tree_st* node, easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_push_back_tree_child(easy_asn1_tree_st* node, easy_asn1_tree_st* node_child);
ENDECODE_API void easy_asn1_insert_child(easy_asn1_tree_st* node, size_t index, uint8_t tag, size_t length, uint8_t* value);
ENDECODE_API void easy_asn1_insert_string_child(easy_asn1_tree_st* node, size_t index, easy_asn1_string_st* str);
ENDECODE_API void easy_asn1_insert_tree_child(easy_asn1_tree_st* node, size_t index, easy_asn1_tree_st* node_child);
ENDECODE_API void easy_asn1_update_length(easy_asn1_tree_st* node, int length);
ENDECODE_API void easy_asn1_update_offset(easy_asn1_tree_st* node, int offset);
ENDECODE_API void easy_asn1_update_offset_child(easy_asn1_tree_st* node, int offset);
ENDECODE_API void easy_asn1_update_level_child(easy_asn1_tree_st* node);

/******************************************************************************
 * @brief   ASN1 序列化
 *****************************************************************************/
ENDECODE_API size_t easy_asn1_encode_length(size_t length, uint8_t* out);
ENDECODE_API size_t easy_asn1_serialize_string(easy_asn1_string_st* str, uint8_t* buffer);
ENDECODE_API size_t easy_asn1_serialize(easy_asn1_tree_st* node, uint8_t* buffer);

#ifdef __cplusplus
}
#endif

#endif