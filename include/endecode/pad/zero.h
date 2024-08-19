#ifndef __ZERO_H__
#define __ZERO_H__

#include <stdint.h>
#include <string.h> // memcpy()

/**
 * @brief 0 填充
 * @param[out]    padded         填充消息
 * @param[in]     block_size     总共块长度
 * @param[in]     message        原消息
 * @param[in]     message_len    原消息长度
 */
static int zero_pad(uint8_t* padded, size_t block_size, const uint8_t* message, size_t message_len)
{
    memset(padded, 0, block_size);
    memcpy(padded, message, message_len);
    return 0;
}

/**
 * @brief 去除 0 填充
 * @param[out]    message        原消息
 * @param[out]    message_len    原消息长度
 * @param[in]     padded         填充消息
 * @param[in]     block_size     总共块长度
 */
static int zero_unpad(uint8_t* message, size_t* message_len, const uint8_t* padded, size_t block_size)
{
    *message_len = block_size;
    for (size_t i = block_size; i > 0; --i)
    {
        if (padded[i - 1] != 0)
        {
            *message_len = i;
            break;
        }
    }
    memcpy(message, padded, *message_len);
    message[*message_len] = '\0';
    return 0;
}

#endif