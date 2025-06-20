#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include <memory>

enum class OutputType
{
    SAY,
    WARN,
};

/// Statement for printing output: say, praise, rebuke
class OutputStmt : public Stmt
{
public:
    OutputType type;
    std::unique_ptr<Expr> message;

    OutputStmt(OutputType type, std::unique_ptr<Expr> message)
        : type(type), message(std::move(message)) {}

    virtual std::string toString() const { return "OutputStmt"; }
};
