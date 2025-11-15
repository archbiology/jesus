#include "faith.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

#include <fstream>
#include <sstream>

Faith::Faith(Interpreter &jesus) : jesus(jesus) {}

void Faith::interpret(const std::string &source)
{
    Lexer lexer;
    auto tokens = lexer.tokenize(source);

    ParserContext context(tokens, &jesus);
    std::vector<std::unique_ptr<Stmt>> statements;

    while (!context.isAtEnd())
    {
        auto stmt = parse(tokens, context);
        if (stmt)
            statements.push_back(std::move(stmt));
    }

    for (auto &you : statements)
        jesus.loves(you);
}

int Faith::execute(const std::string &filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Could not open file: " + filename << "\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    interpret(buffer.str());
    return 0;
}
