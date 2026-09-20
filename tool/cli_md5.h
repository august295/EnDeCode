#pragma once

#include <string>

#include <ghc/fs_std.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ranges.h>

#include "endecode/md5/md5.h"
#include "cli_util.h"
#include "cli_algorithm.h"

inline void HandleMD5(
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

    char digest[MD5_DIGEST_LENGTH] = {0};
    md5((uint8_t*)str.data(), str.size(), (uint8_t*)digest);
    std::string out = fmt::format("{:02x}",
                                  fmt::join(reinterpret_cast<const unsigned char*>(digest),
                                            reinterpret_cast<const unsigned char*>(digest) + MD5_DIGEST_LENGTH,
                                            ""));

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

class AlgoMD5 : public IOneWayAlgorithm
{
public:
    std::string name() const override { return "md5"; }

    void digest(IOOptions& io) override
    {
        HandleMD5(io.inputStr, io.inputFile, io.outFile);
    }
};
