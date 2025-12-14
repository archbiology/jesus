#include "disciple.hpp"
#include "../parser/parser.hpp"
#include <iostream>

Disciple::Disciple(Interpreter &interpreter) : jesus(interpreter) {}

void Disciple::walk()
{
    std::string buffer;
    std::string line;

    std::cout << "(Jesus) ";

    while (std::getline(std::cin, line))
    {
        if (!line.empty())
            line += "\n";

        buffer += line;

        try
        {
            processBuffer(buffer);
        }
        catch (const std::exception &e)
        {
            std::cerr << "❌ Error: " << e.what() << "\n";
            buffer.clear();
        }

        if (buffer.empty())
            std::cout << "(Jesus) ";
    }
}

void Disciple::processBuffer(std::string &buffer)
{
    Lexer lexer;
    auto tokens = lexer.tokenize(buffer);

    if (lexer.insideMultilineComment())
        return;

    ParserContext context(tokens, &jesus, "core"); // TODO: Use __jesus__ instead of 'core'
    std::vector<std::unique_ptr<Stmt>> statements;
    bool waitingForMoreTokens = false;

    // --------------------------------
    // Make sure all tokens are parsed
    // Otherwise, '9 = 9' just prints 9
    // --------------------------------
    while (!context.isAtEnd())
    {
        auto stmt = parse(tokens, context);

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
        return;

    // ----------------------------------------
    // Execute the statements (run the program)
    // ----------------------------------------
    for (auto &you : statements)
    {
        jesus.loves(you);
        jesus.persistAST(std::move(you));
    }

    buffer.clear();
}
