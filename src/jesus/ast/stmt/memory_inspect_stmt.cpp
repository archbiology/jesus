#include "memory_inspect_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void MemoryInspectStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitMemoryInspectStmt(*this);
}
