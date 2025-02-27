#include <memory.h>
#include <stdio.h>
#include <time.h>

#include "endecode/asn1/cert.h"

// 转换UTCTime为标准时间格式
void convertUTCTimeToStandard(const char* utcTime, char* standardTime)
{
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // 解析UTCTime
    sscanf(utcTime, "%2d%2d%2d%2d%2d%2dZ", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    // UTCTime的年份是两位数，需要转换为四位数的年份
    if (tm.tm_year >= 50)
    {
        tm.tm_year += 1900; // 1950-1999
    }
    else
    {
        tm.tm_year += 2000; // 2000-2049
    }

    tm.tm_mon -= 1;         // tm_mon的范围是0-11

    // 格式化输出为标准时间
    strftime(standardTime, 20, "%Y-%m-%d %H:%M:%S", &tm);
}

// 转换GeneralizedTime为标准时间格式
void convertGeneralizedTimeToStandard(const char* generalizedTime, char* standardTime)
{
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // 解析GeneralizedTime
    sscanf(generalizedTime, "%4d%2d%2d%2d%2d%2dZ", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec);

    tm.tm_mon -= 1; // tm_mon的范围是0-11

    // 格式化输出为标准时间
    strftime(standardTime, 20, "%Y-%m-%d %H:%M:%S", &tm);
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