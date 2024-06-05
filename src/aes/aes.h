#ifndef __AES_H__
#define __AES_H__

#include <stdint.h>
#include <string.h>

#define AES_BLOCKLEN 16 // Block length in bytes - AES is 128b block only
#define Nb           4  // Number of columns (32-bit words) comprising the State. For AES, Nb is always 4.
#define Nk           4  // Number of 32-bit words comprising the Cipher Key. For AES-128, Nk is 4.
#define Nr           10 // Number of rounds. For AES-128, Nr is 10.

typedef struct
{
    uint8_t  state[4][4];
    uint32_t roundKeys[Nb * (Nr + 1)];
} AESContext;

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

ENDECODE_API void aes_128(const uint8_t* key, uint8_t* input, uint8_t* output);

#ifdef __cplusplus
}
#endif

#endif