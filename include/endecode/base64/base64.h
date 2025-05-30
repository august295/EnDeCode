#ifndef BASE64_H
#define BASE64_H

#include <stddef.h>
#include <stdint.h>

#define BASE64_ENCODE_OUT_SIZE(s) ((size_t)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((size_t)(((s) / 4) * 3))

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/***************************************************************************
 * @date    2021/04/19
 * @brief   base64 加密
 * @param   in      加密前字符串
 * @param   inlen   加密前字符串长度
 * @param   out     加密后字符串
 * @return  加密后字符串长度
 ***************************************************************************/
ENDECODE_API size_t base64_encode(const uint8_t* in, size_t inlen, char* out);

/***************************************************************************
 * @date    2021/04/19
 * @brief   base64 解密
 * @param   in      解密前字符串
 * @param   inlen   解密前字符串长度
 * @param   out     解密后字符串
 * @return  解密后字符串长度
 ***************************************************************************/
ENDECODE_API size_t base64_decode(const char* in, size_t inlen, uint8_t* out);

#ifdef __cplusplus
}
#endif

#endif