#include "giants_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../types/known_types.hpp"
#include "../../../../../cli/help.hpp"

std::unique_ptr<Expr> GiantsRule::parse(ParserContext &ctx)
{
    if (ctx.match(TokenType::GIANTS))
        return std::make_unique<LiteralExpr>(Value(HelpCLI::getWarInstructions()), KnownTypes::STRING);

    return nullptr;
}
