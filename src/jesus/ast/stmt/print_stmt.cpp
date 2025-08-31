#include "print_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void PrintStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitPrintStmt(*this);
}
