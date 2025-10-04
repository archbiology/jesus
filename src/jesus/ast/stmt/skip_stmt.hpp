#pragma once

#include "stmt.hpp"

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