#include "ast_inspector.hpp"
#include "ast/stmt/inspect_stmt.hpp"
#include "interpreter/interpreter.hpp"

void AstInspector::inspect(Interpreter &jesus, const InspectStmt &stmt)
{
    if (!jesus.keepAst)
    {
        std::cout << "The AST is not available (`ast` petition).\n\n"
                  << "When running a source file (e.g. `jesus program.jesus`), the "
                     "optimized AST is discarded after compilation to reduce memory "
                     "usage.\n\n"
                  << "If you want to inspect the optimized AST, run:\n\n"
                  << "    jesus --keep-ast <file>.jesus \n\n"
                  << "Alternatively, start the REPL by running:\n\n"
                  << "    jesus\n\n"
                  << "The REPL always preserves the original (non-optimized) AST for inspection.\n";

        return;
    }

    // Case 1 - print the whole AST.
    if (stmt.symbolName.empty())
    {
        for (auto &node : jesus.persistedAST)
        {
            std::cout << node->toString() << "\n";
        }
        return;
    }

    // Case 2 - print a specific AST node.
    auto node = jesus.lookupAST(stmt.symbolName);

    if (!node)
    {
        std::cout << "⚠️ No such symbol: " << stmt.symbolName << "\n";
        return;
    }

    std::cout << node->toString() << "\n";
    std::cout << "AST Memory: " << node->approxSize() << " bytes\n";
}
