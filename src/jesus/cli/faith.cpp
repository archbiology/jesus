#include "faith.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "utils/file_utils.hpp"
#include "optimizer/optimizer.hpp"
#include "vm/compiler.hpp"
#include "vm/vm.hpp"

void Faith::interpret(Interpreter &jesus, const std::string &source, const std::string &moduleName)
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

    // Only imported modules keep their classes: the importing module holds
    // methods that point back into the imported module's AST. The main module
    // always eliminates its unused classes on DeadCodeEliminator.
    Optimizer optimizer(retainClasses);
    optimizer.optimize(statements);

    if (useVm)
    {
        Compiler compiler;
        Chunk chunk = compiler.compile(statements);
        VM vm;

        vm.run(chunk);
        return;
    }

    // Keep the AST only while it may still be needed, to save memory: when the
    // user asked to inspect it (`keepAst`), or when this module is imported
    // (`retainClasses`), because the importing module holds methods that point
    // back into it (`Method::definition` is a raw pointer into the AST).
    // Otherwise each node is released as soon as it has run.
    const bool mustRetainAst = keepAst || retainClasses;
    if (mustRetainAst)
    {
        for (auto &you : statements)
        {
            jesus.loves(you);
            jesus.retainAst(std::move(you));
        }
    }
    else
    {
        for (auto &you : statements)
        {
            jesus.loves(you);
        }
    }
}

int Faith::execute(std::string filename)
{
    try
    {
        std::string source = utils::readFile(filename);
        std::string moduleName = utils::basenameWithoutExtension(filename);

        auto newModule = Interpreter::createModule(moduleName, filename);
        Interpreter jesus(newModule, useVm, keepAst);

        interpret(jesus, source, moduleName);
        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << "\n";
        return 1;
    }
}
