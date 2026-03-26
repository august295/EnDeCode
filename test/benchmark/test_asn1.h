#include <fstream>
#include <string>
#include <sstream>

#include <benchmark/benchmark.h>

#include "endecode/base64/base64.h"
#include "endecode/asn1/asn1.h"
#include "endecode/asn1/cert_sm2.h"
#include "endecode/asn1/gm_sof.h"
#include "endecode/asn1/gm_sef.h"

inline std::string ReadCertFile(const std::string& filename)
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