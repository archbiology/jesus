#pragma once

#include "stmt.hpp"

REGISTER_FOR_UML(
    SkipStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"}));

/**
 * set disciples to ["Peter", "James", "John"]
 * for each name in disciples:
 *    if name == 'James':
 *      skip
 *
 *    say name
 */
class SkipStmt : public Stmt
{
public:
  SkipStmt() = default;

  void accept(StmtVisitor &visitor) const override;
};