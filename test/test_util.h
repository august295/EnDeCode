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