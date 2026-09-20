#pragma once

#include <string>

#include <ghc/fs_std.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ranges.h>

#include "endecode/sm3/sm3.h"
#include "cli_util.h"
#include "cli_algorithm.h"

inline void HandleSM3(
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

    uint8_t digest[SM3_DIGEST_LENGTH] = {0};
    sm3((uint8_t*)str.data(), str.size(), (uint8_t*)digest);
    std::string out = fmt::format("{:02x}",
                                  fmt::join(reinterpret_cast<const unsigned char*>(digest),
                                            reinterpret_cast<const unsigned char*>(digest) + SM3_DIGEST_LENGTH,
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

class AlgoSM3 : public IOneWayAlgorithm
{
public:
    std::string name() const override { return "sm3"; }

    void digest(IOOptions& io) override
    {
        HandleSM3(io.inputStr, io.inputFile, io.outFile);
    }
};
