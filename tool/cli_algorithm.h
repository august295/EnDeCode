#pragma once

#include <CLI/CLI.hpp>

struct NonEmptyString : public CLI::Validator
{
    NonEmptyString() : CLI::Validator("NonEmpty")
    {
        func_ = [](const std::string& val) -> std::string {
            if (val.empty())
            {
                return "value cannot be empty";
            }
            return "";
        };
    }
};

struct IOOptions
{
    std::string inputStr;
    std::string inputFile;
    std::string outFile;
};

// 复用：给任意命令挂 -i/-f/-o
inline void add_io_options(CLI::App* cmd, IOOptions& io)
{
    auto* group = cmd->add_option_group("input");
    group->add_option("-i,--input", io.inputStr, "input is string")
        ->check(NonEmptyString());
    group->add_option("-f,--file", io.inputFile, "input is file")
        ->check(NonEmptyString())
        ->check(CLI::ExistingFile);
    group->require_option(1);
    cmd->add_option("-o,--output", io.outFile, "output file, default terminal");
}

class ICryptoAlgorithm
{
public:
    virtual ~ICryptoAlgorithm() = default;

    // 算法名（用于 CLI 子命令名）
    virtual std::string name() const = 0;
    // 描述（用于 CLI help）
    virtual std::string description() const { return name(); }

    // 把自己注册到 CLI（各中间层重写，挂不同的二级子命令）
    virtual void register_cli(CLI::App* parent) = 0;
};

// 可逆算法：需要 encrypt / decrypt 两个方向的算法
class IReversibleAlgorithm : public ICryptoAlgorithm
{
public:
    // 具体算法实现这两个（或一个）
    virtual void encrypt(IOOptions& io) = 0;
    virtual void decrypt(IOOptions& io) = 0;

    // 统一的 CLI 注册：挂 encrypt / decrypt 两个二级子命令
    void register_cli(CLI::App* parent) override
    {
        CLI::App* algo = parent->add_subcommand(name(), description());

        CLI::App* enc = algo->add_subcommand("encrypt", "encrypt");
        add_io_options(enc, enc_io_);
        enc->callback([this]() { encrypt(enc_io_); });

        CLI::App* dec = algo->add_subcommand("decrypt", "decrypt");
        add_io_options(dec, dec_io_);
        dec->callback([this]() { decrypt(dec_io_); });

        algo->require_subcommand(1);
    }

protected:
    IOOptions enc_io_;
    IOOptions dec_io_;
};

// 单向算法：只有 digest 一个操作
class IOneWayAlgorithm : public ICryptoAlgorithm
{
public:
    virtual void digest(IOOptions& io) = 0;

    void register_cli(CLI::App* parent) override
    {
        CLI::App* algo = parent->add_subcommand(name(), description());

        CLI::App* dg = algo->add_subcommand("digest", "compute digest");
        add_io_options(dg, io_);
        dg->callback([this]() { digest(io_); });

        algo->require_subcommand(1);
    }

protected:
    IOOptions io_;
};

class ICodecAlgorithm : public ICryptoAlgorithm
{
public:
    virtual void encode(IOOptions& io) = 0;
    virtual void decode(IOOptions& io) = 0;

    void register_cli(CLI::App* parent) override
    {
        CLI::App* algo = parent->add_subcommand(name(), description());

        CLI::App* en = algo->add_subcommand("encode", "encode");
        add_io_options(en, enc_io_);
        en->callback([this]() { encode(enc_io_); });

        CLI::App* de = algo->add_subcommand("decode", "decode");
        add_io_options(de, dec_io_);
        de->callback([this]() { decode(dec_io_); });

        algo->require_subcommand(1);
    }

protected:
    IOOptions enc_io_;
    IOOptions dec_io_;
};

class IParserAlgorithm : public ICryptoAlgorithm
{
public:
    // parse 的输入输出可能不是同一套 IOOptions，看你的设计
    virtual void parse(IOOptions& io) = 0;
    // 可选：dump / 重新编码
    virtual void dump(IOOptions& io) { /* default: not supported */ }

    void register_cli(CLI::App* parent) override
    {
        CLI::App* algo = parent->add_subcommand(name(), description());

        CLI::App* ps = algo->add_subcommand("parse", "parse ASN.1");
        add_io_options(ps, parse_io_);
        ps->callback([this]() { parse(parse_io_); });

        CLI::App* dp = algo->add_subcommand("dump", "dump to text");
        add_io_options(dp, dump_io_);
        dp->callback([this]() { dump(dump_io_); });

        algo->require_subcommand(1);
    }

protected:
    IOOptions parse_io_;
    IOOptions dump_io_;
};