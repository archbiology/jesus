#include "ast_inspector.hpp"
#include "ast/stmt/inspect_stmt.hpp"
#include "interpreter/interpreter.hpp"

void AstInspector::inspect(Interpreter &jesus, const InspectStmt &stmt)
{
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
