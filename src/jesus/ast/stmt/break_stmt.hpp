#pragma once
#include "stmt.hpp"

REGISTER_FOR_UML(
    BreakStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"}));

/**
 * set disciples to ["Peter", "James", "John"]
 * for each name in disciples:
 *    if name == James:
 *      break # or stop
 *
 *    say name
 */
class BreakStmt : public Stmt
{
public:
  BreakStmt() = default;

  void accept(StmtVisitor &visitor) const override;
};
