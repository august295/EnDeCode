#include "uia2.h"

/* MUL64x.
 * Input V: a 64-bit input.
 * Input c: a 64-bit input.
 * Output : a 64-bit output.
 * A 64-bit memory is allocated which is to be freed by the calling
3GPP Confidentiality and Integrity Algorithms UEA2&UIA2 page 25 of 27
UEA2 and UIA2 Specification Version 2.1
 * function.
 * See section 4.3.2 for details.
 */
uint64_t MUL64x(uint64_t V, uint64_t c)
{
    if (V & 0x8000000000000000)
        return (V << 1) ^ c;
    else
        return V << 1;
}

/* MUL64xPOW.
 * Input V: a 64-bit input.
 * Input i: a positive integer.
 * Input c: a 64-bit input.
 * Output : a 64-bit output.
 * A 64-bit memory is allocated which is to be freed by the calling function.
 * See section 4.3.3 for details.
 */
uint64_t MUL64xPOW(uint64_t V, uint8_t i, uint64_t c)
{
    if (i == 0)
        return V;
    else
        return MUL64x(MUL64xPOW(V, i - 1, c), c);
}

/* MUL64.
 * Input V: a 64-bit input.
 * Input P: a 64-bit input.
 * Input c: a 64-bit input.
 * Output : a 64-bit output.
 * A 64-bit memory is allocated which is to be freed by the calling
 * function.
 * See section 4.3.4 for details.
 */
uint64_t MUL64(uint64_t V, uint64_t P, uint64_t c)
{
    uint64_t result = 0;
    int      i      = 0;

    for (i = 0; i < 64; i++)
    {
        if ((P >> i) & 0x1)
            result ^= MUL64xPOW(V, i, c);
    }

    return result;
}

/* mask8bit.
 * Input n: an integer in 1-7.
 * Output : an 8 bit mask.
 * Prepares an 8 bit mask with required number of 1 bits on the MSB side.
 */
uint8_t mask8bit(int n)
{
    return 0xFF ^ ((1 << (8 - n)) - 1);
}

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
uint8_t* snow_3g_uia2(uint8_t* key, uint32_t count, uint32_t fresh, uint32_t dir, uint8_t* data, uint64_t length)
{
    uint32_t       K[4], IV[4], z[5];
    uint32_t       i        = 0, D;
    static uint8_t MAC_I[4] = {0, 0, 0, 0}; /* static memory for the result */
    uint64_t       EVAL;
    uint64_t       V;
    uint64_t       P;
    uint64_t       Q;
    uint64_t       c;

    uint64_t M_D_2;
    int      rem_bits = 0;

    /* Load the Integrity Key for SNOW3G initialization as in section 4.4. */
    for (i = 0; i < 4; i++)
    {
        K[3 - i] = (key[4 * i] << 24) ^
                   (key[4 * i + 1] << 16) ^
                   (key[4 * i + 2] << 8) ^
                   (key[4 * i + 3]);
    }

    /* Prepare the Initialization Vector (IV) for SNOW3G initialization as in section 4.4. */
    IV[3] = count;
    IV[2] = fresh;
    IV[1] = count ^ (dir << 31);
    IV[0] = fresh ^ (dir << 15);

    z[0] = z[1] = z[2] = z[3] = z[4] = 0;

    /* Run SNOW 3G to produce 5 keystream words z_1, z_2, z_3, z_4 and z_5. */
    snow_3g(K, IV, 5, z);

    P = (uint64_t)z[0] << 32 | (uint64_t)z[1];
    Q = (uint64_t)z[2] << 32 | (uint64_t)z[3];

    /* Calculation */
    if ((length % 64) == 0)
        D = (length >> 6) + 1;
    else
        D = (length >> 6) + 2;
    EVAL = 0;

    c = 0x1b;

    /* for 0 <= i <= D-3 */
    for (i = 0; i < D - 2; i++)
    {
        V = EVAL ^ ((uint64_t)data[8 * i] << 56 |
                    (uint64_t)data[8 * i + 1] << 48 |
                    (uint64_t)data[8 * i + 2] << 40 |
                    (uint64_t)data[8 * i + 3] << 32 |
                    (uint64_t)data[8 * i + 4] << 24 |
                    (uint64_t)data[8 * i + 5] << 16 |
                    (uint64_t)data[8 * i + 6] << 8 |
                    (uint64_t)data[8 * i + 7]);

        EVAL = MUL64(V, P, c);
    }

    /* for D-2 */
    rem_bits = length % 64;
    if (rem_bits == 0)
        rem_bits = 64;

    M_D_2 = 0;
    i     = 0;
    while (rem_bits > 7)
    {
        M_D_2 |= (uint64_t)data[8 * (D - 2) + i] << (8 * (7 - i));
        rem_bits -= 8;
        i++;
    }
    if (rem_bits > 0)
        M_D_2 |= (uint64_t)(data[8 * (D - 2) + i] & mask8bit(rem_bits)) << (8 * (7 - i));

    V    = EVAL ^ M_D_2;
    EVAL = MUL64(V, P, c);

    /* for D-1 */
    EVAL ^= length;

    /* Multiply by Q */
    EVAL = MUL64(EVAL, Q, c);
    for (i = 0; i < 4; i++)
    {
        // MAC_I[i] = (mac32 >> (8*(3-i))) & 0xff;
        MAC_I[i] = (EVAL >> (8 * (3 - i))) & 0xff;
    }
    return MAC_I;
}
/* End of f9.c */
