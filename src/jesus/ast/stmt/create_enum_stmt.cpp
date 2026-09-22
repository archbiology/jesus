#include "create_enum_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void CreateEnumStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitCreateEnum(*this);
}
