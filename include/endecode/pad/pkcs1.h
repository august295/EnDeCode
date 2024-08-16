#ifndef __PKCS1_H__
#define __PKCS1_H__

#include <stdint.h>
#include <stdlib.h> // rand()
#include <string.h> // memcpy()

#ifdef __cplusplus
extern "C"
{
#endif

// PKCS#1 v1.5 填充函数
static int pkcs1_v1_5_pad(uint8_t* padded, size_t padded_len, const uint8_t* msg, size_t msg_len)
{
    // Check if the message is too long
    if (msg_len > padded_len - 11)
    {
        return 1;
    }

    // Start with the first two bytes
    padded[0] = 0x00;
    padded[1] = 0x02;

    // Fill the padding string (PS) with non-zero random bytes
    for (size_t i = 2; i < padded_len - msg_len - 1; i++)
    {
        padded[i] = rand() % 0xFF + 1;
    }

    // End of the padding string with 0x00
    padded[padded_len - msg_len - 1] = 0x00;

    // Copy the message after the padding
    memcpy(padded + padded_len - msg_len, msg, msg_len);
    return 0;
}

// PKCS#1 v1.5 去除填充函数
static int pkcs1_v1_5_unpad(uint8_t* msg, size_t* msg_len, const uint8_t* padded, size_t padded_len)
{
    // The padded message must start with 0x00 0x02
    if (padded[0] != 0x00 || padded[1] != 0x02)
    {
        return 1;
    }

    // Look for the 0x00 separator
    size_t i;
    for (i = 2; i < padded_len; i++)
    {
        if (padded[i] == 0x00)
        {
            break;
        }
    }

    // Check if the separator was found
    if (i == padded_len)
    {
        return 1;
    }

    // The message starts right after the 0x00
    *msg_len = padded_len - i - 1;
    memcpy(msg, padded + i + 1, *msg_len);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif