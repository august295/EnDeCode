#ifndef __ASN1_HELPER_H__
#define __ASN1_HELPER_H__

#include "endecode/asn1/asn1.h"

#define MAX_OID 64

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   商用密码 OID 转换为字符串
 * @param   oid_bytes               [IN]        二进制 OID
 * @param   oid_len                 [IN]        二进制 OID 长度
 * @param   oid_str                 [IN/OUT]    字符串 OID
 */
ENDECODE_API void oid_to_string(const uint8_t* oid_bytes, size_t oid_len, char* oid_str);

/**
 * @brief   商用密码字符串转换为 OID
 * @param   oid_str                 [IN]        字符串 OID
 * @param   oid_bytes               [IN/OUT]    二进制 OID
 * @param   oid_len                 [IN/OUT]    二进制 OID 长度
 * @return  0                                   成功
 *          其他                                失败
 */
ENDECODE_API int string_to_oid(const char* oid_str, uint8_t* oid_bytes, size_t* oid_len);

/**
 * @brief   UTCTime时间格式转换为标准时间格式
 * @param   utcTime                 [IN]        utc 时间格式
 * @param   utcOffset               [IN]        utc 时区偏移
 * @param   standardTime            [IN/OUT]    标准时间格式
 */
ENDECODE_API void convertUTCTimeToStandard(const char* utcTime, size_t utcOffset, char* standardTime);

/**
 * @brief   GeneralizedTime时间格式转换为标准时间格式
 * @param   generalizedTime         [IN]        GeneralizedTime 时间格式
 * @param   standardTime            [IN/OUT]    标准时间格式
 */
ENDECODE_API void convertGeneralizedTimeToStandard(const char* generalizedTime, size_t utcOffset, char* standardTime);

ENDECODE_API void easy_asn1_print_integer(const uint8_t* value, size_t len);
ENDECODE_API int  easy_asn1_print_string_try(const uint8_t* value, size_t len);
ENDECODE_API void easy_asn1_print_string(easy_asn1_string_st* str, size_t print_value);
ENDECODE_API void easy_asn1_print_tree(easy_asn1_tree_st* node);

#ifdef __cplusplus
}
#endif

#endif