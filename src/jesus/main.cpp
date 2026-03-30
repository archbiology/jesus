#include <iostream>
#include <string>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "spirit/heart.hpp"
#include "interpreter/interpreter.hpp"
#include "parser/grammar/jesus_grammar.hpp"
#include "types/known_types.hpp"
#include "utils/banner.hpp"
#include "utils/version.hpp"
#include "utils/file_utils.hpp"

#include "cli/cli_parser.hpp"
#include "cli/disciple.hpp"
#include "cli/faith.hpp"
#include "cli/uml_exporter.hpp"
#include "cli/bible.hpp"
#include "cli/help.hpp"

int main(int argc, char **argv)
{
    CLIParser parser;
    ParsedCLI cli = parser.parse(argc, argv);

    if (cli.showHelp)
    {
        HelpCLI::printGeneralHelp();
        return 0;
    }

    if (cli.showConfessHelp)
    {
        std::cout << HelpCLI::getConfessInstructions() << std::endl;
        return 0;
    }

    if (cli.showWarfareHelp)
    {
        std::cout << HelpCLI::getWarInstructions() << std::endl;
        return 0;
    }

    if (cli.showBibleHelp)
    {
        BibleCLI::printBibleHelp();
        return 0;
    }

    if (cli.isScripture)
    {
        auto ref = BibleCLI::parseScripture(cli.scriptureText);
        std::cout << BibleCLI::readScripture(ref);
        return 0;
    }

    grammar::initializeGrammar();
    KnownTypes::registerBuiltInTypes();

    if (cli.export_language_uml)
    {
        UmlExporter::exportPlantUml(std::cout);
        return 0;
    }

    // ----------------------
    // Run a file if provided
    // ----------------------
    if (!cli.filename.empty())
    {
        if (!cli.fileExists)
        {

            std::cerr << "Unknown command or file: " << cli.filename << "\n";
            std::cerr << "Type 'jesus help' for usage.\n\n";

            std::cerr << "Try:\n";
            std::cerr << "  jesus help\n";
            std::cerr << "  jesus <file>.jesus\n";
            std::cerr << "  jesus bible\n";
            std::cerr << "  jesus confess\n";
            std::cerr << "  jesus giants\n";
            std::cerr << "  jesus john 3:16\n\n";
            return 1;
        }

        Faith michael;
        return michael.execute(cli.filename);
    }

    // ---------------
    // Otherwise, REPL
    // ---------------
    const std::string version = JESUS_VERSION;
    const std::string commit = JESUS_COMMIT;
    Banner::show(version, commit, cli.quiet);

    std::string replName = "__jesus__";
    std::string replPath = std::filesystem::current_path() / (replName + ".jesus");
    auto scope = std::make_shared<Heart>(replName);
    auto symbol_table = std::make_shared<SymbolTable>(scope);
    auto repl = std::make_shared<Module>(replName, replPath, symbol_table);
    Interpreter jesus(repl);

    Disciple follower(jesus);
    follower.walk();

    return 0;
}
