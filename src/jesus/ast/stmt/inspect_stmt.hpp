#pragma once
#include "stmt.hpp"

REGISTER_FOR_UML(
    InspectStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"target: AST|BYTECODE|VM", "symbolName: optional"}));

enum class InspectTarget
{
    AST,
    BYTECODE,
    VM
};

class InspectStmt : public Stmt
{
public:
    InspectTarget target;
    std::string symbolName;

    explicit InspectStmt(InspectTarget target, std::string name = "")
        : target(target), symbolName(std::move(name)) {}

    std::string toString() const override
    {
        return "InspectStmt(" + targetToString() + ", " + symbolName + ")";
    }

    void accept(StmtVisitor &visitor) const override;

private:
    std::string targetToString() const
    {
        switch (target)
        {
        case InspectTarget::AST:
            return "AST";
            break;

        case InspectTarget::BYTECODE:
            return "BYTECODE";
            break;

        case InspectTarget::VM:
            return "VM";
            break;

        default:
            return "WHAT";
        }
    }
};
