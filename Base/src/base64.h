#ifndef BASE64_H
#define BASE64_H

#define BASE64_ENCODE_OUT_SIZE(s) ((unsigned int)((((s) + 2) / 3) * 4 + 1))
#define BASE64_DECODE_OUT_SIZE(s) ((unsigned int)(((s) / 4) * 3))

/***************************************************************************
 * @date    2021/04/19
 * @brief   base64 加密
 * @param   in      加密前字符串
 * @param   inlen   加密前字符串长度
 * @param   out     加密后字符串
 * @return  加密后字符串长度
 ***************************************************************************/
unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out);

/***************************************************************************
 * @date    2021/04/19
 * @brief   base64 解密
 * @param   in      解密前字符串
 * @param   inlen   解密前字符串长度
 * @param   out     解密后字符串
 * @return  解密后字符串长度
 ***************************************************************************/
unsigned int base64_decode(const char *in, unsigned int inlen, unsigned char *out);

#endif