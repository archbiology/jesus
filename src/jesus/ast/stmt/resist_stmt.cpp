#include "resist_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void ResistStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitResistStmt(*this);
}
