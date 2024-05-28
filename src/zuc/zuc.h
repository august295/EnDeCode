#ifndef __ZUC_H__
#define __ZUC_H__

#include <stdint.h>

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 根据密钥和初始值进行初始化
 * @param[in]     k              16x8bit 密钥
 * @param[in]     iv             16x8bit 初始值
 */
ENDECODE_API void Initialization(uint8_t* k, uint8_t* iv);

/**
 * @brief 生成密钥流
 * @param[out]    pKeystream     密钥流
 * @param[in]     KeystreamLen   密钥流长度
 */
ENDECODE_API void GenerateKeystream(uint32_t* pKeystream, int KeystreamLen);

/**
 * @brief 祖冲之算法生成密钥流
 * @param[in]     k              16x8bit 密钥
 * @param[in]     iv             16x8bit 初始值
 * @param[out]    ks             密钥流
 * @param[in]     len            密钥流长度
 */
ENDECODE_API void ZUC(uint8_t* k, uint8_t* iv, uint32_t* ks, int len);

#ifdef __cplusplus
}
#endif

#endif