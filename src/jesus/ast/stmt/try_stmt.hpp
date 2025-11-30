#pragma once
#include "stmt.hpp"

REGISTER_FOR_UML(
    TryStmt,
    .packageName("ast.stmt")
        .parentsList({"Stmt"})
        .fieldsList({"tryBody", "catchClauses", "alwaysBody"})
        .methodsList({"accept"}));

/**
 * @brief try-repent block (try-catch / try-except in other languages)
 */
class TryStmt : public Stmt
{
public:
    std::vector<std::shared_ptr<Stmt>> tryBody;
    std::vector<std::pair<std::string, std::vector<std::shared_ptr<Stmt>>>> catchClauses;
    std::vector<std::shared_ptr<Stmt>> alwaysBody;

public:
    TryStmt(std::vector<std::shared_ptr<Stmt>> tryBody,
            std::vector<std::pair<std::string, std::vector<std::shared_ptr<Stmt>>>> catches,
            std::vector<std::shared_ptr<Stmt>> alwaysBody)
        : tryBody(std::move(tryBody)),
          catchClauses(std::move(catches)),
          alwaysBody(std::move(alwaysBody)) {}

    void accept(StmtVisitor &visitor) const override;
};
