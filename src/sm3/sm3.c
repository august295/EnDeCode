#include "endecode/sm3/sm3.h"
#include "endecode/sm2/sm2_define.hpp"

#define ROTL(x, n)   (((x) << (n)) | ((x) >> (32 - (n))))
#define P0(x)        ((x) ^ ROTL((x), 9) ^ ROTL((x), 17))
#define P1(x)        ((x) ^ ROTL((x), 15) ^ ROTL((x), 23))
#define FF0(x, y, z) ((x) ^ (y) ^ (z))
#define FF1(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define GG0(x, y, z) ((x) ^ (y) ^ (z))
#define GG1(x, y, z) (((x) & (y)) | ((~(x)) & (z)))

void sm3_transform(sm3_context* ctx, const uint8_t block[SM3_BLOCK_SIZE])
{
    uint32_t W[68], W1[64];
    uint32_t A, B, C, D, E, F, G, H;
    uint32_t SS1, SS2, TT1, TT2;
    int      j;

    for (j = 0; j < 16; j++)
    {
        W[j] = (block[j * 4] << 24) | (block[j * 4 + 1] << 16) | (block[j * 4 + 2] << 8) | (block[j * 4 + 3]);
    }

    for (j = 16; j < 68; j++)
    {
        W[j] = P1(W[j - 16] ^ W[j - 9] ^ ROTL(W[j - 3], 15)) ^ ROTL(W[j - 13], 7) ^ W[j - 6];
    }

    for (j = 0; j < 64; j++)
    {
        W1[j] = W[j] ^ W[j + 4];
    }

    A = ctx->state[0];
    B = ctx->state[1];
    C = ctx->state[2];
    D = ctx->state[3];
    E = ctx->state[4];
    F = ctx->state[5];
    G = ctx->state[6];
    H = ctx->state[7];

    for (j = 0; j < 16; j++)
    {
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(0x79cc4519, j)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF0(A, B, C) + D + SS2 + W1[j];
        TT2 = GG0(E, F, G) + H + SS1 + W[j];
        D   = C;
        C   = ROTL(B, 9);
        B   = A;
        A   = TT1;
        H   = G;
        G   = ROTL(F, 19);
        F   = E;
        E   = P0(TT2);
    }

    for (j = 16; j < 64; j++)
    {
        SS1 = ROTL((ROTL(A, 12) + E + ROTL(0x7a879d8a, j % 32)), 7);
        SS2 = SS1 ^ ROTL(A, 12);
        TT1 = FF1(A, B, C) + D + SS2 + W1[j];
        TT2 = GG1(E, F, G) + H + SS1 + W[j];
        D   = C;
        C   = ROTL(B, 9);
        B   = A;
        A   = TT1;
        H   = G;
        G   = ROTL(F, 19);
        F   = E;
        E   = P0(TT2);
    }

    ctx->state[0] ^= A;
    ctx->state[1] ^= B;
    ctx->state[2] ^= C;
    ctx->state[3] ^= D;
    ctx->state[4] ^= E;
    ctx->state[5] ^= F;
    ctx->state[6] ^= G;
    ctx->state[7] ^= H;
}

void sm3_init(sm3_context* ctx)
{
    ctx->count    = 0;
    ctx->state[0] = 0x7380166F;
    ctx->state[1] = 0x4914B2B9;
    ctx->state[2] = 0x172442D7;
    ctx->state[3] = 0xDA8A0600;
    ctx->state[4] = 0xA96F30BC;
    ctx->state[5] = 0x163138AA;
    ctx->state[6] = 0xE38DEE4D;
    ctx->state[7] = 0xB0FB0E4E;
    memset(ctx->buffer, 0, SM3_BLOCK_SIZE);
}

void sm3_update(sm3_context* ctx, const uint8_t* data, size_t len)
{
    size_t fill = SM3_BLOCK_SIZE - (ctx->count % SM3_BLOCK_SIZE);
    size_t left = ctx->count % SM3_BLOCK_SIZE;
    ctx->count += len;

    if (left && len >= fill)
    {
        memcpy(ctx->buffer + left, data, fill);
        sm3_transform(ctx, ctx->buffer);
        data += fill;
        len -= fill;
        left = 0;
    }

    while (len >= SM3_BLOCK_SIZE)
    {
        sm3_transform(ctx, data);
        data += SM3_BLOCK_SIZE;
        len -= SM3_BLOCK_SIZE;
    }

    if (len)
    {
        memcpy(ctx->buffer + left, data, len);
    }
}

