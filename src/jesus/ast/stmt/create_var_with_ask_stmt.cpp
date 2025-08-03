#include "create_var_with_ask_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void CreateVarWithAskStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitCreateVarWithAsk(*this);
}
