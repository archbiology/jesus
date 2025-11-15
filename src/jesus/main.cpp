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

#include "cli/cli_parser.hpp"
#include "cli/disciple.hpp"
#include "cli/faith.hpp"

int main(int argc, char **argv)
{
    CLIParser parser;
    ParsedCLI cli = parser.parse(argc, argv);

    auto global_scope = std::make_shared<Heart>("global");
    SymbolTable symbol_table(global_scope);
    Interpreter jesus(symbol_table);
    Faith michael(jesus);

    grammar::initializeGrammar();
    KnownTypes::registerBuiltInTypes();

    // ----------------------
    // Run a file if provided
    // ----------------------
    if (!cli.filename.empty())
    {
        return michael.execute(cli.filename);
    }

    // ---------------
    // Otherwise, REPL
    // ---------------
    const std::string version = JESUS_VERSION;
    const std::string commit = JESUS_COMMIT;
    Banner::show(version, commit, cli.quiet);

    Disciple follower(jesus);
    follower.walk();

    return 0;
}
