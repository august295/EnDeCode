#ifndef __SM4_H__
#define __SM4_H__

#include <stddef.h>
#include <stdint.h>

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/* SM4 algorithm context */
typedef struct
{
    uint32_t nr;
    uint32_t rk[32];
} Sm4Context;

/**
 * @brief 密钥扩展
 * @param[in,out] context        sm4 上下文
 * @param[in]     key            16x8bit 密钥
 * @param[in]     keyLen         密钥长度
 */
ENDECODE_API int sm4Init(Sm4Context* context, const uint8_t* key, uint64_t keyLen);

/**
 * @brief 加密
 * @param[in]     context        sm4 上下文
 * @param[in]     input          8bit 明文
 * @param[in]     output         8bit 密文
 */
ENDECODE_API void sm4EncryptBlock(Sm4Context* context, const uint8_t* input, uint8_t* output);

/**
 * @brief 解密
 * @param[in]     context        sm4 上下文
 * @param[in]     input          8bit 密文
 * @param[in]     output         8bit 明文
 */
ENDECODE_API void sm4DecryptBlock(Sm4Context* context, const uint8_t* input, uint8_t* output);

#ifdef __cplusplus
}
#endif

#endif