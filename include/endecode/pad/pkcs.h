#ifndef __PKCS_H__
#define __PKCS_H__

#include <stdint.h>
#include <stdlib.h> // rand()
#include <string.h> // memcpy()

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @brief PKCS#1 v1.5 填充函数
 * @param[out]    padded         填充消息
 * @param[in]     block_size     总共块长度
 * @param[in]     message        原消息
 * @param[in]     message_len    原消息长度
 */
static int pkcs1_v1_5_pad(uint8_t* padded, size_t block_size, const uint8_t* message, size_t message_len)
{
    // Check if the message is too long
    if (message_len > block_size - 11)
    {
        return 1;
    }

    // Start with the first two bytes
    padded[0] = 0x00;
    padded[1] = 0x02;

    // Fill the padding string (PS) with non-zero random bytes
    for (size_t i = 2; i < block_size - message_len - 1; i++)
    {
        padded[i] = rand() % 0xFF + 1;
    }

    // End of the padding string with 0x00
    padded[block_size - message_len - 1] = 0x00;

    // Copy the message after the padding
    memcpy(padded + block_size - message_len, message, message_len);
    return 0;
}

/**
 * @brief PKCS#1 v1.5 去除填充函数
 * @param[out]    message        原消息
 * @param[out]    message_len    原消息长度
 * @param[in]     padded         填充消息
 * @param[in]     block_size     总共块长度
 */
static int pkcs1_v1_5_unpad(uint8_t* message, size_t* message_len, const uint8_t* padded, size_t block_size)
{
    // The padded message must start with 0x00 0x02
    // 0 at the beginning of numbers are not displayed
    if (padded[0] != 0x02)
    {
        return 1;
    }

    // Look for the 0x00 separator
    size_t i;
    for (i = 1; i < block_size; i++)
    {
        if (padded[i] == 0x00)
        {
            break;
        }
    }

    // Check if the separator was found
    if (i == block_size)
    {
        return 1;
    }

    // The message starts right after the 0x00
    *message_len = block_size - i - 1;
    memcpy(message, padded + i + 1, *message_len);

    return 0;
}

#ifdef __cplusplus
}
#endif

#endif