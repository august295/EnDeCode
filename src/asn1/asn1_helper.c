#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "endecode/asn1/asn1_helper.h"

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

char* easy_asn1_print_integer(const uint8_t* value, size_t len)
{
    if (len == 0)
    {
        // 长度为 0，返回 "0"
        char* zero = (char*)malloc(2);
        strcpy(zero, "0");
        return zero;
    }

    // 复制数据，避免修改原始数据
    uint8_t* buf = (uint8_t*)malloc(len + 1);
    memcpy(buf, value, len);

    // 动态结果存储（初始 64 字节）
    size_t res_cap = 64, res_len = 0;
    char*  res = (char*)malloc(res_cap);

    while (len > 0)
    {
        uint32_t rem = 0;

        for (size_t i = 0; i < len; ++i)
        {
            uint32_t val = (rem << 8) | buf[i];
            buf[i]       = val / 10;
            rem          = val % 10;
        }

        // 存储余数作为当前最低位
        if (res_len + 1 >= res_cap)
        {
            res_cap *= 2;
            res = (char*)realloc(res, res_cap);
        }
        res[res_len++] = '0' + rem;

        // 去掉前导 0
        size_t leading_zeros = 0;
        while (leading_zeros < len && buf[leading_zeros] == 0)
            leading_zeros++;

        if (leading_zeros > 0)
        {
            memmove(buf, buf + leading_zeros, len - leading_zeros);
            len -= leading_zeros;
        }
    }

    // 如果是 0
    if (res_len == 0)
    {
        free(res);
        char* zero = (char*)malloc(2);
        strcpy(zero, "0");
        return zero;
    }

    // 构造结果字符串（反转）
    char* result = (char*)malloc(res_len + 1);
    for (size_t i = 0; i < res_len; ++i)
        result[i] = res[res_len - 1 - i];
    result[res_len] = '\0';

    free(buf);
    free(res);
    return result;
}

int easy_asn1_print_string_try(const uint8_t* value, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (value[i] < 20 || value[i] > 127)
        {
            return 0;
        }
    }
    return 1;
}

void easy_asn1_print_string(easy_asn1_string_st* str, size_t print_value)
{
    printf("Tag: %02X, Length: %llu", str->tag, str->length);
    if (print_value == 0)
    {
        printf(", Value: ");
        switch (str->tag)
        {
        case EASY_ASN1_BOOLEAN:
            printf("%s", str->value[0] == 0 ? "FALSE" : "TRUE");
            break;
        case EASY_ASN1_INTEGER: {
            char* temp = easy_asn1_print_integer(str->value, str->length);
            printf("%s", temp);
        }
        break;
        case EASY_ASN1_NULL:
            printf("%s", "NULL");
            break;
        case EASY_ASN1_OBJECT: {
            char oid_str[MAX_OID] = {0};
            oid_to_string(str->value, str->length, oid_str);
            printf("%s", oid_str);
        }
        break;
        case EASY_ASN1_UTF8STRING:
        case EASY_ASN1_NUMERICSTRING:
        case EASY_ASN1_PRINTABLESTRING:
        case EASY_ASN1_T61STRING:
        case EASY_ASN1_VIDEOTEXSTRING:
        case EASY_ASN1_IA5STRING:
            printf("%.*s", (int)str->length, str->value);
            break;
        case EASY_ASN1_UTCTIME: {
            char time[21] = {0};
            convertUTCTimeToStandard(str->value, 8, time);
            printf("%s", time);
        }
        break;
        case EASY_ASN1_GENERALIZEDTIME: {
            char time[21] = {0};
            convertGeneralizedTimeToStandard(str->value, 8, time);
            printf("%s", time);
        }
        break;
        default:
            if (easy_asn1_print_string_try(str->value, str->length))
            {
                printf("%.*s", (int)str->length, str->value);
            }
            else
            {
                for (size_t i = 0; i < str->length; i++)
                {
                    printf("%02X ", str->value[i]);
                }
            }
            break;
        }
    }
    printf("\n");
}

void easy_asn1_print_tree(easy_asn1_tree_st* node)
{
    if (node)
    {
        for (uint32_t i = 0; i < node->level; i++)
        {
            printf("    ");
        }
        printf("Offset: %llu, ", node->offset);
        easy_asn1_print_string(&node->value, node->children_size);

        for (uint32_t i = 0; i < node->children_size; i++)
        {
            easy_asn1_print_tree(node->children[i]);
        }
    }
}