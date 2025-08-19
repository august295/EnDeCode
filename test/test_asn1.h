#include <fstream>
#include <string>

#include <gtest/gtest.h>
#include <benchmark/benchmark.h>

#include "endecode/base64/base64.h"
#include "endecode/asn1/asn1.h"
#include "endecode/asn1/asn1_helper.h"
#include "endecode/asn1/cert_sm2.h"
#include "endecode/asn1/gm_sof.h"
#include "endecode/asn1/gm_sef.h"

#include "test_util.h"

std::string ReadCertFile(const std::string& filename)
{
    std::ifstream      file(filename, std::ios::in | std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    file.close();

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
        return cleaned;
    }
}

/**
 * @brief   功能测试
 */
TEST(test_asn1, asn1_test1)
{
    uint8_t asn1_data[] = {0x04, 0x05, 'h', 'e', 'l', 'l', 'o'};
    size_t  dataLen     = sizeof(asn1_data);

    easy_asn1_string_st str;
    easy_asn1_parse_string(asn1_data, dataLen, &str);
#ifdef CONSOLE_PRINT
    easy_asn1_print_string(&str, 0);
#endif
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
    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse(asn1_data, sizeof(asn1_data), 0, 0, &tree);
#ifdef CONSOLE_PRINT
    easy_asn1_print_tree(tree);
#endif
    easy_asn1_free_tree(tree);
}

TEST(test_asn1, asn1_test3)
{
    // 读取整个文件内容到流中
    std::ifstream      file("./asn1/seal.gb38540", std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    file.close();

    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse((const uint8_t*)content.c_str(), content.size(), 0, 0, &tree);
#ifdef CONSOLE_PRINT
    easy_asn1_print_tree(tree);
#endif
    easy_asn1_free_tree(tree);
}

TEST(test_asn1, asn1_test4)
{
    std::ifstream      file("./asn1/seal.gb38540", std::ios::in | std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    file.close();

    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse((const uint8_t*)content.c_str(), content.size(), 0, 0, &tree);
    SEALINFO* seal = NULL;
    SEF_ParseSeal(tree, &seal);

    // 读取印章
    char*        type    = seal->imageType;
    std::string  filenam = std::string("seal.") + type;
    std::fstream outfile(filenam, std::ios::out | std::ios::binary);
    outfile.write((char*)seal->imageData, seal->imageDataLen);
    outfile.close();

    // 释放
    SEF_FreeSeal(seal);
    easy_asn1_free_tree(tree);
}

TEST(test_asn1, asn1_test5)
{
    std::ifstream      file("./asn1/signatures.gb38540", std::ios::in | std::ios::binary);
    std::ostringstream oss;
    oss << file.rdbuf();
    std::string content = oss.str();
    file.close();

    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse((const uint8_t*)content.c_str(), content.size(), 0, 0, &tree);
    SIGNEDVALUEINFO* signatures = NULL;
    SEF_ParseSignatures(tree, &signatures);

    // 读取印章
    easy_asn1_tree_st* seal_tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse((const uint8_t*)signatures->sealData, signatures->sealDataLen, 0, 0, &seal_tree);
    SEALINFO* seal = NULL;
    SEF_ParseSeal(seal_tree, &seal);
    char*        type    = seal->imageType;
    std::string  filenam = std::string("signatures_seal.") + type;
    std::fstream outfile(filenam, std::ios::out | std::ios::binary);
    outfile.write((char*)seal->imageData, seal->imageDataLen);
    outfile.close();

    // 释放
    SEF_FreeSeal(seal);
    SEF_FreeSignatures(signatures);
    easy_asn1_free_tree(seal_tree);
    easy_asn1_free_tree(tree);
}

TEST(test_asn1, asn1_oid)
{
    std::string filename = "./oid.json";

    OID_MAPPING* oid_mapping     = NULL;
    size_t       oid_mapping_len = 0;
    ReadOid(filename.c_str(), &oid_mapping, &oid_mapping_len);

    // 释放
    FreeOid(oid_mapping, oid_mapping_len);
}

TEST(test_asn1, asn1_serialize)
{
    std::string str = ReadCertFile("./cer/github.cer");
    EXPECT_TRUE(!str.empty());

    size_t   str_size = str.size();
    uint8_t* data     = (uint8_t*)malloc(BASE64_DECODE_OUT_SIZE(str_size));
    size_t   data_len = base64_decode(str.c_str(), str_size, (unsigned char*)data);

    // 解析
    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse(data, data_len, 0, 0, &tree);

    // 序列化
    size_t   tree_size = easy_asn1_serialize(tree, NULL);
    uint8_t* tree_ser  = (uint8_t*)malloc(tree_size);
    easy_asn1_serialize(tree, tree_ser);
    EXPECT_TRUE(my_equal_array_8bit(data, tree_ser, data_len));

#ifdef CONSOLE_PRINT
    easy_asn1_print_tree(tree);
#endif

    // 释放
    free(data);
    easy_asn1_free_tree(tree);
    free(tree_ser);
}

TEST(test_asn1, asn1_create1)
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
    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_create_node(ESAY_X509_ASN1_SEQUENCE, 0, NULL, tree);
    easy_asn1_push_back_child(tree, ESAY_X509_ASN1_SEQUENCE, 0, NULL);
    easy_asn1_tree_st* child1 = easy_asn1_get_tree_item(tree, 0);
    easy_asn1_push_back_child(child1, ESAY_X509_ASN1_SEQUENCE, 0, NULL);
    easy_asn1_tree_st* child11 = easy_asn1_get_tree_item(child1, 0);
    int                num     = 1;
    easy_asn1_push_back_child(child11, EASY_ASN1_INTEGER, 1, (uint8_t*)&num);
    easy_asn1_push_back_child(tree, ESAY_X509_ASN1_SEQUENCE, 0, NULL);
    easy_asn1_tree_st* child2 = easy_asn1_get_tree_item(tree, 1);
    int                num2   = 2;
    easy_asn1_push_back_child(child2, EASY_ASN1_INTEGER, 1, (uint8_t*)&num2);
    int num3 = 3;
    easy_asn1_push_back_child(child2, EASY_ASN1_INTEGER, 1, (uint8_t*)&num3);
#ifdef CONSOLE_PRINT
    easy_asn1_print_tree(tree);
#endif

    // 序列化
    size_t   tree_size    = easy_asn1_serialize(tree, NULL);
    uint8_t* tree_ser     = (uint8_t*)malloc(tree_size);
    size_t   tree_ser_len = easy_asn1_serialize(tree, tree_ser);
    EXPECT_TRUE(my_equal_array_8bit(asn1_data, tree_ser, tree_ser_len));

    // 释放
    easy_asn1_free_tree(tree);
    free(tree_ser);
}

TEST(test_asn1, asn1_create2)
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
    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_create_node(ESAY_X509_ASN1_SEQUENCE, 0, NULL, tree);

    easy_asn1_tree_st* node1 = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_create_node(ESAY_X509_ASN1_SEQUENCE, 0, NULL, node1);
    easy_asn1_push_back_child(node1, ESAY_X509_ASN1_SEQUENCE, 0, NULL);
    easy_asn1_tree_st* child1 = easy_asn1_get_tree_item(node1, 0);
    int                num    = 1;
    easy_asn1_push_back_child(child1, EASY_ASN1_INTEGER, 1, (uint8_t*)&num);

    easy_asn1_tree_st* node2 = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_create_node(ESAY_X509_ASN1_SEQUENCE, 0, NULL, node2);
    int num2 = 2;
    int num3 = 3;
    easy_asn1_push_back_child(node2, EASY_ASN1_INTEGER, 1, (uint8_t*)&num2);
    easy_asn1_push_back_child(node2, EASY_ASN1_INTEGER, 1, (uint8_t*)&num3);

    easy_asn1_push_back_tree_child(tree, node1);
    easy_asn1_push_back_tree_child(tree, node2);
#ifdef CONSOLE_PRINT
    easy_asn1_print_tree(tree);
#endif

    // 序列化
    size_t   tree_size    = easy_asn1_serialize(tree, NULL);
    uint8_t* tree_ser     = (uint8_t*)malloc(tree_size);
    size_t   tree_ser_len = easy_asn1_serialize(tree, tree_ser);
    EXPECT_TRUE(my_equal_array_8bit(asn1_data, tree_ser, tree_ser_len));

    // 释放
    easy_asn1_free_tree(tree);
    free(tree_ser);
}

