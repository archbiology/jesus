#include "assign_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void AssignStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitAssignStmt(*this);
}
