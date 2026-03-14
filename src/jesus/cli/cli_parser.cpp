#include "cli_parser.hpp"
#include "../understanding/scripture/book_aliases.hpp"

ParsedCLI CLIParser::parse(int argc, char **argv)
{
    ParsedCLI out;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--quiet")
        {
            out.quiet = true;
            continue;
        }

        if (arg == "--export-uml")
        {
            out.export_language_uml = true;
            continue;
        }

        if (arg == "--spanish" || arg == "--es")
        {
            out.language = HolyBible::Language::Spanish;
            continue;
        }

        if (arg.starts_with("-"))
            continue; // skip other global flags for the moment

        if (out.filename.empty() && !out.isScripture)
        {
            if (looksLikeBibleBook(arg))
            {
                out.isScripture = true;
                out.scriptureText = arg;
            }
            else
            {
                out.filename = arg; // first non-flag = command
            }
        }
        else
        {
            if (out.isScripture)
                out.scriptureText += " " + arg;
            else
                out.args.push_back(arg); // rest = command args
        }
    }

    return out;
}

bool CLIParser::looksLikeBibleBook(const std::string &token)
{
    return HolyBible::BOOK_ALIASES.contains(token);
}