TEST(test_asn1, x509_test1)
{
    std::string str = ReadCertFile("./cer/github.cer");
    EXPECT_TRUE(!str.empty());

    size_t   str_size = str.size();
    uint8_t* data     = (uint8_t*)malloc(BASE64_DECODE_OUT_SIZE(str_size));
    size_t   data_len = base64_decode(str.c_str(), str_size, (unsigned char*)data);

    easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
    easy_asn1_parse(data, data_len, 0, 0, &tree);
#ifdef CONSOLE_PRINT
    easy_asn1_print_tree(tree);
#endif

    // 释放
    free(data);
    easy_asn1_free_tree(tree);
}

TEST(test_asn1, sm2_test1)
{
    std::string str = ReadCertFile("./cer/sm2-x509.cer");
    EXPECT_TRUE(!str.empty());

    size_t   str_size = str.size();
    uint8_t* data     = (uint8_t*)malloc(BASE64_DECODE_OUT_SIZE(str_size));
    size_t   data_len = base64_decode(str.c_str(), str_size, (unsigned char*)data);

    SM2Certificate* cert = sm2_cert_parse(data, data_len);

    // 释放
    free(data);
    sm2_cert_free(cert);
}

TEST(test_asn1, sm2_test2)
{
    std::string str = ReadCertFile("./cer/sm2-x509.cer");
    EXPECT_TRUE(!str.empty());

#ifdef CONSOLE_PRINT
    BSTR info = NULL;
    printf("Cert info:\n");
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_VERSION);
    printf("SGD_CERT_VERSION: %s\n", info);
    free(info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SERIAL);
    printf("SGD_CERT_SERIAL: %s\n", info);
    free(info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SIGNATURE_ALGORITHM);
    printf("SGD_CERT_SIGNATURE_ALGORITHM: %s\n", info);
    free(info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_ISSUER);
    printf("SGD_CERT_ISSUER: %s\n", info);
    free(info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_VALID_TIME);
    printf("SGD_CERT_VALID_TIME: %s\n", info);
    free(info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SUBJECT);
    printf("SGD_CERT_SUBJECT: %s\n", info);
    free(info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_DER_PUBLIC_KEY);
    printf("SGD_CERT_DER_PUBLIC_KEY: %s\n", info);
    free(info);
    info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_DER_EXTENSIONS);
    printf("SGD_CERT_DER_EXTENSIONS: %s\n", info);
    free(info);
