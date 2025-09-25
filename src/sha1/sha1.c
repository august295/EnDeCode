#include "endecode/sha1/sha1.h"

// 位循环左移
#define ROTL32(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))

// 初始化 SHA-1 上下文
void SHA1Init(sha1_context* context)
{
    context->state[0] = 0x67452301;
    context->state[1] = 0xEFCDAB89;
    context->state[2] = 0x98BADCFE;
    context->state[3] = 0x10325476;
    context->state[4] = 0xC3D2E1F0;
    context->count[0] = 0;
    context->count[1] = 0;
}

// SHA-1 基本运算函数
void SHA1Transform(uint32_t* state, const uint8_t* buffer)
{
    uint32_t  a, b, c, d, e, t, i, j, m[80];
    uint32_t* w = (uint32_t*)buffer;

    // 将输入缓冲区的字节转换为32位字
    for (i = 0; i < 16; ++i)
        m[i] = (w[i] << 24) | ((w[i] & 0xFF00) << 8) | ((w[i] & 0xFF0000) >> 8) | (w[i] >> 24);

    // 扩展消息
    for (i = 16; i < 80; ++i)
        m[i] = ROTL32(m[i - 3] ^ m[i - 8] ^ m[i - 14] ^ m[i - 16], 1);

    a = state[0];
    b = state[1];
    c = state[2];
    d = state[3];
    e = state[4];

    // 主循环
    for (i = 0; i < 80; ++i)
    {
        if (i < 20)
            t = ROTL32(a, 5) + ((b & c) | (~b & d)) + e + m[i] + 0x5A827999;
        else if (i < 40)
            t = ROTL32(a, 5) + (b ^ c ^ d) + e + m[i] + 0x6ED9EBA1;
        else if (i < 60)
            t = ROTL32(a, 5) + ((b & c) | (b & d) | (c & d)) + e + m[i] + 0x8F1BBCDC;
        else
            t = ROTL32(a, 5) + (b ^ c ^ d) + e + m[i] + 0xCA62C1D6;

        e = d;
        d = c;
        c = ROTL32(b, 30);
        b = a;
        a = t;
    }

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
    state[4] += e;
}

// 更新 SHA-1 上下文
void SHA1Update(sha1_context* context, const uint8_t* data, uint64_t len)
{
    uint64_t i, j;
    j = (context->count[0] >> 3) & 63;
    if ((context->count[0] += len << 3) < (len << 3))
        context->count[1]++;
    context->count[1] += (len >> 29);
    if ((j + len) > 63)
    {
        memcpy(&context->buffer[j], data, (i = 64 - j));
        SHA1Transform(context->state, context->buffer);
        for (; i + 63 < len; i += 64)
            SHA1Transform(context->state, &data[i]);
        j = 0;
    }
    else
    {
        i = 0;
    }
    memcpy(&context->buffer[j], &data[i], len - i);
}

// 生成最终的 SHA-1 哈希值
void SHA1Final(sha1_context* context, uint8_t* digest)
{
    uint8_t  finalcount[8];
    uint8_t  c = 0x80;
    uint32_t i;

    // 保存数据长度
    for (i = 0; i < 8; ++i)
        finalcount[i] = (context->count[(i >= 4 ? 0 : 1)] >> ((3 - (i & 3)) * 8)) & 255;

    // 填充
    SHA1Update(context, &c, 1);
    c = 0x00;
    while ((context->count[0] & 504) != 448)
        SHA1Update(context, &c, 1);

    // 附加长度
    SHA1Update(context, finalcount, 8);

    // 生成最终的哈希值
    for (i = 0; i < 20; ++i)
        digest[i] = (context->state[i >> 2] >> ((3 - (i & 3)) * 8)) & 255;
}

// 计算 SHA-1 哈希
void SHA1(const uint8_t* data, uint64_t data_len, uint8_t* digest)
{
    sha1_context ctx;
    SHA1Init(&ctx);
    SHA1Update(&ctx, data, data_len);
    SHA1Final(&ctx, digest);
}

void HMAC_SHA1(const uint8_t* key, size_t key_len, const uint8_t* data, size_t data_len, uint8_t* digest)
{
    uint8_t key_block[SHA1_BLOCK_SIZE];
    uint8_t inner_digest[SHA1_DIGEST_LENGTH];
    uint8_t o_key_pad[SHA1_BLOCK_SIZE];
    uint8_t i_key_pad[SHA1_BLOCK_SIZE];
    size_t  i;

    // Step1: 如果 key 太长，先哈希再用
    if (key_len > SHA1_BLOCK_SIZE)
    {
        sha1_context tctx;
        SHA1Init(&tctx);
        SHA1Update(&tctx, key, key_len);
        SHA1Final(&tctx, key_block);
        memset(key_block + SHA1_DIGEST_LENGTH, 0, SHA1_BLOCK_SIZE - SHA1_DIGEST_LENGTH);
    }
    else
    {
        memcpy(key_block, key, key_len);
        memset(key_block + key_len, 0, SHA1_BLOCK_SIZE - key_len);
    }

    // Step2: 生成 i_key_pad 和 o_key_pad
    for (i = 0; i < SHA1_BLOCK_SIZE; i++)
    {
        i_key_pad[i] = key_block[i] ^ 0x36;
        o_key_pad[i] = key_block[i] ^ 0x5c;
    }

    // Step3: inner hash = SHA1(i_key_pad || msg)
    {
        sha1_context ictx;
        SHA1Init(&ictx);
        SHA1Update(&ictx, i_key_pad, SHA1_BLOCK_SIZE);
        SHA1Update(&ictx, data, data_len);
        SHA1Final(&ictx, inner_digest);
    }

    // Step4: outer hash = SHA1(o_key_pad || inner_digest)
    {
        sha1_context octx;
        SHA1Init(&octx);
        SHA1Update(&octx, o_key_pad, SHA1_BLOCK_SIZE);
        SHA1Update(&octx, inner_digest, SHA1_DIGEST_LENGTH);
        SHA1Final(&octx, digest);
    }
}