#include "output_statement.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void OutputStmt::accept(StmtVisitor &visitor) const
{
    visitor.visitOutput(*this);
}
