#ifndef __ZUC_H__
#define __ZUC_H__

#include <stdint.h>

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct zuc_context
{
    /* the state registers of LFSR */
    uint32_t LFSR_S[16];
    /* the registers of F */
    uint32_t F_R1;
    uint32_t F_R2;
    /* the outputs of BitReorganization */
    uint32_t BRC_X[4];
} zuc_context;

/**
 * @brief 根据密钥和初始值进行初始化
 * @param[in,out] ctx            变化值
 * @param[in]     k              16x8bit 密钥
 * @param[in]     iv             16x8bit 初始值
 */
ENDECODE_API void zuc_Initialization(zuc_context* ctx, uint8_t* k, uint8_t* iv);

/**
 * @brief 生成密钥流
 * @param[in,out] ctx            变化值
 * @param[out]    pKeystream     密钥流
 * @param[in]     KeystreamLen   密钥流长度
 */
ENDECODE_API void zuc_GenerateKeystream(zuc_context* ctx, uint32_t* pKeystream, int KeystreamLen);

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