#include "import_module_stmt.hpp"
#include "../../interpreter/stmt_visitor.hpp"

void ImportModuleStmt::accept(StmtVisitor &visitor) const
{
  visitor.visitImportModuleStmt(*this);
}
