#include <stdlib.h>

#include "uea2.h"

/* f8.
 * Input key: 128 bit Confidentiality Key.
 * Input count:32-bit Count, Frame dependent input.
 * Input bearer: 5-bit Bearer identity (in the LSB side).
 * Input dir:1 bit, direction of transmission.
 * Input data: length number of bits, input bit stream.
 * Input length: 32 bit Length, i.e., the number of bits to be encrypted or decrypted.
 * Output data: Output bit stream. Assumes data is suitably memory allocated.
 * Encrypts/decrypts blocks of data between 1 and 2^32 bits in length as defined in Section 3.
 */
void snow_3g_uea2(uint8_t* key, uint32_t count, uint32_t bearer, uint32_t dir, uint8_t* data, uint32_t length)
{
    uint32_t  K[4], IV[4];
    int       n = (length + 31) / 32;
    int       i = 0;
    uint32_t* KS;

    /*Initialisation*/

    /* Load the confidentiality key for SNOW 3G initialization as in section 3.4. */
    for (i = 0; i < 4; i++)
    {
        K[3 - i] = (key[4 * i] << 24) ^
                   (key[4 * i + 1] << 16) ^
                   (key[4 * i + 2] << 8) ^
                   (key[4 * i + 3]);
    }

    /* Prepare the initialization vector (IV) for SNOW 3G initialization as in section 3.4. */
    IV[3] = count;
    IV[2] = (bearer << 27) | ((dir & 0x1) << 26);
    IV[1] = IV[3];
    IV[0] = IV[2];

    /* Run SNOW 3G algorithm to generate sequence of key stream bits KS*/
    KS = (uint32_t*)malloc(4 * n);
    snow_3g(K, IV, n, KS);

    /* Exclusive-OR the input data with keystream to generate the output bit stream */
    for (i = 0; i < n; i++)
    {
        data[4 * i + 0] ^= (uint8_t)(KS[i] >> 24) & 0xff;
        data[4 * i + 1] ^= (uint8_t)(KS[i] >> 16) & 0xff;
        data[4 * i + 2] ^= (uint8_t)(KS[i] >> 8) & 0xff;
        data[4 * i + 3] ^= (uint8_t)(KS[i]) & 0xff;
    }

    free(KS);
}
