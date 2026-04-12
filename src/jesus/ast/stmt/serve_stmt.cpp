#include "serve_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void ServeStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitServeStmt(*this);
}
