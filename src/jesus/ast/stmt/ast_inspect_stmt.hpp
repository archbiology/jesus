#pragma once
#include "stmt.hpp"

REGISTER_FOR_UML(
    AstInspectStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"symbolName: optional"}));

class AstInspectStmt : public Stmt
{
public:
  std::string symbolName;

  explicit AstInspectStmt(std::string name)
      : symbolName(std::move(name)) {}

  std::string toString() const override
  {
    return "AstInspectStmt(" + symbolName + ")";
  }

  void accept(StmtVisitor &visitor) const override;
};
