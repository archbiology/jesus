#include <iostream>
#include <string>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "spirit/heart.hpp"
#include "interpreter/interpreter.hpp"

int main()
{
    Heart heart;
    Interpreter interpreter(&heart);
    std::string line;

    std::cout << "(Jesus) ";
    while (std::getline(std::cin, line))
    {
        try
        {

            auto tokens = lex(line);
            auto node = parse(tokens);

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
