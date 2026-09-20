#pragma once

#include <string>

#include <ghc/fs_std.hpp>
#include <spdlog/fmt/fmt.h>

#include "endecode/sha1/sha1.h"
#include "endecode/sha2/sha2_256.h"
#include "endecode/sha3/sha3_256.h"
#include "cli_util.h"
#include "cli_algorithm.h"

inline void HandleSHA1(
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

    char digest[SHA1_DIGEST_LENGTH] = {0};
    SHA1((uint8_t*)str.data(), str.size(), (uint8_t*)digest);
    std::string out = fmt::format("{:02x}",
                                  fmt::join(reinterpret_cast<const unsigned char*>(digest),
                                            reinterpret_cast<const unsigned char*>(digest) + SHA1_DIGEST_LENGTH,
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

class AlgoSHA1 : public IOneWayAlgorithm
{
public:
    std::string name() const override { return "sha1"; }

    void digest(IOOptions& io) override
    {
        HandleSHA1(io.inputStr, io.inputFile, io.outFile);
    }
};

inline void HandleSHA256(
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

    char digest[SHA2_256_DIGEST_LENGTH] = {0};
    sha2_256((uint8_t*)str.data(), str.size(), (uint8_t*)digest);
    std::string out = fmt::format("{:02x}",
                                  fmt::join(reinterpret_cast<const unsigned char*>(digest),
                                            reinterpret_cast<const unsigned char*>(digest) + SHA2_256_DIGEST_LENGTH,
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

class AlgoSHA256 : public IOneWayAlgorithm
{
public:
    std::string name() const override { return "sha256"; }

    void digest(IOOptions& io) override
    {
        HandleSHA256(io.inputStr, io.inputFile, io.outFile);
    }
};

inline void HandleSHA3_256(
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

    char digest[SHA3_256_DIGEST_LENGTH] = {0};
    sha3_256((uint8_t*)str.data(), str.size(), (uint8_t*)digest);
    std::string out = fmt::format("{:02x}",
                                  fmt::join(reinterpret_cast<const unsigned char*>(digest),
                                            reinterpret_cast<const unsigned char*>(digest) + SHA3_256_DIGEST_LENGTH,
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

class AlgoSHA3_256 : public IOneWayAlgorithm
{
public:
    std::string name() const override { return "sha3_256"; }

    void digest(IOOptions& io) override
    {
        HandleSHA3_256(io.inputStr, io.inputFile, io.outFile);
    }
};
