#ifndef __SNOW_3G_H__
#define __SNOW_3G_H__

#include <stdint.h>

#include "endecode/common/endecode_export.h"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct snow_3g_context
{
    /* LFSR */
    uint32_t LFSR_S[16];
    /* FSM */
    uint32_t FSM_R1;
    uint32_t FSM_R2;
    uint32_t FSM_R3;
} snow_3g_context;

/**
 * @brief 根据密钥和初始值进行初始化
 * @param[in,out] ctx            变化值
 * @param[in]     k              4x32bit 密钥
 * @param[in]     IV             4x32bit 初始值
 */
ENDECODE_API void Initialize(snow_3g_context* ctx, uint32_t k[4], uint32_t IV[4]);

/**
 * @brief 生成密钥流
 * @param[in]     ctx            变化值
 * @param[in]     n              密钥流长度
 * @param[out]    z              密钥流（确保内存已分配）
 */
ENDECODE_API void GenerateKeystream(snow_3g_context* ctx, uint32_t n, uint32_t* z);

/**
 * @brief 生成密钥流
 * @param[in]     k              16x8bit 密钥
 * @param[in]     iv             16x8bit 初始值
 * @param[out]    ks             密钥流
 * @param[in]     len            密钥流长度
 */
ENDECODE_API void snow_3g(uint32_t k[4], uint32_t IV[4], uint32_t n, uint32_t* z);

#ifdef __cplusplus
}
#endif

#endif