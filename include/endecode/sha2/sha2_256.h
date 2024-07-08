#ifndef __SHA256_H__
#define __SHA256_H__

#include <stdint.h>
#include <string.h>

#define SHA2_256_BLOCK_SIZE    64
#define SHA2_256_DIGEST_LENGTH 32

typedef struct
{
    uint32_t state[8];
    uint64_t count;
    uint8_t  buffer[SHA2_256_BLOCK_SIZE];
} SHA256_CTX;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief sha2-256
 * @param[in]     data           明文
 * @param[in]     data_len       明文长度
 * @param[out]    digest         32x8bit 哈希值
 */
ENDECODE_API void sha2_256(const uint8_t* data, uint64_t len, uint8_t* digest);

#ifdef __cplusplus
}
#endif

#endif