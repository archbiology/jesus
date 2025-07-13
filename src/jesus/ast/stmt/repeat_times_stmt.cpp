#include "repeat_times_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void RepeatTimesStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitRepeatTimes(*this);
}
