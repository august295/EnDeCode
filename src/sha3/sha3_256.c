#include "endecode/sha3/sha3_256.h"

// clang-format off
const uint64_t keccakf_rndc[KECCAKF_ROUNDS] = {
    0x0000000000000001ULL, 0x0000000000008082ULL, 0x800000000000808aULL,
    0x8000000080008000ULL, 0x000000000000808bULL, 0x0000000080000001ULL,
    0x8000000080008081ULL, 0x8000000000008009ULL, 0x000000000000008aULL,
    0x0000000000000088ULL, 0x0000000080008009ULL, 0x000000008000000aULL,
    0x000000008000808bULL, 0x800000000000008bULL, 0x8000000000008089ULL,
    0x8000000000008003ULL, 0x8000000000008002ULL, 0x8000000000000080ULL,
    0x000000000000800aULL, 0x800000008000000aULL, 0x8000000080008081ULL,
    0x8000000000008080ULL, 0x0000000080000001ULL, 0x8000000080008008ULL
};

const int keccakf_rotc[KECCAK_STATE_SIZE] = {
    1,  3,   6, 10, 15, 21, 28, 36,
    45, 55,  2, 14, 27, 41, 56,  8,
    25, 43, 62, 18, 39, 61, 20, 44
};

const int keccakf_piln[KECCAK_STATE_SIZE] = {
    10,  7, 11, 17, 18,  3,  5, 16,
     8, 21, 24,  4, 15, 23, 19, 13,
    12,  2, 20, 14, 22,  9,  6,  1
};
// clang-format on

#define ROTL64(x, y) (((x) << (y)) | ((x) >> (64 - (y))))

void keccakf(uint64_t st[KECCAK_STATE_SIZE])
{
    int      i, j, round;
    uint64_t t, bc[5];

    for (round = 0; round < KECCAKF_ROUNDS; round++)
    {
        for (i = 0; i < 5; i++)
        {
            bc[i] = st[i] ^ st[i + 5] ^ st[i + 10] ^ st[i + 15] ^ st[i + 20];
        }

        for (i = 0; i < 5; i++)
        {
            t = bc[(i + 4) % 5] ^ ((bc[(i + 1) % 5] << 1) | (bc[(i + 1) % 5] >> (64 - 1)));
            for (j = 0; j < 25; j += 5)
            {
                st[j + i] ^= t;
            }
        }

        t = st[1];
        for (i = 0; i < 24; i++)
        {
            j     = keccakf_piln[i];
            bc[0] = st[j];
            st[j] = (t << keccakf_rotc[i]) | (t >> (64 - keccakf_rotc[i]));
            t     = bc[0];
        }

        for (j = 0; j < 25; j += 5)
        {
            for (i = 0; i < 5; i++)
            {
                bc[i] = st[j + i];
            }
            for (i = 0; i < 5; i++)
            {
                st[j + i] ^= (~bc[(i + 1) % 5]) & bc[(i + 2) % 5];
            }
        }

        st[0] ^= keccakf_rndc[round];
    }
}

void sha3_256_init(sha3_256_ctx* ctx)
{
    memset(ctx, 0, sizeof(sha3_256_ctx));
}

void sha3_256_update(sha3_256_ctx* ctx, const uint8_t* data, size_t len)
{
    size_t i;
    while (len > 0)
    {
        size_t to_copy = 136 - ctx->buffer_size;
        if (to_copy > len)
            to_copy = len;

        memcpy(ctx->buffer + ctx->buffer_size, data, to_copy);
        ctx->buffer_size += to_copy;
        data += to_copy;
        len -= to_copy;

        if (ctx->buffer_size == 136)
        {
            for (i = 0; i < 17; i++)
            {
                ctx->state[i] ^= ((uint64_t*)ctx->buffer)[i];
            }
            keccakf(ctx->state);
            ctx->buffer_size = 0;
        }
    }
}

void sha3_256_final(sha3_256_ctx* ctx, uint8_t* hash)
{
    size_t i;
    ctx->buffer[ctx->buffer_size++] = 0x06;
    memset(ctx->buffer + ctx->buffer_size, 0, 136 - ctx->buffer_size);
    ctx->buffer[135] |= 0x80;

    for (i = 0; i < 17; i++)
    {
        ctx->state[i] ^= ((uint64_t*)ctx->buffer)[i];
    }
    keccakf(ctx->state);

    for (i = 0; i < SHA3_256_DIGEST_LENGTH / 8; i++)
    {
        ((uint64_t*)hash)[i] = ctx->state[i];
    }
}

void sha3_256(const uint8_t* data, uint64_t data_len, uint8_t* digest)
{
    sha3_256_ctx ctx;
    sha3_256_init(&ctx);
    sha3_256_update(&ctx, data, data_len);
    sha3_256_final(&ctx, digest);
}
