#include <iostream>
#include <string>
#include "lexer/lexer.hpp"

int main()
{
    std::string line;

    std::cout << "(Jesus) ";
    while (std::getline(std::cin, line))
    {
        auto tokens = lex(line);
        for (const auto &token : tokens)
        {
            std::cout << "[" << token.type << ": " << token.value << "]\n";
        }
        std::cout << "(Jesus) ";
    }

    return 0;
}