void sm3_final(sm3_context* ctx, uint8_t digest[SM3_DIGEST_LENGTH])
{
    uint8_t  bits[8];
    uint32_t left   = ctx->count % SM3_BLOCK_SIZE;
    uint32_t padLen = (left < 56) ? (56 - left) : (120 - left);
    uint64_t count  = ctx->count << 3;

    bits[0] = (count >> 56) & 0xFF;
    bits[1] = (count >> 48) & 0xFF;
    bits[2] = (count >> 40) & 0xFF;
    bits[3] = (count >> 32) & 0xFF;
    bits[4] = (count >> 24) & 0xFF;
    bits[5] = (count >> 16) & 0xFF;
    bits[6] = (count >> 8) & 0xFF;
    bits[7] = count & 0xFF;

    static const uint8_t padding[SM3_BLOCK_SIZE] = {0x80};
    sm3_update(ctx, padding, padLen);
    sm3_update(ctx, bits, 8);

    for (int i = 0; i < 8; i++)
    {
        digest[i * 4]     = (ctx->state[i] >> 24) & 0xFF;
        digest[i * 4 + 1] = (ctx->state[i] >> 16) & 0xFF;
        digest[i * 4 + 2] = (ctx->state[i] >> 8) & 0xFF;
        digest[i * 4 + 3] = ctx->state[i] & 0xFF;
    }
}

void sm3(const uint8_t* data, uint64_t data_len, uint8_t* digest)
{
    sm3_context ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, data, data_len);
    sm3_final(&ctx, digest);
}

void sm3_hmac(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t* hmac)
{
    uint8_t k_ipad[SM3_BLOCK_SIZE];
    uint8_t k_opad[SM3_BLOCK_SIZE];
    uint8_t key_hash[SM3_DIGEST_LENGTH];
    uint8_t inner_hash[SM3_DIGEST_LENGTH];
    size_t  i;

    // Step 1: Process the key
    if (key_len > SM3_BLOCK_SIZE)
    {
        sm3(key, key_len, key_hash);
        key     = key_hash;
        key_len = SM3_DIGEST_LENGTH;
    }

    memset(k_ipad, 0x36, SM3_BLOCK_SIZE);
    memset(k_opad, 0x5C, SM3_BLOCK_SIZE);

    for (i = 0; i < key_len; i++)
    {
        k_ipad[i] ^= key[i];
        k_opad[i] ^= key[i];
    }

    // Step 2: Calculate inner hash
    sm3_context ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, k_ipad, SM3_BLOCK_SIZE);
    sm3_update(&ctx, data, data_len);
    sm3_final(&ctx, inner_hash);

    // Step 3: Calculate outer hash
    sm3_init(&ctx);
    sm3_update(&ctx, k_opad, SM3_BLOCK_SIZE);
    sm3_update(&ctx, inner_hash, SM3_DIGEST_LENGTH);
    sm3_final(&ctx, hmac);
}

void sm3_z(const uint8_t* id, size_t id_len, const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t* z_out)
{
    // Z = SM3(ENTL || id || A || B || xG || yG || xA || yA || M)
    // ENTL = id_len * 8 (bits)
    uint8_t  entl[2];
    uint16_t entl_bits = (uint16_t)(id_len * 8);
    entl[0]            = (entl_bits >> 8) & 0xFF;
    entl[1]            = (entl_bits >> 0) & 0xFF;

    sm3_context ctx;
    sm3_init(&ctx);
    sm3_update(&ctx, entl, 2);
    sm3_update(&ctx, id, id_len);
    sm3_update(&ctx, sm2_a, 32);
    sm3_update(&ctx, sm2_b, 32);
    sm3_update(&ctx, sm2_xG, 32);
    sm3_update(&ctx, sm2_yG, 32);
    sm3_update(&ctx, key, 32);
    sm3_update(&ctx, key + 32, 32);
    sm3_final(&ctx, z_out);

    // e = SM3(Z || M)
    if (data && data_len > 0)
    {
        sm3_init(&ctx);
        sm3_update(&ctx, z_out, 32);
        sm3_update(&ctx, data, data_len);
        sm3_final(&ctx, z_out);
    }
}

void sm3_x9_63_kdf(const uint8_t* z, size_t z_len, const uint8_t* shared_info, size_t shared_info_len, uint8_t* derived_key, size_t key_len)
{
    uint8_t counter[4];
    uint8_t hash_output[SM3_DIGEST_LENGTH];
    size_t  hash_count  = (key_len + SM3_DIGEST_LENGTH - 1) / SM3_DIGEST_LENGTH;
    size_t  derived_pos = 0;

    for (uint32_t i = 1; i <= hash_count; i++)
    {
        // Encode counter (big-endian)
        counter[0] = (i >> 24) & 0xFF;
        counter[1] = (i >> 16) & 0xFF;
        counter[2] = (i >> 8) & 0xFF;
        counter[3] = i & 0xFF;

        sm3_context ctx;
        sm3_init(&ctx);

        // Hash Z || counter || shared_info
        sm3_update(&ctx, z, z_len);
        sm3_update(&ctx, counter, 4);
        if (shared_info && shared_info_len > 0)
        {
            sm3_update(&ctx, shared_info, shared_info_len);
        }

        sm3_final(&ctx, hash_output);

        // Copy to derived key
        size_t copy_len = (derived_pos + SM3_DIGEST_LENGTH > key_len) ? (key_len - derived_pos) : SM3_DIGEST_LENGTH;
        memcpy(derived_key + derived_pos, hash_output, copy_len);
        derived_pos += copy_len;
    }
}
