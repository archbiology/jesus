#pragma once
#include "stmt.hpp"

REGISTER_FOR_UML(
    ImportModuleStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"moduleName"}));

class ImportModuleStmt : public Stmt
{
public:
  std::string moduleName;
  std::string alias; // null if simple import

  ImportModuleStmt(std::string module, std::string alias)
      : moduleName(std::move(module)), alias(std::move(alias)) {}

  void accept(StmtVisitor &visitor) const override;
};
