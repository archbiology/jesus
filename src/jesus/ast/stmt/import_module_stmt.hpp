#pragma once
#include "stmt.hpp"

REGISTER_FOR_UML(
    ImportModuleStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"modules"}));

class ImportItem
{
public:
  std::string originalName;
  std::string alias; // empty if no alias was given
};

class ImportModuleStmt : public Stmt
{
public:
  int relativeDepth;                       // 0 = absolute
  std::vector<std::string> modules;        // module path segments
  std::vector<ImportItem> importedSymbols; // ["Class1", "Class2"]

  ImportModuleStmt(int depth, std::vector<std::string> modules, std::vector<ImportItem> importedSymbols)
      : relativeDepth(depth), modules(std::move(modules)), importedSymbols(std::move(importedSymbols)) {}

  void accept(StmtVisitor &visitor) const override;
};
