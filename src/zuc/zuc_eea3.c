#include <malloc.h>

#include "endecode/zuc/zuc_eea3.h"

void zuc_EEA3(uint8_t* CK, uint32_t COUNT, uint32_t BEARER, uint32_t DIRECTION, uint32_t LENGTH, uint32_t* M, uint32_t* C)
{
    uint32_t *z, L, i;
    uint8_t   IV[16];
    L = (LENGTH + 31) / 32;
    z = (uint32_t*)malloc(L * sizeof(uint32_t));

    IV[0]  = (COUNT >> 24) & 0xFF;
    IV[1]  = (COUNT >> 16) & 0xFF;
    IV[2]  = (COUNT >> 8) & 0xFF;
    IV[3]  = COUNT & 0xFF;
    IV[4]  = ((BEARER << 3) | ((DIRECTION & 1) << 2)) & 0xFC;
    IV[5]  = 0;
    IV[6]  = 0;
    IV[7]  = 0;
    IV[8]  = IV[0];
    IV[9]  = IV[1];
    IV[10] = IV[2];
    IV[11] = IV[3];
    IV[12] = IV[4];
    IV[13] = IV[5];
    IV[14] = IV[6];
    IV[15] = IV[7];
    ZUC(CK, IV, z, L);
    for (i = 0; i < L; i++)
    {
        C[i] = M[i] ^ z[i];
    }
    free(z);
}