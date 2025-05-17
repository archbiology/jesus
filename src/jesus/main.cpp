#include <iostream>
#include <string>
// #include "ignore-this-folder/lexer-color.h"
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

        for(int i =0; i < tokens.size(); i++) {
            std::cout<< "TOKEN: " << tokens[i].toString() << std::endl;
        }

        auto node = parse(tokens);

        if (node)
        {
            std::cout<<"node.toString(): ";
            std::cout<< node->toString() << "\n";

            std::cout << "VISITing...: ";
            node->execute(&heart);
            std::cout<<std::endl;
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
