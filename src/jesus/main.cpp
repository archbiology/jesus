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

int main(int argc, char **argv)
{
    CLIParser parser;
    ParsedCLI cli = parser.parse(argc, argv);

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
