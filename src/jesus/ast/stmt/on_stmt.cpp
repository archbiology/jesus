#include "on_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void OnStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitOnStmt(*this);
}
