#include "for_each_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void ForEachStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitForEach(*this);
}
