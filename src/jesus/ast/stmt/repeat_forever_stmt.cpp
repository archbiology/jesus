#include "repeat_forever_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void RepeatForeverStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitRepeatForeverStmt(*this);
}
