#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "endecode/base64/base64.h"
#include "endecode/asn1/asn1.h"
#include "endecode/asn1/x509.h"
#include "endecode/asn1/cert_sm2.h"
#include "endecode/asn1/gm_sof.h"

#include "test_util.h"

TEST(test_asn1, asn1_test1)
{
    uint8_t        asn1_data[] = {0x04, 0x05, 'h', 'e', 'l', 'l', 'o'};
    const uint8_t* dataPtr     = asn1_data;
    size_t         dataLen     = sizeof(asn1_data);

    easy_asn1_string_st str;
    easy_asn1_parse_string(dataPtr, &str);
    // easy_asn1_print_string(&str, 0);
    easy_asn1_free_string(&str);
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
    easy_asn1_tree_st* tree = NULL;
    easy_asn1_parse(asn1_data, sizeof(asn1_data), 0, &tree);
    // easy_asn1_print_tree(tree);
    easy_asn1_free_tree(tree);
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

    easy_asn1_tree_st* tree = NULL;
    easy_asn1_parse(data, data_len, 0, &tree);
    // easy_asn1_print_tree(tree);
    easy_asn1_free_tree(tree);
}

TEST(test_asn1, sm2_test1)
{
    std::string   filename = "./cer/sm2-x509.cer";
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

    SM2Certificate* cert = sm2_cert_parse(data, data_len);
    sm2_cert_free(cert);
}

TEST(test_asn1, sm2_test2)
{
    std::string   filename = "./cer/sm2-x509.cer";
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

    BSTR info = NULL;
    printf("Cert info:\n");
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_VERSION);
    printf("SGD_CERT_VERSION: %s\n", info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SERIAL);
    printf("SGD_CERT_SERIAL: %s\n", info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SIGNATURE_ALGORITHM);
    printf("SGD_CERT_SIGNATURE_ALGORITHM: %s\n", info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_ISSUER);
    printf("SGD_CERT_ISSUER: %s\n", info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_VALID_TIME);
    printf("SGD_CERT_VALID_TIME: %s\n", info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SUBJECT);
    printf("SGD_CERT_SUBJECT: %s\n", info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_DER_PUBLIC_KEY);
    printf("SGD_CERT_DER_PUBLIC_KEY: %s\n", info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_DER_EXTENSIONS);
    printf("SGD_CERT_DER_EXTENSIONS: %s\n", info);
}