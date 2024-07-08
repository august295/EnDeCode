#ifndef __MD5_H__
#define __MD5_H__

#include <stdint.h>
#include <string.h>

#define MD5_BLOCK_SIZE    64
#define MD5_DIGEST_LENGTH 16

typedef struct md5_context
{
    uint32_t      state[4];
    uint32_t      count[2];
    unsigned char buffer[MD5_BLOCK_SIZE];
} md5_context;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief MD5
 * @param[in]     data           明文
 * @param[in]     data_len       明文长度
 * @param[out]    digest         16x8bit 哈希值
 */
ENDECODE_API void md5(const uint8_t* data, uint64_t data_len, uint8_t* digest);

#ifdef __cplusplus
}
#endif

#endif