#include "return_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"
#include "../../types/known_types.hpp"

void ReturnStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitReturnStmt(*this);
}

std::shared_ptr<CreationType> ReturnStmt::getReturnType(ParserContext &ctx) const
{
    if (value != nullptr)
        return value->getReturnType(ctx);

    return KnownTypes::VOID;
}
