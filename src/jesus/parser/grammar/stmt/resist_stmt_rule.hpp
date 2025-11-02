#pragma once
#include "../grammar_rule.hpp"
#include "../../../ast/stmt/stmt.hpp"

class ResistStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);

    std::shared_ptr<Stmt> parseStmt(ParserContext &ctx, std::string &blockName);
};
