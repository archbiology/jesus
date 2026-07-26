#include <iostream>
#include "cli_parser.hpp"
#include "../understanding/scripture/book_aliases.hpp"
#include "../utils/file_utils.hpp"

ParsedCLI CLIParser::parse(int argc, char **argv)
{
    ParsedCLI out;

    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];

        if (arg == "--help" || arg == "-h" || arg == "/?" || arg == "help")
        {
            out.showHelp = true;
            continue;
        }

        if (arg == "bible")
        {
            out.showBibleHelp = true;
            continue;
        }

        if (arg == "giants")
        {
            out.showWarfareHelp = true;
            continue;
        }

        if (arg == "confess")
        {
            out.showConfessHelp = true;
            continue;
        }

        if (arg == "--quiet")
        {
            out.quiet = true;
            continue;
        }

        if (arg == "--vm")
        {
            out.useVm = true;
            continue;
        }

        if (arg == "--export-uml")
        {
            out.export_language_uml = true;
            continue;
        }

        // -----------------------------------------------------------------------
        // By default, the AST is only preserved in the REPL, which is started by
        // running `jesus` without a source file. This allows users to inspect the
        // original (non-optimized) AST with the `ast` command while learning how
        // the parser builds the syntax tree.
        //
        // When executing a source file (e.g. `jesus program.jesus`), the compiler
        // performs optimization passes before execution. The optimized AST is then
        // discarded because it is no longer needed, reducing memory usage.
        //
        // The `--keep-ast` option preserves the optimized AST when executing a
        // source file, allowing users to inspect the result of the optimization
        // passes with the `ast` command. This option exists primarily for learning,
        // debugging, and developing the compiler.
        // -----------------------------------------------------------------------
        if (arg == "--keep-ast")
        {
            out.keepAst = true;
            continue;
        }

        if (arg.starts_with("-"))
        {
            std::cerr << "Unknown option: " << arg << "\n";
            out.showHelp = true;
            return out;
        }

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
                out.fileExists = utils::isFile(out.filename);
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
