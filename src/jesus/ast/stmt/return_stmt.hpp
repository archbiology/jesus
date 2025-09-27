#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"

/**
 * @brief Represents a `return` statement in the Jesus Programming Language.
 *
 * A `ReturnStmt` optionally carries an expression whose value is returned
 * from the current method. If no expression is provided, the method returns
 * `void` (we call it `nothing`).
 *
 * 📖 Verse: "Return (your heart) to the LORD your God, for he is gracious and merciful,
 * slow to anger, and abounding in steadfast love; and he relents over disaster."
 * — Joel 2:13
 */
class ReturnStmt : public Stmt
{
public:
    std::unique_ptr<Expr> value;

public:
    explicit ReturnStmt(std::unique_ptr<Expr> value) : value(std::move(value)) {}

    void accept(StmtVisitor &visitor) const override;

    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const;
};
