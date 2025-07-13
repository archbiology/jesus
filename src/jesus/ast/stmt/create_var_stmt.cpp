#include "create_var_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void CreateVarStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitCreateVar(*this);
}
