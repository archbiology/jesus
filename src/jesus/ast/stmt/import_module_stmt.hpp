#pragma once
#include "stmt.hpp"

REGISTER_FOR_UML(
    ImportModuleStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"modules"}));

class ImportModuleStmt : public Stmt
{
public:
  int relativeDepth;                // 0 = absolute
  std::vector<std::string> modules; // module path segments
  std::string alias;                // null if simple import

  ImportModuleStmt(int depth, std::vector<std::string> modules, std::string alias)
      : relativeDepth(depth), modules(std::move(modules)), alias(std::move(alias)) {}

  void accept(StmtVisitor &visitor) const override;
};
