#include "break_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void IfStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitIfStmt(*this);
}
