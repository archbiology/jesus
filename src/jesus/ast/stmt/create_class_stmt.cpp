#include "create_class_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void CreateClassStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitCreateClass(*this);
}
