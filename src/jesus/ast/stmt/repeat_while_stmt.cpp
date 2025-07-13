#include "repeat_while_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void RepeatWhileStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitRepeatWhile(*this);
}
