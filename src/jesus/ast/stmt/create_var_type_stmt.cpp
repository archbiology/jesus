#include "create_var_type_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void CreateVarTypeStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitCreateVarType(*this);
}
