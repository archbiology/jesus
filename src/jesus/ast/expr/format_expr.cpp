#include "format_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/known_types.hpp"
#include "../../utils/terminal/color.hpp"

namespace
{
std::string ansiCodeFor(TokenType type)
{
    switch (type)
    {
    case TokenType::YELLOW:
        return terminal::color::yellow;

    case TokenType::RED:
        return terminal::color::red;

    case TokenType::BLUE:
        return terminal::color::blue;

    case TokenType::GREEN:
        return terminal::color::green;

    case TokenType::BOLD:
        return terminal::color::bold;

    case TokenType::ITALIC:
        return terminal::color::italic;

    default:
        return "";
    }
}
} // namespace

Value FormatExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitFormatExpr(*this);
}

Value FormatExpr::evaluate(std::shared_ptr<Heart> heart) const
{
    Value innerVal = inner->evaluate(heart);

    std::string text = innerVal.toString();

    std::string prefix;
    for (TokenType f : formatters)
        prefix += ansiCodeFor(f);

    std::string result = prefix + text + terminal::color::reset;
    return Value(result);
}

std::shared_ptr<CreationType> FormatExpr::getReturnType(ParserContext &ctx) const
{
    return KnownTypes::STRING;
}

std::string FormatExpr::toString() const
{
    std::string str = "FormatExpr(";

    for (TokenType f : formatters)
    {
        switch (f)
        {

        case TokenType::RED:
            str += "red(";
            break;

        case TokenType::GREEN:
            str += "green(";
            break;

        case TokenType::BLUE:
            str += "blue(";
            break;

        case TokenType::YELLOW:
            str += "yellow(";
            break;

        case TokenType::BOLD:
            str += "bold(";
            break;

        case TokenType::ITALIC:
            str += "italic(";
            break;

        default:
            break;
        }
    }
    str += inner->toString();
    str += std::string(formatters.size(), ')');
    str += ")";
    return str;
}
