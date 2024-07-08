#include <stdint.h>

#include "endecode/a5_1/a5_1.h"

/* Masks for the three shift registers */
#define R1MASK 0x07FFFF /* 19 bits, numbered 0..18 */
#define R2MASK 0x3FFFFF /* 22 bits, numbered 0..21 */
#define R3MASK 0x7FFFFF /* 23 bits, numbered 0..22 */

/* Middle bit of each of the three shift registers, for clock control */
#define R1MID 0x000100 /* bit 8 */
#define R2MID 0x000400 /* bit 10 */
#define R3MID 0x000400 /* bit 10 */

/* Feedback taps, for clocking the shift registers.
 * These correspond to the primitive polynomials
 * x^19 + x^18 + x^17 + x^14 + 1,
 * x^22 + x^21 + 1,
 * x^23 + x^22 + x^21 + x^8 + 1.
 */
#define R1TAPS 0x072000 /* bits 18,17,16,13 */
#define R2TAPS 0x300000 /* bits 21,20 */
#define R3TAPS 0x700080 /* bits 22,21,20,7 */

/* Output taps, for output generation */
#define R1OUT 0x040000 /* bit 18 (the high bit) */
#define R2OUT 0x200000 /* bit 21 (the high bit) */
#define R3OUT 0x400000 /* bit 22 (the high bit) */

/* Calculate the parity of a 32-bit uint32_t, i.e. the sum of its bits modulo 2 */
uint32_t parity(uint32_t x)
{
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}

/* Clock one shift register */
uint32_t clockone(uint32_t reg, uint32_t mask, uint32_t taps)
{
    uint32_t t = reg & taps;
    reg        = (reg << 1) & mask;
    reg |= parity(t);
    return reg;
}

/* The three shift registers.  They're in global variables to make the code
 * easier to understand.
 * A better implementation would not use global variables. */
uint32_t R1, R2, R3;

/* Look at the middle bits of R1,R2,R3, take a vote, and
 * return the majority value of those 3 bits. */
uint32_t majority()
{
    int sum;
    sum = parity(R1 & R1MID) + parity(R2 & R2MID) + parity(R3 & R3MID);
    if (sum >= 2)
        return 1;
    else
        return 0;
}

/* Clock two or three of R1,R2,R3, with clock control
 * according to their middle bits.
 * Specifically, we clock Ri whenever Ri's middle bit
 * agrees with the majority value of the three middle bits.*/
void a5_1_clock()
{
    uint32_t maj = majority();
    if (((R1 & R1MID) != 0) == maj)
        R1 = clockone(R1, R1MASK, R1TAPS);
    if (((R2 & R2MID) != 0) == maj)
        R2 = clockone(R2, R2MASK, R2TAPS);
    if (((R3 & R3MID) != 0) == maj)
        R3 = clockone(R3, R3MASK, R3TAPS);
}

/* Clock all three of R1,R2,R3, ignoring their middle bits.
 * This is only used for key setup. */
void clockallthree()
{
    R1 = clockone(R1, R1MASK, R1TAPS);
    R2 = clockone(R2, R2MASK, R2TAPS);
    R3 = clockone(R3, R3MASK, R3TAPS);
}

/* Generate an output bit from the current state.
 * You grab a bit from each register via the output generation taps;
 * then you XOR the resulting three bits. */
uint32_t getbit()
{
    return parity(R1 & R1OUT) ^ parity(R2 & R2OUT) ^ parity(R3 & R3OUT);
}

/* Do the A5/1 key setup.  This routine accepts a 64-bit key and
 * a 22-bit frame number. */
void keysetup(uint8_t key[8], uint32_t frame)
{
    int      i;
    uint32_t keybit, framebit;

    /* Zero out the shift registers. */
    R1 = R2 = R3 = 0;

    /* Load the key into the shift registers,
     * LSB of first uint8_t of key array first,
     * clocking each register once for every
     * key bit loaded.  (The usual clock
     * control rule is temporarily disabled.) */
    for (i = 0; i < 64; i++)
    {
        clockallthree();                      /* always clock */
        keybit = (key[i / 8] >> (i & 7)) & 1; /* The i-th bit of the key */
        R1 ^= keybit;
        R2 ^= keybit;
        R3 ^= keybit;
    }

    /* Load the frame number into the shift
     * registers, LSB first,
     * clocking each register once for every
     * key bit loaded.  (The usual clock
     * control rule is still disabled.) */
    for (i = 0; i < 22; i++)
    {
        clockallthree();             /* always clock */
        framebit = (frame >> i) & 1; /* The i-th bit of the frame */
        R1 ^= framebit;
        R2 ^= framebit;
        R3 ^= framebit;
    }

    /* Run the shift registers for 100 clocks
     * to mix the keying material and frame number
     * together with output generation disabled,
     * so that there is sufficient avalanche.
     * We re-enable the majority-based clock control
     * rule from now on. */
    for (i = 0; i < 100; i++)
    {
        a5_1_clock();
    }

    /* Now the key is properly set up. */
}

/* Generate output.  We generate 228 bits of
 * keystream output.  The first 114 bits is for
 * the A->B frame; the next 114 bits is for the
 * B->A frame.  You allocate a 15-uint8_t buffer
 * for each direction, and this function fills
 * it in. */
void a5_1_generate_keystream(uint8_t key[8], uint32_t frame, uint8_t* AtoBkeystream, uint8_t* BtoAkeystream)
{
    keysetup(key, frame);

    /* Generate 114 bits of keystream for the
     * A->B direction.  Store it, MSB first. */
    for (int i = 0; i < 114; i++)
    {
        a5_1_clock();
        AtoBkeystream[i / 8] |= getbit() << (7 - (i & 7));
    }

    /* Generate 114 bits of keystream for the
     * B->A direction.  Store it, MSB first. */
    for (int i = 0; i < 114; i++)
    {
        a5_1_clock();
        BtoAkeystream[i / 8] |= getbit() << (7 - (i & 7));
    }
}