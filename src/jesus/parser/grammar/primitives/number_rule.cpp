#include "number_rule.hpp"
#include "../../../lexer/token_type.hpp"

bool NumberRule::parse(ParserContext &ctx)
{
    return ctx.matchAny({TokenType::INT, TokenType::DOUBLE});
}
