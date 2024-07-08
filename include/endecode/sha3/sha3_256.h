#ifndef __SHA3_H__
#define __SHA3_H__

#include <stdint.h>
#include <string.h>

#define SHA3_256_DIGEST_LENGTH 32
#define KECCAKF_ROUNDS         24
#define KECCAK_STATE_SIZE      25

typedef struct
{
    uint64_t state[KECCAK_STATE_SIZE];
    uint8_t  buffer[136];
    size_t   buffer_size;
} sha3_256_ctx;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief sha3_256
 * @param[in]     data           明文
 * @param[in]     data_len       明文长度
 * @param[out]    digest         32x8bit 哈希值
 */
ENDECODE_API void sha3_256(const uint8_t* data, uint64_t data_len, uint8_t* digest);

#ifdef __cplusplus
}
#endif

#endif