#include <malloc.h>
#include <memory.h>
#include <stdio.h>

#include "endecode/asn1/asn1.h"

void easy_asn1_init_string(easy_asn1_string_st* str)
{
    str->tag    = 0;
    str->length = 0;
    str->value  = NULL;
}

void easy_asn1_init_tree(easy_asn1_tree_st* node)
{
    easy_asn1_init_string(&node->value);
    node->level         = 0;
    node->children_size = 0;
    node->children      = NULL;
}

void easy_asn1_create_string(uint8_t tag, uint32_t length, uint8_t* value, easy_asn1_string_st* str)
{
    str->tag    = tag;
    str->length = length;
    str->value  = (uint8_t*)calloc(1, length + 1);
    if (str->value == NULL)
    {
        str->tag    = 0;
        str->length = 0;
        return;
    }
    memcpy(str->value, value, length);
}

void easy_asn1_copy_string(easy_asn1_string_st* src, easy_asn1_string_st* dest)
{
    dest->tag    = src->tag;
    dest->length = src->length;
    dest->value  = (uint8_t*)calloc(1, src->length + 1);
    if (dest->value == NULL)
    {
        dest->tag    = 0;
        dest->length = 0;
        return;
    }
    memcpy(dest->value, src->value, src->length);
}

void easy_asn1_print_string(easy_asn1_string_st* str, int print_value)
{
    printf("Tag: %02X, Length: %08X, Value: ", str->tag, str->length);
    if (print_value == 0)
    {
        for (size_t i = 0; i < str->length; i++)
        {
            printf("%02X ", str->value[i]);
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
        easy_asn1_print_string(&node->value, node->children_size);

        for (uint32_t i = 0; i < node->children_size; i++)
        {
            easy_asn1_print_tree(node->children[i]);
        }
    }
}

void easy_asn1_free_string(easy_asn1_string_st* str)
{
    if (str)
    {
        free(str->value);
        str->value = NULL;
    }
}

void easy_asn1_free_tree(easy_asn1_tree_st* node)
{
    if (node)
    {
        easy_asn1_free_string(&node->value);
        node->level = 0;
        for (uint32_t i = 0; i < node->children_size; i++)
        {
            easy_asn1_free_tree(node->children[i]);
        }
        node->children_size = 0;
        free(node->children);
        node->children = NULL;
    }
}

const char* easy_asn1_tag_name(int tag)
{
    switch (tag)
    {
    case EASY_ASN1_BOOLEAN: return "BOOLEAN";
    case EASY_ASN1_INTEGER: return "INTEGER";
    case EASY_ASN1_BIT_STRING: return "BIT_STRING";
    case EASY_ASN1_OCTET_STRING: return "OCTET_STRING";
    case EASY_ASN1_NULL: return "NULL";
    case EASY_ASN1_OBJECT: return "OBJECT_IDENTIFIER";
    case EASY_ASN1_OBJECT_DESCRIPTOR: return "OBJECT_DESCRIPTOR";
    case EASY_ASN1_EXTERNAL: return "EXTERNAL";
    case EASY_ASN1_REAL: return "REAL";
    case EASY_ASN1_ENUMERATED: return "ENUMERATED";
    case EASY_ASN1_UTF8STRING: return "UTF8STRING";
    case EASY_ASN1_SEQUENCE: return "SEQUENCE";
    case EASY_ASN1_SET: return "SET";
    case EASY_ASN1_NUMERICSTRING: return "NUMERICSTRING";
    case EASY_ASN1_PRINTABLESTRING: return "PRINTABLESTRING";
    case EASY_ASN1_T61STRING: return "T61STRING";
    case EASY_ASN1_VIDEOTEXSTRING: return "VIDEOTEXSTRING";
    case EASY_ASN1_IA5STRING: return "IA5STRING";
    case EASY_ASN1_UTCTIME: return "UTCTIME";
    case EASY_ASN1_GENERALIZEDTIME: return "GENERALIZEDTIME";
    case EASY_ASN1_GRAPHICSTRING: return "GRAPHICSTRING";
    case EASY_ASN1_ISO64STRING: return "ISO64STRING";
    case EASY_ASN1_GENERALSTRING: return "GENERALSTRING";
    case EASY_ASN1_UNIVERSALSTRING: return "UNIVERSALSTRING";
    case EASY_ASN1_BMPSTRING: return "BMPSTRING";
    case ESAY_X509_ASN1_SEQUENCE: return "SEQUENCE";
    case ESAY_X509_ASN1_SET: return "SET";
    case ESAY_X509_CONTEXT0: return "CONTEXT_SPECIFIC [0]";
    case ESAY_X509_CONTEXT1: return "CONTEXT_SPECIFIC [1]";
    case ESAY_X509_CONTEXT2: return "CONTEXT_SPECIFIC [2]";
    case ESAY_X509_CONTEXT3: return "CONTEXT_SPECIFIC [3]";
    }

    return NULL;
}

const uint8_t* easy_asn1_parse_tag(const uint8_t* data, uint8_t* tag)
{
    *tag = *data++;
    return data;
}

const uint8_t* easy_asn1_parse_length(const uint8_t* data, uint32_t* length)
{
    *length = 0;
    if (*data & 0x80)
    {
        uint8_t num_bytes = *data++ & 0x7F;
        for (uint8_t i = 0; i < num_bytes; i++)
        {
            *length = (*length << 8) | *data++;
        }
    }
    else
    {
        *length = *data++;
    }
    return data;
}

const uint8_t* easy_asn1_parse_string(const uint8_t* data, easy_asn1_string_st* str)
{
    data       = easy_asn1_parse_tag(data, &str->tag);
    data       = easy_asn1_parse_length(data, &str->length);
    str->value = (uint8_t*)malloc(sizeof(uint8_t) * str->length);
    if (str->value)
    {
        memcpy(str->value, data, str->length);
    }
    return data + str->length;
}

// 解析 ASN.1 数据
void easy_asn1_parse(const uint8_t* data, size_t len, int level, easy_asn1_tree_st** node)
{
    if (len == 0 || data == NULL || node == NULL)
    {
        return;
    }

    // 如果传入的 node 是 NULL，分配内存
    if (*node == NULL)
    {
        *node = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
        if (*node == NULL)
        {
            return;
        }
        memset(*node, 0, sizeof(easy_asn1_tree_st));
    }

    // 解析 ASN.1 标签（假设第一个字节是标签）
    easy_asn1_parse_string(data, &(*node)->value);
    (*node)->level         = level;
    (*node)->children_size = 0;
    (*node)->children      = NULL;

    // 判断是否溢出
    if ((*node)->value.length > len)
    {
        return;
    }

    // 如果当前节点是嵌套结构（如 SEQUENCE 或 SET），递归解析子节点
    if ((*node)->value.tag & 0x20)
    {
        size_t child_offset    = 0;
        (*node)->children_size = 0;

        // 预分配子节点数组（假设最多 10 个子节点）
        (*node)->children = (easy_asn1_tree_st**)malloc(10 * sizeof(easy_asn1_tree_st*));
        if ((*node)->children == NULL)
        {
            return;
        }

        // 递归解析子节点
        while (child_offset < (*node)->value.length)
        {
            (*node)->children[(*node)->children_size] = NULL;
            easy_asn1_parse((*node)->value.value + child_offset, (*node)->value.length - child_offset, level + 1, &((*node)->children[(*node)->children_size]));

            // 更新子节点偏移量
            child_offset += 1;
            if (((*node)->value.value[child_offset] & 0x80) == 0)
            {
                child_offset += 1;
            }
            else
            {
                child_offset += 1 + ((*node)->value.value[child_offset] & 0x7F);
            }
            child_offset += (*node)->children[(*node)->children_size]->value.length;
            (*node)->children_size++;
        }
    }
}

// 编码 ASN.1 的长度字段
size_t easy_asn1_encode_length(uint32_t length, uint8_t* out)
{
    if (length < 128)
    {
        if (out)
            out[0] = (uint8_t)length;
        return 1;
    }
    else
    {
        int      num_bytes = 0;
        uint32_t tmp       = length;
        while (tmp)
        {
            tmp >>= 8;
            num_bytes++;
        }
        if (out)
        {
            out[0] = 0x80 | num_bytes;
            for (int i = 0; i < num_bytes; ++i)
            {
                out[num_bytes - i] = (length >> (i * 8)) & 0xFF;
            }
        }
        return 1 + num_bytes;
    }
}

size_t easy_asn1_serialize(easy_asn1_tree_st* node, uint8_t* buffer)
{
    if (!node)
        return 0;

    size_t offset = 0;

    // 写 tag
    if (buffer)
        buffer[offset] = node->value.tag;
    offset += 1;

    uint8_t* content_buf = NULL;
    size_t   content_len = 0;

    if ((node->value.tag & 0x20) && node->children_size > 0)
    {
        // 构建结构类型（SEQUENCE/SET）内容部分：递归 children
        for (uint32_t i = 0; i < node->children_size; ++i)
        {
            content_len += easy_asn1_serialize(node->children[i], NULL);
        }

        // 写入 Length
        size_t len_len = easy_asn1_encode_length((uint32_t)content_len, buffer ? buffer + offset : NULL);
        offset += len_len;

        // 写入 children 内容
        for (uint32_t i = 0; i < node->children_size; ++i)
        {
            offset += easy_asn1_serialize(node->children[i], buffer ? buffer + offset : NULL);
        }
    }
    else
    {
        // 普通类型，直接使用 value
        content_len = node->value.length;

        // 写入 Length
        size_t len_len = easy_asn1_encode_length((uint32_t)content_len, buffer ? buffer + offset : NULL);
        offset += len_len;

        // 写入 Value
        if (buffer && node->value.value && content_len > 0)
        {
            memcpy(buffer + offset, node->value.value, content_len);
        }
        offset += content_len;
    }

    return offset;
}
