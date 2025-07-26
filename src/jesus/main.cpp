#include <iostream>
#include <string>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "spirit/heart.hpp"
#include "interpreter/interpreter.hpp"
#include "parser/grammar/jesus_grammar.hpp"
#include "types/known_types.hpp"

int main()
{
    grammar::initializeGrammar(); // Sets the Expression rule target to Primary

    Heart heart;
    Interpreter interpreter(&heart);
    std::string line;

    KnownTypes::registerBuiltInTypes();

    std::cout << "(Jesus) ";
    while (std::getline(std::cin, line))
    {
        try
        {

            auto tokens = lex(line);
            ParserContext context(tokens, &interpreter);
            auto node = parse(tokens, context);

            if (node)
            {
                interpreter.execute(node);
            }
        }
        catch (const std::exception &e)
        {
            std::cerr << "❌ Error: " << e.what() << "\n";
        }

        // ----------------
        // Print the tokens
        // ----------------
        // for (const auto &token : tokens)
        // {
        //     std::cout << "[" << token.type << ": " << token.value << "]\n";
        // }

        std::cout << "(Jesus) ";
    }

    return 0;
}
