#include "ast_inspect_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void AstInspectStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitAstInspectStmt(*this);
}
