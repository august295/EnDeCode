#ifndef ENDECODE_EXPORT_H
#define ENDECODE_EXPORT_H

/**
 * @brief 跨平台导入导出宏
 */
#ifdef ENDECODE_STATIC
    #define ENDECODE_API
#else
    #if defined(_WIN32)
        #ifdef ENDECODE_EXPORT
            #define ENDECODE_API __declspec(dllexport)
        #else
            #define ENDECODE_API __declspec(dllimport)
        #endif
    #elif defined(__linux__)
        #define ENDECODE_API __attribute__((visibility("default")))
    #else
        #define ENDECODE_API
    #endif
#endif

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief   16进制转字符串
 * @param   hex                     [IN]        16进制数据
 * @param   hex_len                 [IN]        16进制数据长度
 * @param   upper                   [IN]        1-大写，0-小写
 * @param   ascii                   [IN/OUT]    字符串
 */
static void hex_to_ascii(const uint8_t* hex, int hex_len, int upper, uint8_t* ascii)
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

/**
 * @brief   字符串转16进制数据
 * @param   ascii           [IN]        字符串（长度必须是偶数）
 * @param   hex             [OUT]       输出的16进制数据
 * @return  int                         返回转换得到的 hex 长度（字节数），错误返回 -1
 */
static int ascii_to_hex(const uint8_t* ascii, int len, uint8_t* hex)
{
    if (len % 2 != 0)
    {
        return -1; // 字符串必须是偶数长度
    }

    for (int i = 0; i < len / 2; i++)
    {
        uint8_t high = ascii[i * 2];
        uint8_t low  = ascii[i * 2 + 1];

        // 高 4 bit
        if (high >= '0' && high <= '9')
            high = high - '0';
        else if (high >= 'A' && high <= 'F')
            high = high - 'A' + 10;
        else if (high >= 'a' && high <= 'f')
            high = high - 'a' + 10;
        else
            return -1;

        // 低 4 bit
        if (low >= '0' && low <= '9')
            low = low - '0';
        else if (low >= 'A' && low <= 'F')
            low = low - 'A' + 10;
        else if (low >= 'a' && low <= 'f')
            low = low - 'a' + 10;
        else
            return -1;

        hex[i] = (high << 4) | low;
    }

    return len / 2;
}

/**
 * @brief   按照块大小打印16进制数据
 * @param   data                    [IN]        数据
 * @param   len                     [IN]        数据长度
 * @param   block_size              [IN]        分块长度
 * @param   right                   [IN]        右对齐
 */
static void print_hex(const uint8_t* data, size_t len, size_t block_size, int right)
{
    size_t remainder = 0;
    if (right)
    {
        remainder = len % block_size;
        for (size_t i = 0; i < remainder; i++)
        {
            printf("%02X", data[i]);
        }
        printf(" ");
    }
    size_t i = 0;
    for (i = remainder; i + block_size < len; i = i + block_size)
    {
        for (size_t j = 0; j < block_size; j++)
        {
            printf("%02X", data[i + j]);
        }
        printf(" ");
    }
    for (; i < len; i++)
    {
        printf("%02X", data[i]);
    }
    printf("\n");
}

#endif
