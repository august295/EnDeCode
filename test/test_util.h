#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include <stdint.h>

bool my_equal_array_8bit(uint8_t* data, uint8_t* temp, uint32_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (data[i] != temp[i])
        {
            return false;
        }
    }
    return true;
}

bool my_equal_array_32bit(uint32_t* data, uint32_t* temp, uint32_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (data[i] != temp[i])
        {
            return false;
        }
    }
    return true;
}

bool my_equal_array_32bit_1bit(uint32_t* data, uint32_t* temp, uint32_t len_bit)
{
    size_t i   = 0;
    int    len = len_bit / 32;
    for (i = 0; i < len; i++)
    {
        if (data[i] != temp[i])
        {
            return false;
        }
    }
    if (len_bit % 32)
    {
        i++;
        int shift = 32 - (len_bit % 32);
        if ((data[i] >> shift) != (data[i] >> shift))
        {
            return false;
        }
    }
    return true;
}

/**
 * @brief 32bit 转 8bit
 * @param[out]    output         8bit 数据
 * @param[in]     input          32bit 数据
 * @param[in]     len            32bit 数据长度
 * @param[in]     little         1-小端，0-大端
 */
void bit32_to_8(uint8_t* output, const uint32_t* input, int len, int little)
{
    int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        if (little)
        {
            output[j]     = (uint8_t)((input[i] >> 0) & 0xff);
            output[j + 1] = (uint8_t)((input[i] >> 8) & 0xff);
            output[j + 2] = (uint8_t)((input[i] >> 16) & 0xff);
            output[j + 3] = (uint8_t)((input[i] >> 24) & 0xff);
        }
        else
        {
            output[j]     = (uint8_t)((input[i] >> 24) & 0xff);
            output[j + 1] = (uint8_t)((input[i] >> 16) & 0xff);
            output[j + 2] = (uint8_t)((input[i] >> 8) & 0xff);
            output[j + 3] = (uint8_t)((input[i] >> 0) & 0xff);
        }
    }
}

/**
 * @brief 8bit 转 32bit
 * @param[out]    output         32bit 数据
 * @param[in]     input          8bit 数据
 * @param[in]     len            8bit 数据长度
 * @param[in]     little         1-小端，0-大端
 */
void bit8_to_32(uint32_t* output, const uint8_t* input, int len, int little)
{
    int i, j;
    for (i = 0, j = 0; j < len; i++, j += 4)
    {
        if (little)
        {
            output[i] = (((uint32_t)input[j]) << 0) |
                        (((uint32_t)input[j + 1]) << 8) |
                        (((uint32_t)input[j + 2]) << 16) |
                        (((uint32_t)input[j + 3]) << 24);
        }
        else
        {
            output[i] = (((uint32_t)input[j]) << 24) |
                        (((uint32_t)input[j + 1]) << 16) |
                        (((uint32_t)input[j + 2]) << 8) |
                        (((uint32_t)input[j + 3]) << 0);
        }
    }
}

/**
 * @brief 交换 16 位整数的字节顺序
 * @param[in]     value          64bit 整数
 * @return uint16_t              交换后整数
 */
uint16_t swap16(uint16_t value)
{
    return ((value & 0x00FFU) << 8) | ((value & 0xFF00U) >> 8);
}

/**
 * @brief 交换 32 位整数的字节顺序，复用 swap16
 * @param[in]     value          64bit 整数
 * @return uint32_t              交换后整数
 */
uint32_t swap32(uint32_t value)
{
    return ((uint32_t)swap16((uint16_t)(value & 0xFFFFU)) << 16) | (uint32_t)swap16((uint16_t)(value >> 16));
}

/**
 * @brief 交换 64 位整数的字节顺序，复用 swap32
 * @param[in]     value          64bit 整数
 * @return uint64_t              交换后整数
 */
uint64_t swap64(uint64_t value)
{
    return ((uint64_t)swap32((uint32_t)(value & 0xFFFFFFFFULL)) << 32) | (uint64_t)swap32((uint32_t)(value >> 32));
}

/**
 * @brief 16进制转字符串
 * @param[in]     hex            16进制
 * @param[in]     hex_len        16进制长度
 * @param[in]     upper          1-大写，0-小写
 * @param[out]    ascii          字符串
 */
void hex_to_ascii(const uint8_t* hex, int hex_len, int upper, uint8_t* ascii)
{
    uint8_t temp[2];
    for (int i = 0; i < hex_len; i++)
    {
        temp[0] = (hex[i] & 0xF0) >> 4;
        temp[1] = (hex[i] & 0x0F);
        for (int j = 0; j < 2; j++)
        {
            if (temp[j] < 10)
            {
                temp[j] += '0';
            }
            else
            {
                temp[j] = upper ? temp[j] - 10 + 'A' : temp[j] - 10 + 'a';
            }
            *ascii++ = temp[j];
        }
    }
    *ascii++ = '\0';
}

#endif