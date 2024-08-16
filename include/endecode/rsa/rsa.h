#ifndef __RSA_H__
#define __RSA_H__

#include <stdint.h>

#include "endecode/common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

// ENDECODE_API void rsa(const uint8_t* key, uint8_t* input, uint8_t* output);
ENDECODE_API void rsa();

#ifdef __cplusplus
}
#endif

#endif