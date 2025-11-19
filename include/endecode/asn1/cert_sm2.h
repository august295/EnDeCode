#ifndef __CERT_SM2_H__
#define __CERT_SM2_H__

#include "endecode/asn1/asn1.h"
#include "endecode/asn1/cert.h"

/**
 * @brief   SOF证书解析项标识
 */
#define SGD_CERT_VERSION                         0x00000001 // 证书版本
#define SGD_CERT_SERIAL                          0x00000002 // 证书序列号
#define SGD_CERT_SIGNATURE_ALGORITHM             0x00000003 // 证书签名算法
#define SGD_CERT_PUBLIC_KEY_PARAMETERS           0x00000004 // 证书公钥参数
#define SGD_CERT_ISSUER                          0x00000005 // 证书颁发者信息
#define SGD_CERT_VALID_TIME                      0x00000006 // 证书有效期
#define SGD_CERT_SUBJECT                         0x00000007 // 证书拥有者信息
#define SGD_CERT_DER_PUBLIC_KEY                  0x00000008 // 证书公钥信息
#define SGD_CERT_DER_EXTENSIONS                  0x00000009 // 证书扩展项信息
#define SGD_EXT_AUTHORITYKEYIDENTIFIER_INFO      0x00000011 // 颁发者密钥标识符
#define SGD_EXT_SUBJECTKEYIDENTIFIER_INFO        0x00000012 // 证书持有者密钥标识符
#define SGD_EXT_KEYUSAGE_INFO                    0x00000013 // 密钥用途
#define SGD_EXT_PRIVATEKEYUSAGEPERIOD_INFO       0x00000014 // 私钥有效期
#define SGD_EXT_CERTIFICATEPOLICIES_INFO         0x00000015 // 证书策略
#define SGD_EXT_POLICYMAPPINGS_INFO              0x00000016 // 策略映射
#define SGD_EXT_BASICCONSTRAINTS_INFO            0x00000017 // 基本限制
#define SGD_EXT_POLICYCONSTRAINTS_INFO           0x00000018 // 策略限制
#define SGD_EXT_EXTKEYUSAGE_INFO                 0x00000019 // 扩展密钥用途
#define SGD_EXT_CRLDISTRIBUTIONPOINTS_INFO       0x0000001A // CRL发布点
#define SGD_EXT_NETSCAPE_CERT_TYPE_INFO          0x0000001B // Netscape属性
#define SGD_EXT_SELFDEFINED_EXTENSION_INFO       0x0000001C // 扩展密钥用途
#define SGD_EXT_SUBHECT_ALT_NAME                 0x0000001D // 主体替换名称
#define SGD_EXT_ISSUER_ALT_NAME                  0x0000001E // 颁发者替换名称
#define SGD_EXT_FRESHEST_CRL                     0x0000001F // 最新证书撤销列表
#define SGD_CERT_ISSUER_CN                       0x00000021 // 证书颁发者CN
#define SGD_CERT_ISSUER_O                        0x00000022 // 证书颁发者O
#define SGD_CERT_ISSUER_OU                       0x00000023 // 证书颁发者OU
#define SGD_CERT_SUBJECT_CN                      0x00000031 // 证书拥有者信息CN
#define SGD_CERT_SUBJECT_O                       0x00000032 // 证书拥有者信息O
#define SGD_CERT_SUBJECT_OU                      0x00000033 // 证书拥有者信息OU
#define SGD_CERT_SUBJECT_EMAIL                   0x00000034 // 证书拥有者信息EMAIL
#define SGD_CERT_NOTBEFORE_TIME                  0x00000035 // 证书起始日期
#define SGD_CERT_NOTAFTER_TIME                   0x00000036 // 证书结束日期
#define SGD_EXT_IDENTIFY_CODE                    0x00000037 // 个人身份标识码
#define SGD_EXT_INSURANCE_NUMBER                 0x00000038 // 个人社会保险号
#define SGD_EXT_UNIFORM_SOCIAL_CREDIT_INENTIFIER 0x00000039 // 统一社会信用代码
#define SGD_EXT_AUTHORITY_INFO_ACCESS            0x0000003A // 机构信息访问
#define SGD_EXT_SUBHECT_INFORMATION_ACCESS       0x0000003B // 主体信息访问

// 定义算法标识
typedef struct AlgorithmIdentifier
{
    easy_asn1_string_st algorithm;  // 算法
    easy_asn1_string_st parameters; // 扩展参数
} AlgorithmIdentifier;

// 定义属性类型和值
typedef struct AttributeTypeAndValue
{
    easy_asn1_string_st type;  // 属性类型
    easy_asn1_string_st value; // 属性值
} AttributeTypeAndValue;

// 定义相对可分辨名称（RDN）
typedef struct RelativeDistinguishedName
{
    AttributeTypeAndValue* rdn;   // RDN数组
    uint32_t               count; // RDN数量
} RelativeDistinguishedName;

// 定义名称（DN）
typedef struct Name
{
    RelativeDistinguishedName* names; // DN数组
    uint32_t                   count; // DN数量
} Name;

// 定义有效期
typedef struct Validity
{
    easy_asn1_string_st notBefore; // 生效时间
    easy_asn1_string_st notAfter;  // 失效时间
} Validity;

// 定义公钥信息
typedef struct SubjectPublicKeyInfo
{
    AlgorithmIdentifier algorithm;        // 公钥算法标识
    easy_asn1_string_st subjectPublicKey; // 公钥值（DER编码）
} SubjectPublicKeyInfo;

// 定义证书主体（TBS）
typedef struct TBSCertificate
{
    easy_asn1_string_st  version;              // 版本号
    easy_asn1_string_st  serialNumber;         // 证书序列号
    AlgorithmIdentifier  signature;            // 签名算法标识
    Name                 issuer;               // 颁发者名称
    Validity             validity;             // 有效期
    Name                 subject;              // 主体名称
    SubjectPublicKeyInfo subjectPublicKeyInfo; // 主体公钥信息
    easy_asn1_string_st  issuerUniqueId;       // 颁发者唯一标识符
    easy_asn1_string_st  subjectUniqueId;      // 主体唯一标识符
    easy_asn1_string_st  extensions;           // 扩展项
} TBSCertificate;

// 定义证书结构
typedef struct SM2Certificate
{
    TBSCertificate      tbsCertificate;     // 基本证书域
    AlgorithmIdentifier signatureAlgorithm; // 签名算法域
    easy_asn1_string_st signatureValue;     // 签名值域
} SM2Certificate;

#include "endecode/common/endecode_export.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   初始化证书结构体
 * @param   cert                    [IN]        证书结构体
 */
ENDECODE_API void sm2_cert_init(SM2Certificate* cert);

/**
 * @brief   释放证书结构体
 * @param   cert                    [IN]        证书结构体
 */
ENDECODE_API void sm2_cert_free(SM2Certificate* cert);

/**
 * @brief   解析证书
 * @param   data                    [IN]        二进制证书数据
 * @param   len                     [IN]        二进制证书数据长度
 * @return  SM2Certificate*                     证书结构体（可能部分解析成功）
 *          NULL                                解析失败
 */
ENDECODE_API SM2Certificate* sm2_cert_parse(const uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif

#endif