#endif
}

/**
 * @brief   性能测试
 */
static void bench_sof_parse_cert(benchmark::State& state, const std::string& filename)
{
    while (state.KeepRunning())
    {
        try
        {
            state.PauseTiming();
            std::string str  = ReadCertFile(filename);
            BSTR        info = NULL;
            state.ResumeTiming();

            size_t          str_size = str.size();
            uint8_t*        data     = (uint8_t*)malloc(BASE64_DECODE_OUT_SIZE(str_size));
            size_t          data_len = base64_decode(str.c_str(), str_size, (unsigned char*)data);
            SM2Certificate* cert     = sm2_cert_parse(data, data_len);

            state.PauseTiming();
            free(data);
            sm2_cert_free(cert);
            state.ResumeTiming();
        }
        catch (const std::exception&)
        {
        }
    }
}
BENCHMARK_CAPTURE(bench_sof_parse_cert, 1KB, "cer/sm2-x509.cer");

static void bench_sof_get_cert_info(benchmark::State& state, const std::string& filename)
{
    while (state.KeepRunning())
    {
        try
        {
            state.PauseTiming();
            std::string str  = ReadCertFile(filename);
            BSTR        info = NULL;
            state.ResumeTiming();

            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_VERSION);
            free(info);
            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SERIAL);
            free(info);
            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SIGNATURE_ALGORITHM);
            free(info);
            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_ISSUER);
            free(info);
            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_VALID_TIME);
            free(info);
            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_SUBJECT);
            free(info);
            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_DER_PUBLIC_KEY);
            free(info);
            info = SOF_GetCertInfo((BSTR)str.c_str(), SGD_CERT_DER_EXTENSIONS);
            free(info);

            state.PauseTiming();
            info = NULL;
            state.ResumeTiming();
        }
        catch (const std::exception&)
        {
        }
    }
}
BENCHMARK_CAPTURE(bench_sof_get_cert_info, 1KB, "cer/sm2-x509.cer");

static void bench_sef_parse_seal(benchmark::State& state, const std::string& filename)
{
    while (state.KeepRunning())
    {
        try
        {
            state.PauseTiming();
            std::ifstream      file(filename, std::ios::in | std::ios::binary);
            std::ostringstream oss;
            oss << file.rdbuf();
            std::string content = oss.str();
            file.close();
            state.ResumeTiming();

            easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
            easy_asn1_parse((const uint8_t*)content.c_str(), content.size(), 0, 0, &tree);
            SEALINFO* seal = NULL;
            SEF_ParseSeal(tree, &seal);

            state.PauseTiming();
            SEF_FreeSeal(seal);
            easy_asn1_free_tree(tree);
            state.ResumeTiming();
        }
        catch (const std::exception&)
        {
        }
    }
}
BENCHMARK_CAPTURE(bench_sef_parse_seal, 15KB, "asn1/seal.gb38540");

static void bench_sef_parse_signatures(benchmark::State& state, const std::string& filename)
{
    while (state.KeepRunning())
    {
        try
        {
            state.PauseTiming();
            std::ifstream      file(filename, std::ios::in | std::ios::binary);
            std::ostringstream oss;
            oss << file.rdbuf();
            std::string content = oss.str();
            file.close();
            state.ResumeTiming();

            easy_asn1_tree_st* tree = (easy_asn1_tree_st*)malloc(sizeof(easy_asn1_tree_st));
            easy_asn1_parse((const uint8_t*)content.data(), content.size(), 0, 0, &tree);
            SIGNEDVALUEINFO* signatures = NULL;
            SEF_ParseSignatures(tree, &signatures);

            state.PauseTiming();
            SEF_FreeSignatures(signatures);
            easy_asn1_free_tree(tree);
            state.ResumeTiming();
        }
        catch (const std::exception&)
        {
        }
    }
}
BENCHMARK_CAPTURE(bench_sef_parse_signatures, 16KB, "asn1/signatures.gb38540");