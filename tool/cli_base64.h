#pragma once

#include <string>

#include <ghc/fs_std.hpp>
#include <spdlog/fmt/fmt.h>

#include "endecode/base64/base64.h"
#include "cli_util.h"
#include "cli_algorithm.h"

inline void HandleBase64(
    bool               isEncrypt,
    const std::string& content,
    const std::string& infile,
    const std::string& outfile)
{
    std::string str;
    if (!content.empty())
    {
        str = content;
    }
    else if (!infile.empty())
    {
        str = ReadFile(infile);
    }

    char* result     = nullptr;
    int   result_len = 0;
    if (isEncrypt)
    {
        result     = (char*)malloc(BASE64_ENCODE_OUT_SIZE(str.size()));
        result_len = base64_encode((uint8_t*)str.data(), str.size(), result);
    }
    else
    {
        result     = (char*)malloc(str.size());
        result_len = base64_decode(str.data(), str.size(), (uint8_t*)result);
    }

    std::string out;
    if (result && (result_len > 0))
    {
        out = std::string(result, result_len);
        free(result);
    }
    else
    {
        fmt::println("base64 转换错误");
    }

    if (outfile.empty())
    {
        fmt::println(out);
    }
    else
    {
        fs::path of = fs::path(outfile);
        if (!fs::exists(of.parent_path()))
        {
            fs::create_directories(of.parent_path());
        }
        std::ofstream ofs(of, std::ios::trunc | std::ios::binary);
        ofs.write(out.data(), out.size());
        ofs.close();
    }
}

class Base64Algo : public ICodecAlgorithm
{
public:
    std::string name() const override { return "base64"; }

    void encode(IOOptions& io) override
    {
        HandleBase64(true, io.inputStr, io.inputFile, io.outFile);
    }
    void decode(IOOptions& io) override
    {
        HandleBase64(false, io.inputStr, io.inputFile, io.outFile);
    }
};
