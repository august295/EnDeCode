#ifndef __TEST_UTIL_H__
#define __TEST_UTIL_H__

#include <stdint.h>

bool my_equal(uint8_t* data, uint8_t* temp, uint32_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (data[i] != temp[i])
        {
            return false;
        }
    }
    return true;
}

#endif