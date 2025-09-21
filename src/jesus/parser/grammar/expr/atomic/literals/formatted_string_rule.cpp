#include "formatted_string_rule.hpp"
#include "../../../../../ast/expr/literal_expr.hpp"
#include "../../../../../ast/expr/formatted_string_expr.hpp"
#include "../../../../../types/known_types.hpp"

std::unique_ptr<Expr> FormattedStringRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::FORMATTED_STRING))
        return nullptr;

    Token token = ctx.previous();
    const std::string raw = token.literal.toString();

    std::vector<std::string> parts;
    std::vector<std::string> variables;

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

        // variable name inside the brace
        std::string varName = raw.substr(open + 1, close - open - 1);

        // Validate it's a declared variable (only identifiers allowed)
        if (!ctx.variableExists(varName))
            throw std::runtime_error("Undeclared variable in formatted string: " + varName);

        variables.push_back(varName);

        pos = close + 1;
    }

    if (variables.empty())
        return std::make_unique<LiteralExpr>(token.literal, KnownTypes::STRING);

    return std::make_unique<FormattedStringExpr>(std::move(raw), std::move(parts), std::move(variables));
}
