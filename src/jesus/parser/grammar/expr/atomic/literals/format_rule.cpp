#include "format_rule.hpp"
#include "ast/expr/format_expr.hpp"
#include <vector>

std::unique_ptr<Expr> FormatRule::parse(ParserContext &ctx)
{
    auto modifiers = {
        TokenType::RED, TokenType::GREEN, TokenType::BLUE, TokenType::YELLOW, TokenType::BOLD, TokenType::ITALIC};

    if (!ctx.checkAny(modifiers))
    {
        return valueRule->parse(ctx);
    }

    std::vector<TokenType> formatters;
    std::string modifierText;
    while (ctx.checkAny(modifiers))
    {

        const auto token = ctx.advance();
        formatters.push_back(token.type);

        if (!modifierText.empty())
            modifierText += " ";

        modifierText += token.lexeme;
    }

    auto inner = valueRule->parse(ctx);
    if (!inner)
        throw std::runtime_error(
            "Expected a value after '" + modifierText + "'.\n\nExample:\n  " + modifierText + " \"life\"");

    return std::make_unique<FormatExpr>(std::move(formatters), std::move(inner));
}
