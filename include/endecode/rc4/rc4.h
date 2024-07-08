#ifndef __RC4_H__
#define __RC4_H__

#include <stddef.h>
#include <stdint.h>

/**
 * @brief RC4 algorithm context
 **/
typedef struct
{
    uint16_t i;
    uint16_t j;
    uint8_t  s[256];
} Rc4Context;

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif
/**
 * @brief Initialize an RC4 context using the supplied key
 * @param[in] context   Pointer to the RC4 context to initialize
 * @param[in] key       Pointer to the key
 * @param[in] length    Length of the key
 * @return Error code
 **/
ENDECODE_API int rc4_init(Rc4Context* context, const uint8_t* key, uint64_t length);

/**
 * @brief Encrypt/decrypt data with the RC4 algorithm
 * @param[in] context   Pointer to the RC4 context
 * @param[in] input     Pointer to the data to encrypt/decrypt (optional)
 * @param[in] output    Pointer to the resulting data (optional)
 * @param[in] length    Length of the input data
 **/
ENDECODE_API void rc4_crypt(Rc4Context* context, const uint8_t* input, uint8_t* output, uint64_t length);

#ifdef __cplusplus
}
#endif

#endif