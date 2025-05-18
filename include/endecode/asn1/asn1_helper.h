#ifndef __ASN1_HELPER_H__
#define __ASN1_HELPER_H__

#include "endecode/asn1/asn1.h"

#define MAX_OID 64 // OID 字符串最大长度

/**
 * @brief   OID 映射结构体
 */
typedef struct OID_MAPPING
{
    char*               oid_string; // OID 字符串
    easy_asn1_string_st oid_object; // OID 对象
    char*               oid_name;   // OID 名称
} OID_MAPPING;

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
 * @brief   根据配置文件获取 oid 信息
 * @param   filename                [IN]        文件名称
 * @param   oid_mapping             [IN/OUT]    oid 结构体
 * @param   map_len                 [IN/OUT]    oid 结构体个数
 */
ENDECODE_API void ReadOid(const char* filename, OID_MAPPING** oid_mapping, size_t* map_len);

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

/**
 * @brief   计算字符串转大数
 * @param   value                   [IN]        数据
 * @param   len                     [IN]        数据长度
 * @return  char*                               字符串大数
 */
ENDECODE_API char* easy_asn1_print_integer(const uint8_t* value, size_t len);

/**
 * @brief   是否是可见字符串
 * @param   value                   [IN]        数据
 * @param   len                     [IN]        数据长度
 * @return  int                                 0: 非可见字符串; 1: 可见字符串
 */
ENDECODE_API int easy_asn1_print_string_try(const uint8_t* value, size_t len);

/**
 * @brief   打印 ASN1 结构体
 * @param   str                     [IN]        ASN1 结构体
 * @param   print_value             [IN]        0: 打印值; 其他: 不打印
 */
ENDECODE_API void easy_asn1_print_string(easy_asn1_string_st* str, size_t print_value);

/**
 * @brief   打印 ASN1 树
 * @param   node                    [IN]        树节点
 */
ENDECODE_API void easy_asn1_print_tree(easy_asn1_tree_st* node);

#ifdef __cplusplus
}
#endif

#endif