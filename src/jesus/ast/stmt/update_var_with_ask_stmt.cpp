#include "update_var_with_ask_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void UpdateVarWithAskStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitUpdateVarWithAsk(*this);
}
