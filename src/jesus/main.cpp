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
    std::string buffer;
    std::cout << std::unitbuf;  // Enable automatic flushing on cout

    std::cout << "(Jesus) ";
    while (std::getline(std::cin, line))
    {
        buffer += line + "\n";  // Accumulate first
        try
        {
            // Accumulate lines until we reach 'amen' or EOF
            if (line.find("amen") != std::string::npos || std::cin.eof())
            {

                if (buffer.empty())
                {
                    continue;
                }

                auto tokens = lex(buffer);

                auto node = parse(tokens);

                if (node)
                {
                    interpreter.execute(node);
                }

                buffer.clear();
                continue;
            }

            buffer += line + "\n"; // Accumulate
        }
        catch (const std::exception &e)
        {
            std::cerr << "❌ Error: " << e.what() << "\n";
            buffer.clear(); // Reset buffer on error
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
