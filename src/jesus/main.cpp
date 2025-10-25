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

int main(int argc, char **argv)
{
    bool quiet = false;
    for (int i = 1; i < argc; ++i)
    {
        std::string arg = argv[i];
        if (arg == "--quiet")
            quiet = true;
    }

    const std::string version = JESUS_VERSION;
    const std::string commit = JESUS_COMMIT;
    Banner::show(version, commit, quiet);

    grammar::initializeGrammar(); // Sets the Expression rule target to Primary

    auto global_scope = std::make_shared<Heart>("global");
    SymbolTable symbol_table(global_scope);
    Interpreter jesus(symbol_table);
    Lexer lexer;

    std::string buffer;
    std::string line;

    KnownTypes::registerBuiltInTypes();

    std::cout << "(Jesus) ";
    while (std::getline(std::cin, line))
    {
        if (!line.empty())
            line += "\n";

        buffer += line;

        try
        {

            auto tokens = lexer.tokenize(buffer);
            if (lexer.insideMultilineComment())
                continue;

            ParserContext context(tokens, &jesus);
            std::vector<std::unique_ptr<Stmt>> statements;
            bool waitingForMoreTokens = false;

            // --------------------------------
            // Make sure all tokens are parsed
            // Otherwise, '9 = 9' just prints 9
            // --------------------------------
            while (!context.isAtEnd())
            {
                auto stmt = parse(tokens, context); // AST - Abstract Syntax Tree
                if (stmt)
                {
                    if (stmt->inProgress())
                    {
                        waitingForMoreTokens = true;
                        break;
                    }

                    statements.push_back(std::move(stmt));
                }
            }

            // ----------------------------------------
            // ENTER pressed but still need more tokens
            // ----------------------------------------
            if (waitingForMoreTokens)
                continue;

            // ----------------------------------------
            // Execute the statements (run the program)
            // ----------------------------------------
            for (auto &you : statements)
            {
                jesus.loves(you);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "❌ Error: " << e.what() << "\n";
        }

        buffer.clear();
        std::cout << "(Jesus) ";
    }

    return 0;
}
