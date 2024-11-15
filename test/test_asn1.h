#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "endecode/asn1/asn1.h"
#include "endecode/asn1/x509.h"
#include "endecode/base64/base64.h"

#include "test_util.h"

#define PEM_CERT_BEGIN "-----BEGIN CERTIFICATE-----"
#define PEM_CERT_END   "-----END CERTIFICATE-----"

TEST(test_asn1, asn1_test1)
{
    uint8_t        asn1_data[] = {0x04, 0x05, 'h', 'e', 'l', 'l', 'o'};
    const uint8_t* dataPtr     = asn1_data;
    size_t         dataLen     = sizeof(asn1_data);

    asn1_string_st str;
    asn1_parse_string(dataPtr, &str);
    // asn1_print_string(&str);
    free(str.value);
}

TEST(test_asn1, asn1_test2)
{
    // clang-format off
    uint8_t asn1_data[] = {
       0x30, 0x0F,          // SEQUENCE, length 15
       0x30, 0x05,          // SEQUENCE, length 5
       0x30, 0x03,          // SEQUENCE, length 3
       0x02, 0x01, 0x01,    // INTEGER, length 1, value 1
       0x30, 0x06,          // SEQUENCE, length 6
       0x02, 0x01, 0x02,    // INTEGER, length 1, value 2
       0x02, 0x01, 0x03     // INTEGER, length 1, value 3
    };
    // clang-format on
    asn1_tree_st* tree = asn1_parse(asn1_data, sizeof(asn1_data), 0);
    // asn1_print_tree(tree);
    asn1_free_tree(tree);
}

TEST(test_asn1, x509_test1)
{
    std::string   filename = "./cer/github.cer";
    std::ifstream in(filename);

    std::string str;
    std::string line;
    while (std::getline(in, line))
    {
        if (line == PEM_CERT_BEGIN || line == PEM_CERT_END || line.empty())
        {
            continue;
        }
        str += line;
    }
    in.close();

    size_t   str_size = str.size();
    uint8_t* data     = (uint8_t*)malloc(BASE64_DECODE_OUT_SIZE(str_size));
    size_t   data_len = base64_decode(str.c_str(), str_size, (unsigned char*)data);

    asn1_tree_st* tree = x509_parse_cert(data, data_len, 0);
    // asn1_print_tree(tree);
    asn1_free_tree(tree);
}
