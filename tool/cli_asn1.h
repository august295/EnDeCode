#pragma once

#include <string>

#include <ghc/fs_std.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ranges.h>

#include "endecode/base64/base64.h"
#include "endecode/asn1/asn1.h"
#include "endecode/asn1/asn1_helper.h"
#include "endecode/asn1/cert.h"
#include "cli_util.h"
#include "cli_algorithm.h"

inline std::string easy_asn1_format_string(easy_asn1_string_st* str, size_t print_value)
{
    std::string out = fmt::format("Tag: {:02X}, Length: {}", str->tag, str->length);

    if (print_value == 0)
    {
        out += ", Value: ";
        switch (str->tag)
        {
        case EASY_ASN1_BOOLEAN:
            out += (str->value[0] == 0 ? "FALSE" : "TRUE");
            break;
        case EASY_ASN1_INTEGER: {
            char* temp = easy_asn1_print_integer(str->value, str->length);
            out += temp;
            free(temp);
        }
        break;
        case EASY_ASN1_NULL:
            out += "NULL";
            break;
        case EASY_ASN1_OBJECT: {
            char oid_str[MAX_OID] = {0};
            oid_to_string(str->value, str->length, oid_str);
            out += oid_str;
        }
        break;
        case EASY_ASN1_UTF8STRING:
        case EASY_ASN1_NUMERICSTRING:
        case EASY_ASN1_PRINTABLESTRING:
        case EASY_ASN1_T61STRING:
        case EASY_ASN1_VIDEOTEXSTRING:
        case EASY_ASN1_IA5STRING:
            out += std::string((char*)str->value, str->length);
            break;
        case EASY_ASN1_UTCTIME: {
            char time[21] = {0};
            convertUTCTimeToStandard((char*)str->value, str->length, 8, time);
            out += time;
        }
        break;
        case EASY_ASN1_GENERALIZEDTIME: {
            char time[21] = {0};
            convertGeneralizedTimeToStandard((char*)str->value, str->length, 8, time);
            out += time;
        }
        break;
        default:
            if (easy_asn1_print_string_try(str->value, str->length))
            {
                out += fmt::format("{:.{}s}", (char*)str->value, str->length);
            }
            else
            {
                out += fmt::format("{:02X}", fmt::join(reinterpret_cast<const unsigned char*>(str->value), reinterpret_cast<const unsigned char*>(str->value) + str->length, " "));
            }
            break;
        }
    }

    out += '\n';
    return out;
}

inline std::string easy_asn1_format_tree(easy_asn1_tree_st* node)
{
    std::string out;
    if (node)
    {
        for (uint32_t i = 0; i < node->level; i++)
            out += "    ";

        out += fmt::format("Offset: {}, ", node->offset);
        out += easy_asn1_format_string(&node->value, node->children_size);

        struct easy_asn1_tree_st* iter = node->first_child;
        while (iter != NULL)
        {
            out += easy_asn1_format_tree(iter);
            iter = iter->next_sibling;
        }
    }
    return out;
}

inline std::string HandleAsn1Content(const std::string& content)
{
    // 如果是 DER 编码的证书，直接返回
    // 如果是 PEM 编码的证书，则提取出 base64 内容
    if (content[0] == 0x30)
    {
        return content;
    }
    else
    {
        size_t begin_pos = content.find(PEM_CERT_BEGIN);
        size_t end_pos   = content.find(PEM_CERT_END);
        if (begin_pos == std::string::npos || end_pos == std::string::npos || begin_pos > end_pos)
        {
            return "";
        }
        begin_pos += strlen(PEM_CERT_BEGIN);
        std::string base64 = content.substr(begin_pos, end_pos - begin_pos);
        // 清除空格和换行
        std::string cleaned;
        for (char c : base64)
        {
            if (c != '\r' && c != '\n' && c != ' ')
                cleaned += c;
        }
        uint8_t* data     = (uint8_t*)malloc(BASE64_DECODE_OUT_SIZE(cleaned.size()));
        size_t   data_len = base64_decode(cleaned.data(), cleaned.size(), data);
        return std::string((char*)data, data_len);
    }
}

inline void HandleAsn1(
    const std::string& content,
    const std::string& infile,
    const std::string& outfile)
{
    std::string str;
    if (!content.empty())
    {
        str = HandleAsn1Content(content);
    }
    else if (!infile.empty())
    {
        auto content = ReadFile(infile);
        str          = HandleAsn1Content(content);
    }
    easy_asn1_tree_st* asn1_tree_st = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse((uint8_t*)str.data(), str.size(), 0, 0, &asn1_tree_st);
    if (outfile.empty())
    {
        easy_asn1_print_tree(asn1_tree_st);
    }
    else
    {
        std::string out = easy_asn1_format_tree(asn1_tree_st);
        fs::path    of  = fs::path(outfile);
        if (!fs::exists(of.parent_path()))
        {
            fs::create_directories(of.parent_path());
        }
        std::ofstream ofs(of, std::ios::trunc | std::ios::binary);
        ofs.write(out.data(), out.size());
        ofs.close();
    }

    easy_asn1_free_tree(asn1_tree_st);
}

class Asn1Algo : public IParserAlgorithm
{
public:
    std::string name() const override { return "asn1"; }

    void parse(IOOptions& io) override
    {
        HandleAsn1(io.inputStr, io.inputFile, io.outFile);
    }
};