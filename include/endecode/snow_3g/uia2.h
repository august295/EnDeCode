#ifndef __UIA2_H__
#define __UIA2_H__

#include "snow_3g.h"

#include "endecode/common/endecode_export.h"
#ifdef __cplusplus
extern "C"
{
#endif
/* f9.
 * Input key: 128 bit Integrity Key.
 * Input count:32-bit Count, Frame dependent input.
 * Input fresh: 32-bit Random number.
 * Input dir:1 bit, direction of transmission (in the LSB).
 * Input data: length number of bits, input bit stream.
 * Input length: 64 bit Length, i.e., the number of bits to be MAC'd.
 * Output  : 32 bit block used as MAC
 * Generates 32-bit MAC using UIA2 algorithm as defined in Section 4.
 */
ENDECODE_API uint8_t* snow_3g_uia2(uint8_t* key, uint32_t count, uint32_t fresh, uint32_t dir, uint8_t* data, uint64_t length);

#ifdef __cplusplus
}
#endif

#endif
