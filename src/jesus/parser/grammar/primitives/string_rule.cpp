#include "string_rule.hpp"
#include "../../../lexer/token_type.hpp"

bool StringRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::STRING))
    {
        return true;
    }
    return false;
}
