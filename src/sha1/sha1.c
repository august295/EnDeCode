#include "sha1/sha1.h"

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