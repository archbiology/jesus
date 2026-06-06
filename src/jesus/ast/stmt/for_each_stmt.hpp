#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include <string>
#include <memory>
#include <vector>

REGISTER_FOR_UML(
    ForEachStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"varName", "body"}));

/**
 * set disciples to ["Peter", "James", "John"]
 * for each name in disciples:
 *    say name
 */
class ForEachStmt : public Stmt
{
public:
    std::vector<std::string> varNames;
    std::unique_ptr<Expr> iterable;
    std::vector<std::unique_ptr<Stmt>> body;

    ForEachStmt(
        std::vector<std::string> varNames,
        std::unique_ptr<Expr> iterable,
        std::vector<std::unique_ptr<Stmt>> body)
        : varNames(std::move(varNames)),
          iterable(std::move(iterable)),
          body(std::move(body)) {}

  void accept(StmtVisitor &visitor) const override;
};
