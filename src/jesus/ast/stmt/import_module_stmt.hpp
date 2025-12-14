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
  int relativeDepth;                        // 0 = absolute
  std::vector<std::string> modules;         // module path segments
  std::vector<std::string> importedSymbols; // ["Class1", "Class2"]

  ImportModuleStmt(int depth, std::vector<std::string> modules, std::vector<std::string> importedSymbols)
      : relativeDepth(depth), modules(std::move(modules)), importedSymbols(std::move(importedSymbols)) {}

  void accept(StmtVisitor &visitor) const override;
};
