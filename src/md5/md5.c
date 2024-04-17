#include "md5.h"

// leftrotate function definition
#define LEFTROTATE(x, c) (((x) << (c)) | ((x) >> (32 - (c))))

// 四个固定变量
#define A 0x67452301
#define B 0xefcdab89
#define C 0x98badcfe
#define D 0x10325476

// clang-format off
/**
 * 常量ti
 * 公式 unsigned int(abs(sin(i+1))*(2pow32))
 */
const static uint32_t k[64] = {
    0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
    0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821,
    0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d, 0x02441453, 0xd8a1e681, 0xe7d3fbc8,
    0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a,
    0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
    0x289b7ec6, 0xeaa127fa, 0xd4ef3085, 0x04881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665,
    0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
    0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
};

// 每次左移位数
const static uint32_t r[64] = {
    7, 12, 17, 22, 7, 12, 17, 22,
    7, 12, 17, 22, 7, 12, 17, 22,
    5, 9, 14, 20, 5, 9, 14, 20,
    5, 9, 14, 20, 5, 9, 14, 20,
    4, 11, 16, 23, 4, 11, 16, 23,
    4, 11, 16, 23, 4, 11, 16, 23,
    6, 10, 15, 21, 6, 10, 15, 21,
    6, 10, 15, 21, 6, 10, 15, 21
};
// clang-format on

void to_bytes(const uint32_t val, uint8_t* bytes)
{
    bytes[0] = (uint8_t)val;
    bytes[1] = (uint8_t)(val >> 8);
    bytes[2] = (uint8_t)(val >> 16);
    bytes[3] = (uint8_t)(val >> 24);
}

void to_int32(const uint8_t* bytes, uint32_t* val)
{
    *val = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
}

void md5(const uint8_t* initial_msg, uint64_t initial_len, uint8_t* digest)
{
    // 初始化
    uint32_t h0 = A;
    uint32_t h1 = B;
    uint32_t h2 = C;
    uint32_t h3 = D;

    // Message (to prepare)
    uint8_t* msg = NULL;

    uint64_t new_len, offset;
    uint32_t w[16];
    uint32_t a, b, c, d, i, f, g, temp;

    /**
     * 填充函数
     *  - 处理后应满足 bits ≡ 448 (mod512)，字节就是 bytes ≡ 56 (mode64)
     *  - 填充方式为先加一个 1，其它位补零
     *  - 最后加上 64 位的原来长度
     */
    new_len = initial_len + 1;
    while (new_len % (512 / 8) != 448 / 8)
    {
        new_len++;
    }

    msg = (uint8_t*)malloc(new_len + 8);
    memcpy(msg, initial_msg, initial_len);
    // 首部填充 "1"
    msg[initial_len] = 0x80;
    // 后面填充 "0"
    for (offset = initial_len + 1; offset < new_len; offset++)
    {
        msg[offset] = 0;
    }

    // 在缓冲区末尾附加 len 长度，以 bit 为单位
    to_bytes(initial_len * 8, msg + new_len);
    // initial_len>>29 == initial_len*8>>32, 但是防止溢出
    to_bytes(initial_len >> 29, msg + new_len + 4);

    // 处理 512bits
    for (offset = 0; offset < new_len; offset += (512 / 8))
    {
        // 512bits 分为 16*32 bits, 16 组
        for (i = 0; i < 16; i++)
        {
            to_int32(msg + offset + i * 4, &w[i]);
        }
        // 赋值
        a = h0;
        b = h1;
        c = h2;
        d = h3;

        // 主循环
        for (i = 0; i < 64; i++)
        {
            if (i < 16)
            {
                f = (b & c) | ((~b) & d);
                g = i;
            }
            else if (i < 32)
            {
                f = (d & b) | ((~d) & c);
                g = (5 * i + 1) % 16;
            }
            else if (i < 48)
            {
                f = b ^ c ^ d;
                g = (3 * i + 5) % 16;
            }
            else
            {
                f = c ^ (b | (~d));
                g = (7 * i) % 16;
            }

            temp = d;
            d    = c;
            c    = b;
            b    = b + LEFTROTATE((a + f + k[i] + w[g]), r[i]);
            a    = temp;
        }

        // 更新
        h0 += a;
        h1 += b;
        h2 += c;
        h3 += d;
    }

    free(msg);

    // 32 bit 转 4*8 bit，依次存储数据
    to_bytes(h0, digest);
    to_bytes(h1, digest + 4);
    to_bytes(h2, digest + 8);
    to_bytes(h3, digest + 12);
}

void hex_to_ascii(const uint8_t* pHex, int flag, uint8_t* pAscii, int nLen)
{
    uint8_t Nibble[2];
    for (int i = 0; i < nLen; i++)
    {
        Nibble[0] = (pHex[i] & 0xF0) >> 4;
        Nibble[1] = pHex[i] & 0x0F;
        for (int j = 0; j < 2; j++)
        {
            if (Nibble[j] < 10)
            {
                Nibble[j] += 0x30;
            }
            else
            {
                if (Nibble[j] < 16)
                {
                    if (flag == 0)
                    {
                        Nibble[j] = Nibble[j] - 10 + 'A';
                    }
                    else
                    {
                        Nibble[j] = Nibble[j] - 10 + 'a';
                    }
                }
            }
            *pAscii++ = Nibble[j];
        }
    }
    *pAscii++ = '\0';
}

void md5_ascii(const uint8_t* plaintext, uint64_t plaintext_len, int flag, uint8_t* ciphertext)
{
    uint8_t result[16] = {0};
    md5(plaintext, plaintext_len, result);
    hex_to_ascii(result, flag, ciphertext, 16);
}
