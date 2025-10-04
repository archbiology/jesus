#include "skip_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void SkipStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitSkipStmt(*this);
}
