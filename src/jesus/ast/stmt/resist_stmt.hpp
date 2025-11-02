#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"

/**
 * @brief 'resist' is used to throw/raise exceptions in Jesus Programming Language.
 *
 * resist ItsWritten('Away from me, Satan! For it is written: ‘Worship the Lord your God, and serve him only‘')
 *
 * "Submit yourselves, then, to God. Resist the devil, and he will flee from you." — James 4:7
 */
class ResistStmt : public Stmt
{
public:
    std::string exceptionType;
    std::unique_ptr<Expr> messageExpr;

public:
    ResistStmt(const std::string &type, std::unique_ptr<Expr> msg)
        : exceptionType(type), messageExpr(std::move(msg)) {}

    void accept(StmtVisitor &visitor) const override;
};
