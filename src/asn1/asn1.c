#include "endecode/asn1/asn1.h"

const char* asn1_tag_name(int tag)
{
    switch (tag)
    {
    case V_ASN1_BOOLEAN: return "BOOLEAN";
    case V_ASN1_INTEGER: return "INTEGER";
    case V_ASN1_BIT_STRING: return "BIT_STRING";
    case V_ASN1_OCTET_STRING: return "OCTET_STRING";
    case V_ASN1_NULL: return "NULL";
    case V_ASN1_OBJECT: return "OBJECT";
    case V_ASN1_OBJECT_DESCRIPTOR: return "OBJECT_DESCRIPTOR";
    case V_ASN1_EXTERNAL: return "EXTERNAL";
    case V_ASN1_REAL: return "REAL";
    case V_ASN1_ENUMERATED: return "ENUMERATED";
    case V_ASN1_UTF8STRING: return "UTF8STRING";
    case V_ASN1_SEQUENCE: return "SEQUENCE";
    case V_ASN1_SET: return "SET";
    case V_ASN1_NUMERICSTRING: return "NUMERICSTRING";
    case V_ASN1_PRINTABLESTRING: return "PRINTABLESTRING";
    case V_ASN1_T61STRING: return "T61STRING";
    case V_ASN1_VIDEOTEXSTRING: return "VIDEOTEXSTRING";
    case V_ASN1_IA5STRING: return "IA5STRING";
    case V_ASN1_UTCTIME: return "UTCTIME";
    case V_ASN1_GENERALIZEDTIME: return "GENERALIZEDTIME";
    case V_ASN1_GRAPHICSTRING: return "GRAPHICSTRING";
    case V_ASN1_ISO64STRING: return "ISO64STRING";
    case V_ASN1_GENERALSTRING: return "GENERALSTRING";
    case V_ASN1_UNIVERSALSTRING: return "UNIVERSALSTRING";
    case V_ASN1_BMPSTRING: return "BMPSTRING";
    }

    return NULL;
}

const uint8_t* asn1_parse_tag(const uint8_t* data, uint8_t* tag)
{
    *tag = *data++;
    return data;
}

const uint8_t* asn1_parse_length(const uint8_t* data, uint32_t* length)
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

int get_byte_length(uint32_t value)
{
    int byte_length = 0;
    while (value > 0)
    {
        byte_length++;
        value >>= 8;
    }
    return byte_length;
}

const uint8_t* asn1_parse_string(const uint8_t* data, struct asn1_string_st* str)
{
    data       = asn1_parse_tag(data, &str->tag);
    data       = asn1_parse_length(data, &str->length);
    str->value = malloc(sizeof(uint8_t) * str->length);
    memcpy(str->value, data, str->length);
    return data + str->length;
}

struct asn1_tree_st* asn1_create_node(uint8_t tag, uint32_t length, uint8_t* value, uint32_t level)
{
    struct asn1_tree_st* node = (struct asn1_tree_st*)malloc(sizeof(struct asn1_tree_st));
    node->value.tag           = tag;
    node->value.length        = length;
    node->value.value         = (uint8_t*)malloc(length);
    memcpy(node->value.value, value, length);
    node->level         = level;
    node->children_size = 0;
    node->children      = NULL;
    return node;
}

void asn1_free_tree(struct asn1_tree_st* node)
{
    if (node)
    {
        for (uint32_t i = 0; i < node->children_size; i++)
        {
            asn1_free_tree(node->children[i]);
        }
        free(node->value.value);
        free(node->children);
        free(node);
    }
}

struct asn1_tree_st* asn1_parse(const uint8_t* data, size_t len, int level)
{
    const uint8_t*        end     = data + len;
    struct asn1_tree_st*  root    = NULL;
    struct asn1_tree_st** current = &root;

    while (data < end)
    {
        struct asn1_string_st str;
        asn1_parse_string(data, &str);
        *current = asn1_create_node(str.tag, str.length, str.value, level);
        free(str.value);

        if ((str.tag & 0x20) != 0)
        {
            const uint8_t* child_data   = (*current)->value.value;
            uint32_t       child_length = (*current)->value.length;
            (*current)->children_size   = 0;

            while (child_length != 0)
            {
                struct asn1_tree_st* child = asn1_parse(child_data, child_length, level + 1);
                if (child)
                {
                    (*current)->children_size++;
                    (*current)->children                                = (struct asn1_tree_st**)realloc((*current)->children, (*current)->children_size * sizeof(struct asn1_tree_st*));
                    (*current)->children[(*current)->children_size - 1] = child;

                    uint32_t child_total_length = 2 + child->value.length;
                    if (child->value.length > 0x7f)
                    {
                        child_total_length += get_byte_length(child->value.length);
                    }

                    child_data += child_total_length;
                    child_length -= child_total_length;
                }
                else
                {
                    break;
                }
            }
        }
        data += len;
        current = &((*current)->children[(*current)->children_size - 1]);
    }
    return root;
}