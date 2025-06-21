#pragma once
#include "stmt.hpp"

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
};
