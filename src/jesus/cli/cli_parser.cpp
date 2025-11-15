#include "cli_parser.hpp"

ParsedCLI CLIParser::parse(int argc, char **argv)
{
    ParsedCLI out;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (out.filename.empty())
        {
            if (arg == "--quiet")
                out.quiet = true;
            else if (arg.starts_with("-"))
                continue; // skip other global flags for the moment
            else
                out.filename = arg; // first non-flag = command
        }
        else
        {
            out.args.push_back(arg); // rest = command args
        }
    }

    return out;
}
