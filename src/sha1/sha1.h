#ifndef __SHA1_H__
#define __SHA1_H__

#include <stdint.h>
#include <string.h>

#define SHA1_BLOCK_SIZE    64
#define SHA1_DIGEST_LENGTH 20

// 定义 SHA-1 的数据结构
typedef struct sha1_context
{
    uint32_t state[5];
    uint32_t count[2];
    uint8_t  buffer[SHA1_BLOCK_SIZE];
} sha1_context;

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief SHA1
 * @param[in]     data           明文
 * @param[in]     data_len       明文长度
 * @param[out]    digest         20x8bit 哈希值
 */
ENDECODE_API void SHA1(const uint8_t* data, uint64_t len, uint8_t* digest);

#ifdef __cplusplus
}
#endif

#endif