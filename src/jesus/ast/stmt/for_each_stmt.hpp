#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include <string>
#include <memory>
#include <vector>

/**
 * set disciples to ["Peter", "James", "John"]
 * for each name in disciples:
 *    say name
 */
class ForEachStmt : public Stmt
{
public:
    std::string varName;
    std::unique_ptr<Expr> iterable;
    std::vector<std::unique_ptr<Stmt>> body;

    ForEachStmt(std::string varName,
                std::unique_ptr<Expr> iterable,
                std::vector<std::unique_ptr<Stmt>> body)
        : varName(std::move(varName)),
          iterable(std::move(iterable)),
          body(std::move(body)) {}
};
