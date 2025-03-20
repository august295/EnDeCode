#include <memory.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "endecode/asn1/cert.h"

void oid_to_string(const uint8_t* oid_bytes, size_t oid_len, char* oid_str)
{
    size_t   i;
    uint32_t value  = 0;
    size_t   offset = 0;

    // Process the first byte to extract the first two parts (X.Y)
    uint8_t first_byte = oid_bytes[0];
    if (first_byte >= 80)
    {
        // X is 2, Y is first_byte - 80
        oid_str[offset++] = '2';
        oid_str[offset++] = '.';
        oid_str[offset++] = (first_byte - 80) + '0';
    }
    else
    {
        // X is first_byte / 40, Y is first_byte % 40
        oid_str[offset++] = (first_byte / 40) + '0';
        oid_str[offset++] = '.';
        oid_str[offset++] = (first_byte % 40) + '0';
    }

    // Process the remaining bytes
    for (i = 1; i < oid_len; i++)
    {
        if (oid_bytes[i] & 0x80)
        {
            value = (value << 7) | (oid_bytes[i] & 0x7F);
        }
        else
        {
            value = (value << 7) | oid_bytes[i];
            sprintf(oid_str + offset, ".%u", value);
            offset = strlen(oid_str);
            value  = 0;
        }
    }
}

int string_to_oid(const char* oid_str, uint8_t* oid_bytes, size_t* oid_len)
{
    size_t len      = 0;
    char*  str_copy = strdup(oid_str);
    if (!str_copy)
        return -1;

    char* token = strtok(str_copy, ".");
    if (!token)
    {
        free(str_copy);
        return -1;
    }
    int first_part = atoi(token);

    token = strtok(NULL, ".");
    if (!token)
    {
        free(str_copy);
        return -1;
    }
    int second_part = atoi(token);

    // Validate first and second parts
    if (first_part < 0 || first_part > 2)
    {
        free(str_copy);
        return -1;
    }
    if (first_part == 0 || first_part == 1)
    {
        if (second_part < 0 || second_part >= 40)
        {
            free(str_copy);
            return -1;
        }
    }
    else
    {
        if (second_part < 0 || second_part > 175)
        {
            free(str_copy);
            return -1;
        }
    }

    uint8_t first_byte = (uint8_t)(first_part * 40 + second_part);
    oid_bytes[len++]   = first_byte;

    // Process remaining parts
    while ((token = strtok(NULL, ".")) != NULL)
    {
        uint32_t value = (uint32_t)atoi(token);
        if (value == 0)
        {
            oid_bytes[len++] = 0;
            continue;
        }

        uint8_t buffer[16];
        int     buffer_len = 0;
        do
        {
            buffer[buffer_len++] = (uint8_t)(value & 0x7F);
            value >>= 7;
        } while (value > 0);

        // Reverse and set high bit
        for (int j = buffer_len - 1; j >= 0; j--)
        {
            if (j != 0)
            {
                oid_bytes[len++] = buffer[j] | 0x80;
            }
            else
            {
                oid_bytes[len++] = buffer[j];
            }
            if (len >= 0xFFFF)
            {
                free(str_copy);
                return -1;
            }
        }
    }

    free(str_copy);
    *oid_len = len;
    return 0;
}

// 转换UTCTime为标准时间格式
void convertUTCTimeToStandard(const char* utcTime, size_t utcOffset, char* standardTime)
{
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // 解析UTCTime
    if (sscanf(utcTime, "%2d%2d%2d%2d%2d%2dZ", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6)
    {
        return;
    }

    if (tm.tm_year < 50)
    {
        tm.tm_year += 100;
    }

    tm.tm_mon -= 1;
    tm.tm_hour += utcOffset;
    mktime(&tm);

    // 格式化输出为标准时间
    strftime(standardTime, 20, "%Y-%m-%d %H:%M:%S", &tm);
}

// 转换GeneralizedTime为标准时间格式
void convertGeneralizedTimeToStandard(const char* generalizedTime, size_t utcOffset, char* standardTime)
{
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));

    // 解析GeneralizedTime
    if (sscanf(generalizedTime, "%4d%2d%2d%2d%2d%2dZ", &tm.tm_year, &tm.tm_mon, &tm.tm_mday, &tm.tm_hour, &tm.tm_min, &tm.tm_sec) != 6)
    {
        return;
    }

    tm.tm_year -= 1900;
    tm.tm_mon -= 1;
    tm.tm_hour += utcOffset;
    mktime(&tm);

    // 格式化输出为标准时间
    strftime(standardTime, 20, "%Y-%m-%d %H:%M:%S", &tm);
}
