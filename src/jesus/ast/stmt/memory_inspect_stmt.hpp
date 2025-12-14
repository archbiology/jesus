#pragma once

#include "stmt.hpp"

REGISTER_FOR_UML(
    MemoryInspectStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"}));

class MemoryInspectStmt : public Stmt
{
public:
    std::string toString() const override
    {
        return "MemoryInspectStmt()";
    }

    void accept(StmtVisitor &visitor) const override;
};
