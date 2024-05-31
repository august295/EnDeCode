#ifndef __MD5_H__
#define __MD5_H__

#include <stddef.h>
#include <stdint.h>

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

typedef struct md5_context
{
    uint32_t      state[4];
    uint32_t      count[2];
    unsigned char buffer[64];
} md5_context;

/**
 * @brief
 * @param[in]     data_msg       明文
 * @param[in]     data_len       明文长度
 * @param[out]    digest         密文
 */
ENDECODE_API void md5(const uint8_t* data_msg, uint64_t data_len, uint8_t* digest);

#ifdef __cplusplus
}
#endif

#endif