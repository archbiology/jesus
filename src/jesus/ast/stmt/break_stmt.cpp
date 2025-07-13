#include "break_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void BreakStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitBreak(*this);
}
