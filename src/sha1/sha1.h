#ifndef __SHA1_H__
#define __SHA1_H__

#include <stdint.h>
#include <string.h>

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

// 定义 SHA-1 的数据结构
typedef struct sha1_context
{
    uint32_t state[5];
    uint32_t count[2];
    uint8_t  buffer[64];
} sha1_context;

/**
 * @brief
 * @param[in]     data
 * @param[in]     len
 * @param[in]     hash
 */
ENDECODE_API void SHA1(const uint8_t* data, uint64_t len, uint8_t* hash);

#ifdef __cplusplus
}
#endif

#endif