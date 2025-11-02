#include "try_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void TryStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitTryStmt(*this);
}
