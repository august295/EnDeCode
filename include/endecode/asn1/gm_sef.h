#ifndef __GM_SEF_H__
#define __GM_SEF_H__

#include "endecode/asn1/asn1.h"

/**
 * @brief   常量定义
 */
#define BUF_SIZE      256  // 通用字节长度
#define MAX_CERT_SIZE 5120 // 数字证书最大长度

enum CertListType
{
    CLT_CERT      = 1,
    CLT_CERT_HASH = 2,
};

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

ENDECODE_API void SEF_ParseSeal(const uint8_t* data, size_t len, SEALINFO** seal);

#ifdef __cplusplus
}
#endif

#endif
