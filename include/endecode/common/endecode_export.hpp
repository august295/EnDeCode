#ifndef ENDECODE_EXPORT_HPP
#define ENDECODE_EXPORT_HPP

/**
 * @brief 跨平台导入导出宏
 */
#if defined(WIN32) || defined(_WIN32)
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

#endif
