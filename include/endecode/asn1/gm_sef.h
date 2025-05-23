#ifndef __GM_SEF_H__
#define __GM_SEF_H__

#include "endecode/asn1/asn1.h"

/**
 * @brief   常量定义
 */
#define BUF_SIZE          256   // 通用字节长度
#define MAX_CERT_SIZE     5120  // 数字证书最大长度
#define MAX_SEALDATA_SIZE 51200 // 印章数据最大长度

enum CertListType
{
    CLT_CERT      = 1, // 证书
    CLT_CERT_HASH = 2, // 证书杂凑值
};

// B.5 签章信息数据结构
typedef struct SEF_SIGNEDVALUEINFO
{
    int            version;                     // 签章数据版本
    unsigned char  sealData[MAX_SEALDATA_SIZE]; // DER 编码的电子印章数据
    int            sealDataLen;                 // 电子印章数据长度
    char           signDateTime[BUF_SIZE];      // 签章时间，GeneralizedTime 时间格式
    unsigned char  dataHash[32];                // 原文杂凑值二进制数据
    int            dataHashLen;                 // 原文杂凑数据长度
    unsigned char  signerCert[MAX_CERT_SIZE];   // DER 编码的签章人证书，如果签名使用 SM2 算法，应符合 GB/T 20518 的规定
    int            signerCertLen;               // 签章人证书数据长度
    char           signatureAlgOID[32];         // 签名算法 OID，应符合 GB/T 33560 的规定
    unsigned char* propertyInfo;                // UTF-8 编码原文属性信息，如果存在，应解析
    int            propertyInfoLen;             // 原文属性信息长度
    unsigned char* timeStamp;                   // DER 编码的时间戳，应符合 GB/T 20520 的规定
    int            timeStampLen;                // 时间戳数据长度
} SIGNEDVALUEINFO, *PSIGNEDVALUEINFO;

// B.6 印章信息数据结构
typedef struct SEF_SEALINFO
{
    char           sealID[BUF_SIZE];         // 印章标识，对应 GB/T 38540 的 esID
    int            version;                  // 印章版本
    char           verderID[BUF_SIZE];       // 厂商标识，对应 GB/T 38540 的 Vid
    int            type;                     // 印章类型，应符合 GB/T 38540 的规定
    char           name[BUF_SIZE];           // UTF-8 编码的印章名称
    int            certListType;             // 签章者证书信息类型，1：证书；2：证书杂凑值
    unsigned char* certListInfo;             // DER 编码的签章者证书列表信息，证书列表信息采用 sequence 结构封装
    int            certListInfoLen;          // 签章者证书列表信息数据长度
    char           validStart[32];           // 有效起始时间，GeneralizedTime 时间格式
    char           validEnd[32];             // 有效结束时间，GeneralizedTime 时间格式
    char           createDate[32];           // 制章日期，GeneralizedTime 时间格式
    unsigned char  makerCert[MAX_CERT_SIZE]; // DER 编码的制章人数字证书
    int            makerCertLen;             // 制章人数字证书数据长度
    char           signatureAlgOID[32];      // 签名算法 OID 标识，应符合 GB/T 33560 的规定
    unsigned char* imageData;                // 印章图像二进制数据
    int            imageDataLen;             // 印章图像数据长度
    char           imageType[16];            // 印章图像类型，支持 PNG BMP JPG
    int            imageHeight;              // 印章图像高度，单位毫米
    int            imageWidth;               // 印章图像宽度，单位毫米
} SEALINFO, *PSEALINFO;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   解析印章
 * @param   data                    [IN]        二进制数据
 * @param   len                     [IN]        二进制数据长度
 * @param   seal                    [IN/OUT]    印章结构体
 */
ENDECODE_API void SEF_ParseSeal(const easy_asn1_tree_st* tree, SEALINFO** seal);

/**
 * @brief   解析签章
 * @param   data                    [IN]        二进制数据
 * @param   len                     [IN]        二进制数据长度
 * @param   seal                    [IN/OUT]    签章结构体
 */
ENDECODE_API void SEF_ParseSignatures(const easy_asn1_tree_st* tree, SIGNEDVALUEINFO** signatures);

#ifdef __cplusplus
}
#endif

#endif
