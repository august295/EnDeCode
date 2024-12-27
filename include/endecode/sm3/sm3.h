#ifndef __SM3_H__
#define __SM3_H__

#include <stdint.h>
#include <string.h>

#define SM3_BLOCK_SIZE    64
#define SM3_DIGEST_LENGTH 32

typedef struct
{
    uint32_t state[8];
    uint64_t count;
    uint8_t  buffer[SM3_BLOCK_SIZE];
} sm3_context;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief sm3
 * @param[in]     data           明文
 * @param[in]     data_len       明文长度
 * @param[out]    hash           32x8bit 哈希值
 */
ENDECODE_API void sm3(const uint8_t* data, uint64_t data_len, uint8_t* digest);

ENDECODE_API void sm3_hmac(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t* hmac);

ENDECODE_API void sm3_x9_63_kdf(const uint8_t* z, size_t z_len, const uint8_t* shared_info, size_t shared_info_len, uint8_t* derived_key, size_t key_len);

#ifdef __cplusplus
}
#endif

#endif