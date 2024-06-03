#ifndef __DES_H__
#define __DES_H__

#include <stdint.h>

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief des
 * @param[in]     plaintext      明文
 * @param[in]     key            密钥
 * @param[in]     method         1-加密，0-解密
 */
ENDECODE_API uint64_t des(uint64_t plaintext, uint64_t key, int method);

#ifdef __cplusplus
}
#endif

#endif
