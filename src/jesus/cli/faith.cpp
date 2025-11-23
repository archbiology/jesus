#include "faith.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "../utils/file_utils.hpp"

Faith::Faith(Interpreter &jesus) : jesus(jesus) {}

void Faith::interpret(const std::string &source, const std::string &moduleName)
{
    Lexer lexer;
    auto tokens = lexer.tokenize(source);

    ParserContext context(tokens, &jesus, moduleName);
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
    try
    {
        std::string source = utils::readFile(filename);
        std::string moduleName = utils::basenameWithoutExtension(filename);
        interpret(source, moduleName);
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
