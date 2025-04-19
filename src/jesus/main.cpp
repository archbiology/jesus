#include <iostream>
#include <string>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "spirit/heart.hpp"

int main()
{
    Heart heart;
    std::string line;

    std::cout << "(Jesus) ";
    while (std::getline(std::cin, line))
    {
        auto tokens = lex(line);
        auto node = parse(tokens);

        if (node)
        {
            node->execute(&heart);
        }
        else
        {
            std::cout << "And the command was without form and void.\n";
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
