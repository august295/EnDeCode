#include <memory.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "cJSON.h"

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

void ReadOid(const char* filename, OID_MAPPING** oid_mapping, size_t* map_len)
{
    FILE* file = fopen(filename, "r");
    if (file == NULL)
    {
        return;
    }
    // 到文件尾部取文件大小，然后回到文件头部
    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(sizeof(char) * file_size);
    if (-1 == fread(buffer, sizeof(char), file_size, file))
    {
        free(buffer);
        fclose(file);
        return;
    }
    fclose(file);

    cJSON* root = cJSON_ParseWithLength(buffer, file_size);
    free(buffer);
    if (!root)
    {
        cJSON_Delete(root);
    }
    else
    {
        // 计算个数
        *map_len      = 0;
        int root_size = cJSON_GetArraySize(root);
        for (size_t i = 0; i < root_size; i++)
        {
            cJSON* type_node = cJSON_GetArrayItem(root, i);
            *map_len += cJSON_GetArraySize(type_node);
        }

        // 保存数据结构
        *oid_mapping = (OID_MAPPING*)calloc(*map_len, sizeof(OID_MAPPING));
        int index    = 0;
        for (size_t i = 0; i < root_size; i++)
        {
            cJSON* type_node      = cJSON_GetArrayItem(root, i);
            int    type_node_size = cJSON_GetArraySize(type_node);
            for (size_t j = 0; j < type_node_size; j++)
            {
                cJSON* oid_json   = cJSON_GetArrayItem(type_node, j);
                char*  oid        = cJSON_GetObjectItem(oid_json, "oid")->valuestring;
                char*  short_name = cJSON_GetObjectItem(oid_json, "short_name")->valuestring;

                uint8_t oid_bytes[MAX_OID] = {0};
                size_t  oid_len            = 0;
                string_to_oid(oid, oid_bytes, &oid_len);

                int oid_string_len               = strlen(oid);
                (*oid_mapping)[index].oid_string = (char*)malloc(oid_string_len + 1);
                memcpy((*oid_mapping)[index].oid_string, oid, oid_string_len);
                (*oid_mapping)[index].oid_string[oid_string_len] = '\0';
                easy_asn1_create_string(EASY_ASN1_OBJECT, oid_len, oid_bytes, &(*oid_mapping)[index].oid_object);
                int short_name_len             = strlen(short_name);
                (*oid_mapping)[index].oid_name = (char*)malloc(short_name_len + 1);
                memcpy((*oid_mapping)[index].oid_name, short_name, short_name_len);
                (*oid_mapping)[index].oid_name[short_name_len] = '\0';
                index++;
            }
        }
    }
    cJSON_Delete(root);
}

void FreeOid(OID_MAPPING* oid_mapping, size_t map_len)
{
    if (oid_mapping)
    {
        for (size_t i = 0; i < map_len; i++)
        {
            free(oid_mapping[i].oid_string);
            easy_asn1_free_string(&oid_mapping[i].oid_object);
            free(oid_mapping[i].oid_name);
        }
    }
    free(oid_mapping);
    oid_mapping = NULL;
}

void convertUTCTimeToStandard(const char* utcTime, size_t length, size_t utcOffset, char* standardTime)
{
    // 格式必须至少包含 YYMMDDhhmmssZ 共13字节
    if (length < 13)
    {
        return;
    }

    char      buf[5] = {0};
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    memcpy(buf, utcTime, 2);
    tm.tm_year = atoi(buf);
    if (tm.tm_year < 50)
    {
        tm.tm_year += 100;
    }
    memcpy(buf, utcTime + 2, 2);
    tm.tm_mon = atoi(buf) - 1;
    memcpy(buf, utcTime + 4, 2);
    tm.tm_mday = atoi(buf);
    memcpy(buf, utcTime + 6, 2);
    tm.tm_hour = atoi(buf);
    memcpy(buf, utcTime + 8, 2);
    tm.tm_min = atoi(buf);
    memcpy(buf, utcTime + 10, 2);
    tm.tm_sec = atoi(buf);
    // 检查末尾是否是 'Z'
    if (utcTime[12] != 'Z')
    {
        return;
    }
    tm.tm_hour += utcOffset;
    mktime(&tm);

    // 格式化输出为标准时间
    strftime(standardTime, 20, "%Y-%m-%d %H:%M:%S", &tm);
}

void convertGeneralizedTimeToStandard(const char* generalizedTime, size_t length, size_t utcOffset, char* standardTime)
{
    // 格式必须至少包含 YYYYMMDDhhmmssZ 共15字节
    if (length < 15)
    {
        return;
    }

    char      buf[5] = {0};
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    memcpy(buf, generalizedTime, 4);
    tm.tm_year = atoi(buf) - 1900;
    memcpy(buf, generalizedTime + 4, 2);
    tm.tm_mon = atoi(buf) - 1;
    memcpy(buf, generalizedTime + 6, 2);
    tm.tm_mday = atoi(buf);
    memcpy(buf, generalizedTime + 8, 2);
    tm.tm_hour = atoi(buf);
    memcpy(buf, generalizedTime + 10, 2);
    tm.tm_min = atoi(buf);
    memcpy(buf, generalizedTime + 12, 2);
    tm.tm_sec = atoi(buf);
    // 末尾必须是 'Z'
    if (generalizedTime[14] != 'Z')
    {
        return;
    }
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
    printf("Tag: %02X, Length: %zu", str->tag, str->length);
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
            free(temp);
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
            convertUTCTimeToStandard(str->value, str->length, 8, time);
            printf("%s", time);
        }
        break;
        case EASY_ASN1_GENERALIZEDTIME: {
            char time[21] = {0};
            convertGeneralizedTimeToStandard(str->value, str->length, 8, time);
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
        printf("Offset: %zu, ", node->offset);
        easy_asn1_print_string(&node->value, node->children_size);

        for (uint32_t i = 0; i < node->children_size; i++)
        {
            easy_asn1_print_tree(node->children[i]);
        }
    }
}