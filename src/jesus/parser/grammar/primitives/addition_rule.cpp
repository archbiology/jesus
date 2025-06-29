#include "addition_rule.hpp"
#include "../../../lexer/token_type.hpp"
#include <iostream>

bool AdditionRule::parse(ParserContext &ctx)
{
    if (!inner->parse(ctx))
        return false;

    while (ctx.matchAny({TokenType::PLUS, TokenType::MINUS}))
    {
        if (!inner->parse(ctx))
        {
            std::cerr << "Expected expression after '+' or '-' in AdditionRule.\n";
            return false;
        }
    }

    return true;
}
