#include "confess_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../types/known_types.hpp"
#include "../../../../../cli/help.hpp"

std::unique_ptr<Expr> ConfessRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::CONFESS))
        return std::make_unique<LiteralExpr>(Value(HelpCLI::getConfessInstructions()), KnownTypes::STRING);

    return nullptr;
}
