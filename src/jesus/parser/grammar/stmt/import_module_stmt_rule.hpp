#pragma once

#include "../../parser_context.hpp"
#include "../../../ast/stmt/stmt.hpp"

class ImportModuleStmtRule
{
public:
    std::unique_ptr<Stmt> parse(ParserContext &ctx);

    /**
     * @brief come <identifier>
     */
    std::unique_ptr<Stmt> parseComeModule(ParserContext &ctx);

    /**
     * @brief from <identifier> come <identifier>
     */
    std::unique_ptr<Stmt> parseFromModuleComeMember(ParserContext &ctx);
};
