#include "memory_inspect_stmt_rule.hpp"
#include "../../../ast/stmt/memory_inspect_stmt.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> MemoryInspectStmtRule::parse(ParserContext &ctx)
{

    if (!ctx.match(TokenType::MEMORY))
    {
        return nullptr;
    }

    return std::make_unique<MemoryInspectStmt>();
}
