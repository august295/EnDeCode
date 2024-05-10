#ifndef __MD5_H__
#define __MD5_H__

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * 函数
 *  1. 32bit  转换为 4*8bit
 *  2. 4*8bit 转换为 32bit
 *  3. md5 计算
 *  4. 16进制转字符串
 */

/**
 * @brief 32bit 转换为 4*8bit
 */
void to_bytes(const uint32_t val, uint8_t* bytes);

/**
 * @brief 4*8bit 转换为 32bit
 */
void to_int32(const uint8_t* bytes, uint32_t* val);

/**
 * @brief md5 计算核心函数
 * @param initial_msg  [in] 输入流
 * @param initial_len  [in] 输入流长度
 * @param digest       [out] 16进制输出
 */
void md5(const uint8_t* initial_msg, uint64_t initial_len, uint8_t* digest);

/**
 * @brief 16进制转字符串
 * @param pHex         [in] 16进制
 * @param flag         [in] 0-大写，1-小写
 * @param pAscii       [out] 字符串
 * @param nLen         [out] 字符串长度
 */
void hex_to_ascii(const uint8_t* pHex, int flag, uint8_t* pAscii, int nLen);

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief mds5 加密
 * @param plaintext     明文
 * @param plaintext_len 明文长度
 * @param flag          0-大写，1-小写
 * @param ciphertext    密文
 */
ENDECODE_API void md5_ascii(const uint8_t* plaintext, uint64_t plaintext_len, int flag, uint8_t* ciphertext);

#ifdef __cplusplus
}
#endif

#endif