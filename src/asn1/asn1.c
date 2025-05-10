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

void easy_asn1_create_string(uint8_t tag, size_t length, uint8_t* value, easy_asn1_string_st* str)
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
        node->offset = 0;
        node->level  = 0;
        for (uint32_t i = 0; i < node->children_size; i++)
        {
            easy_asn1_free_tree(node->children[i]);
        }
        node->children_size = 0;
        free(node->children);
        node->children = NULL;
    }
}

char* easy_asn1_tag_name(uint8_t tag)
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

size_t easy_asn1_parse_tag(const uint8_t* data, uint8_t* tag)
{
    *tag = *data;
    return 1;
}

size_t easy_asn1_parse_length(const uint8_t* data, size_t* length)
{
    size_t offset = 1;
    *length       = 0;
    if (*data & 0x80)
    {
        uint8_t num_bytes = *data & 0x7F;
        for (uint8_t i = 0; i < num_bytes; i++)
        {
            *length = (*length << 8) | *(data + 1 + i);
        }
        offset += num_bytes;
    }
    else
    {
        *length = *data;
    }
    return offset;
}

size_t easy_asn1_parse_string(const uint8_t* data, easy_asn1_string_st* str)
{
    size_t offset = 0;
    offset += easy_asn1_parse_tag(data + offset, &str->tag);
    offset += easy_asn1_parse_length(data + offset, &str->length);
    str->value = (uint8_t*)malloc(sizeof(uint8_t) * str->length);
    if (str->value)
    {
        memcpy(str->value, data + offset, str->length);
    }
    return offset + str->length;
}

size_t easy_asn1_parse_construct(uint8_t tag)
{
    size_t ConstructTagLen = sizeof(ConstructTags);
    for (size_t i = 0; i < ConstructTagLen; i++)
    {
        if (tag == ConstructTags[i])
        {
            return 1;
        }
    }
    return 0;
}

size_t easy_asn1_parse_predict(const uint8_t* data, size_t start, size_t length)
{
    size_t  offset = start;
    uint8_t tag    = 0;
    size_t  len    = 0;
    offset += easy_asn1_parse_tag(data + offset, &tag);
    offset += easy_asn1_parse_length(data + offset, &len);
    if (easy_asn1_parse_construct(tag) == 0 || len > length)
    {
        return 0;
    }
    return offset + len;
}

// 解析 ASN.1 数据
void easy_asn1_parse(const uint8_t* data, size_t len, size_t offset, size_t level, easy_asn1_tree_st** node)
{
    // 检查边界条件和无效指针
    if (offset >= len || !node)
    {
        return;
    }

    // 创建新节点
    *node = (easy_asn1_tree_st*)calloc(1, sizeof(easy_asn1_tree_st));
    if (!*node)
    {
        return;
    }

    // 设置节点基本信息
    (*node)->offset        = offset;
    (*node)->level         = level;
    (*node)->children_size = 0;
    (*node)->children      = NULL;

    // 解析当前节点的值
    size_t consumed = easy_asn1_parse_string(data + offset, &(*node)->value);
    if (consumed == 0 || (*node)->value.value == NULL)
    {
        free(*node);
        *node = NULL;
        return;
    }

    uint8_t tag = (*node)->value.tag;
    // 如果是构造类型（如 SEQUENCE/SET），递归解析子节点
    if (tag & CONSTRUCTED)
    {
        size_t children_offset = offset + (consumed - (*node)->value.length);
        size_t end_offset      = children_offset + (*node)->value.length;

        // 临时变量用于遍历和计数子节点
        size_t temp_offset = children_offset;
        size_t child_count = 0;

        // 第一次遍历：计算子节点数量
        while (temp_offset < end_offset)
        {
            uint8_t tag;
            size_t  length;
            size_t  tag_len = easy_asn1_parse_tag(data + temp_offset, &tag);
            size_t  len_len = easy_asn1_parse_length(data + temp_offset + tag_len, &length);

            if (tag_len == 0 || len_len == 0)
                break;

            temp_offset += tag_len + len_len + length;
            child_count++;
        }

        // 分配子节点数组
        (*node)->children_size = child_count;
        (*node)->children      = (easy_asn1_tree_st**)calloc(child_count, sizeof(easy_asn1_tree_st*));
        if (!(*node)->children)
        {
            free((*node)->value.value);
            free(*node);
            *node = NULL;
            return;
        }

        // 第二次遍历：递归解析每个子节点
        temp_offset = children_offset;
        for (size_t i = 0; i < child_count; i++)
        {
            easy_asn1_parse(data, len, temp_offset, level + 1, &(*node)->children[i]);
            if (!(*node)->children[i])
            {
                // 解析失败，清理已分配的资源
                for (size_t j = 0; j < i; j++)
                {
                    if ((*node)->children[j])
                    {
                        free((*node)->children[j]->value.value);
                        free((*node)->children[j]);
                    }
                }
                free((*node)->children);
                free((*node)->value.value);
                free(*node);
                *node = NULL;
                return;
            }
            // 更新下一个子节点的偏移量
            uint8_t tag;
            size_t  length;
            size_t  tag_len = easy_asn1_parse_tag(data + temp_offset, &tag);
            size_t  len_len = easy_asn1_parse_length(data + temp_offset + tag_len, &length);
            temp_offset += tag_len + len_len + length;
        }
    }
    else
    {
        // 对于 BIT STRING，第一个字节是未使用的比特数
        size_t         content_offset = (tag == EASY_ASN1_BIT_STRING) ? 1 : 0;
        const uint8_t* content_data   = (*node)->value.value + content_offset;
        size_t         content_len    = (*node)->value.length - content_offset;

        // 检查内容是否看起来像 ASN.1 结构（以有效的标签开头）
        size_t predict_len = easy_asn1_parse_predict(content_data, 0, content_len);
        if (predict_len == content_len)
        {
            // 计算嵌套内容的原始偏移量
            size_t nested_offset = offset + consumed - (*node)->value.length + content_offset;

            // 尝试解析嵌套内容，使用正确的原始偏移量
            easy_asn1_tree_st* child = NULL;
            easy_asn1_parse(data, len, nested_offset, level + 1, &child);

            if (child)
            {
                (*node)->children_size = 1;
                (*node)->children      = (easy_asn1_tree_st**)calloc(1, sizeof(easy_asn1_tree_st*));
                if ((*node)->children)
                {
                    (*node)->children[0] = child;
                }
                else
                {
                    (*node)->children_size = 0;
                    easy_asn1_free_tree(child);
                }
            }
        }
    }
}

// 编码 ASN.1 的长度字段
size_t easy_asn1_encode_length(size_t length, uint8_t* out)
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

    if ((node->value.tag & CONSTRUCTED) && node->children_size > 0)
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
