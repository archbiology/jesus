#include "binary_expr.hpp"
#include "interpreter/expr_visitor.hpp"
#include "types/creation_type.hpp"
#include "types/known_types.hpp"

Value BinaryExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitBinary(*this);
}

std::shared_ptr<CreationType> BinaryExpr::getReturnType(ParserContext &ctx) const
{
    switch (op.type)
    {
    case TokenType::LESS:
    case TokenType::LESS_EQUAL:
    case TokenType::GREATER:
    case TokenType::GREATER_EQUAL:
    case TokenType::IS:
    case TokenType::NOT_EQUAL:
    case TokenType::AND:
    case TokenType::OR:
        return KnownTypes::TRUTH;

    case TokenType::VERSUS:
    {
        // bool XOR bool -> truth
        if (left->getReturnType(ctx) == KnownTypes::TRUTH)
            return KnownTypes::TRUTH;

        // int XOR int -> int
        return KnownTypes::INT;
    }

    case TokenType::PLUS:
    case TokenType::MINUS:
    case TokenType::STAR:
    case TokenType::MOD:
    {
        auto leftType = left->getReturnType(ctx);
        auto rightType = right->getReturnType(ctx);

        if (leftType == KnownTypes::STRING || rightType == KnownTypes::STRING)
        {
            return KnownTypes::STRING;
        }

        if (leftType == KnownTypes::DOUBLE || rightType == KnownTypes::DOUBLE)
        {
            return KnownTypes::DOUBLE;
        }

        return KnownTypes::INT;
    }

    case TokenType::SLASH:
        return KnownTypes::DOUBLE;

    default:
        return right->getReturnType(ctx);
    }
}