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

#endif