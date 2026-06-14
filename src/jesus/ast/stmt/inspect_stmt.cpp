#include "inspect_stmt.hpp"
#include "interpreter/stmt_visitor.hpp"

void InspectStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitInspectStmt(*this);
}
