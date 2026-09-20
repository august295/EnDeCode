#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
#endif

#include <CLI/CLI.hpp>

#include "cli_asn1.h"
#include "cli_base64.h"

int main(int argc, char** argv)
{
#if defined(_WIN32) || defined(_WIN64)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    CLI::App app{"App description"};
    argv = app.ensure_utf8(argv);

    CLI::App*  codec = app.add_subcommand("codec", "encoders/decoders");
    Base64Algo base64Algo;
    base64Algo.register_cli(codec);

    CLI::App* parse = app.add_subcommand("parse", "parsers");
    Asn1Algo  asn1Algo;
    asn1Algo.register_cli(parse);

    try
    {
        app.require_subcommand(1);
        app.parse(argc, argv);
    }
    catch (const CLI::ParseError& e)
    {
        return app.exit(e);
    }

    return 0;
}