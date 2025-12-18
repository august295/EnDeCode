#ifndef __SM4_H__
#define __SM4_H__

#include <stddef.h>
#include <stdint.h>

#include "endecode/pad/pad.h"

#include "endecode/common/endecode_export.h"
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

/**
 * @brief   ECB模式加密
 * @param   input                   [IN]        密文
 * @param   input_len               [IN]        密文长度
 * @param   output                  [IN]        明文
 * @param   output_len              [IN]        明文长度
 * @param   func_pad                [IN]        填充函数
 */
ENDECODE_API void sm4_encrypt_ecb(const uint8_t* input, size_t input_len, uint8_t* output, size_t* output_len, _func_pad func_pad);

/**
 * @brief   ECB模式解密
 * @param   input                   [IN]        密文
 * @param   input_len               [IN]        密文长度
 * @param   output                  [IN]        明文
 * @param   output_len              [IN]        明文长度
 * @param   func_pad                [IN]        填充函数
 */
ENDECODE_API void sm4_decrypt_ecb(const uint8_t* input, size_t input_len, uint8_t* output, size_t* output_len, _func_unpad func_unpad);

#ifdef __cplusplus
}
#endif

#endif