#pragma once
#include "stmt.hpp"

/**
 * set disciples to ["Peter", "James", "John"]
 * for each name in disciples:
 *    if name == James:
 *      continue # or next; skip;
 *
 *    say name
 */
class ContinueStmt : public Stmt
{
public:
  ContinueStmt() = default;
};