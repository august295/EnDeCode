#ifndef __RSA_H__
#define __RSA_H__

#include <stdint.h>

#include <gmp.h>

#include "endecode/pad/pad.h"

typedef struct rsa_st
{
    uint32_t bits; // bit length
    mpz_t    n;
    mpz_t    e; // public key (n,e)
    mpz_t    d; // private key (n,d)
    mpz_t    p;
    mpz_t    q;
    mpz_t    p1;
    mpz_t    q1;
    mpz_t    phi;
} rsa_st;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief 生成 RSA 原始密钥
 * @param[in]     bits           密钥长度
 * @param[in]     e              公钥中的 e
 * @return rsa_st*               RSA 原始密钥
 */
ENDECODE_API rsa_st* rsa_key_gen(uint32_t bits, uint32_t e);

/**
 * @brief 销毁 RSA 原始密钥
 * @param[in]     ctx            RSA 原始密钥
 */
ENDECODE_API void rsa_key_free(rsa_st* ctx);

/**
 * @brief 公钥加密
 * @param[in]     input          明文
 * @param[in]     input_len      明文长度
 * @param[out]    output         密文
 * @param[in]     ctx            RSA 原始密钥
 * @param[in]     func_pad       填充函数
 * @return int                   密文长度
 */
ENDECODE_API int rsa_public_encrypt(const uint8_t* input, uint32_t input_len, uint8_t* output, const rsa_st* ctx, _func_pad func_pad);

/**
 * @brief 私钥解密
 * @param[in]     input          密文
 * @param[in]     input_len      密文长度
 * @param[out]    output         明文
 * @param[in]     ctx            RSA 原始密钥
 * @param[in]     func_unpad     取消填充函数
 * @return int                   明文长度
 */
ENDECODE_API int rsa_private_decrypt(const uint8_t* input, uint32_t input_len, uint8_t* output, const rsa_st* ctx, _func_unpad func_unpad);

#ifdef __cplusplus
}
#endif

#endif