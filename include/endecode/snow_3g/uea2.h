#ifndef __UEA2_H__
#define __UEA2_H__

#include "snow_3g.h"

#include "endecode/common/endecode_export.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* f8.
 * Input key: 128 bit Confidentiality Key.
 * Input count:32-bit Count, Frame dependent input.
 * Input bearer: 5-bit Bearer identity (in the LSB side).
 * Input dir:1 bit, direction of transmission.
 * Input data: length number of bits, input bit stream.
 * Input length: 32 bit Length, i.e., the number of bits to be encrypted or
 *               decrypted.
 * Output data: Output bit stream. Assumes data is suitably memory
 * allocated.
 * Encrypts/decrypts blocks of data between 1 and 2^32 bits in length as
 * defined in Section 3.
 */
ENDECODE_API void snow_3g_uea2(uint8_t* key, uint32_t count, uint32_t bearer, uint32_t dir, uint8_t* data, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif