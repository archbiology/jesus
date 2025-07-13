#include "update_var_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void UpdateVarStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitUpdateVar(*this);
}
