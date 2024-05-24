#ifndef __A5_1_H__
#define __A5_1_H__

#include <stdint.h>

uint32_t parity(uint32_t x);

uint32_t clockone(uint32_t reg, uint32_t mask, uint32_t taps);

uint32_t majority();

void a5_1_clock();

void clockallthree();

uint32_t getbit();

void keysetup(uint8_t key[8], uint32_t frame);

#include "common/endecode_export.hpp"
#ifdef __cplusplus
extern "C"
{
#endif

ENDECODE_API void a5_1_generate_keystream(uint8_t key[8], uint32_t frame, uint8_t* AtoBkeystream, uint8_t* BtoAkeystream);

#ifdef __cplusplus
}
#endif

#endif
