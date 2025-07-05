#include "variable_rule.hpp"

bool VariableRule::parse(ParserContext &ctx)
{
    return ctx.match(TokenType::IDENTIFIER);
}
