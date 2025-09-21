#include "formatted_string_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../ast/expr/formatted_string_expr.hpp"
#include "../../../../../types/known_types.hpp"
#include "../../../../../lexer/lexer.hpp"
#include "../../../jesus_grammar.hpp"

std::unique_ptr<Expr> FormattedStringRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::FORMATTED_STRING))
        return nullptr;

    Token token = ctx.previous();
    const std::string raw = token.literal.toString();

    std::vector<std::string> parts;
    std::vector<std::unique_ptr<Expr>> exprs;

    size_t pos = 0;
    while (pos < raw.size())
    {
        // Each part goes until until the next brace '{'
        size_t open = raw.find('{', pos);
        if (open == std::string::npos)
        {
            parts.push_back(raw.substr(pos));
            break;
        }

        // Find the closing brace '}'
        size_t close = raw.find('}', open);
        if (close == std::string::npos)
            throw std::runtime_error("Unclosed brace '}' in formatted string: " + raw);

        // literal before the next brace
        parts.push_back(raw.substr(pos, open - pos));

        // The var name or 'object attribute'
        std::string exprText = raw.substr(open + 1, close - open - 1);

        // Parse it as a full expression
        auto tokens = lex(exprText);
        ParserContext subCtx(tokens, ctx.interpreter);
        auto expr = grammar::GetAttribute->parse(subCtx);
        if (!expr)
            throw std::runtime_error("Invalid expression in formatted string: " + exprText);

        if (!expr->canBeUsedInFormattedString())
        {
            throw std::runtime_error(
                "NasaRules: Only simple variables or attribute chains are allowed inside formatted strings. "
                "This restriction ensures code clarity and safety, following Object Calisthenics "
                "and the 10 NASA rules for maintainable software. "
                "Expression rejected: '" +
                exprText + "'.");
        }

        exprs.push_back(std::move(expr));

        pos = close + 1;
    }

    if (exprs.empty())
        return std::make_unique<LiteralExpr>(token.literal, KnownTypes::STRING);

    return std::make_unique<FormattedStringExpr>(std::move(raw), std::move(parts), std::move(exprs));
}
