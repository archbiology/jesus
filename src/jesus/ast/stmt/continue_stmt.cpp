#include "continue_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void ContinueStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitContinue(*this);
}
