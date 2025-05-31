#include "expression_parser.hpp"
#include "../ast/expr/binary_expr.hpp"
#include "../ast/expr/unary_expr.hpp"
#include "../ast/expr/literal_expr.hpp"
#include "../ast/expr/variable_expr.hpp"
#include "../ast/expr/grouping_expr.hpp"
#include <stdexcept>

ExpressionParser::ExpressionParser(const std::vector<Token> &tokens)
    : tokens(tokens), current(0) {}

std::unique_ptr<Expr> ExpressionParser::parse()
{
    if (isAtEnd()) return NULL;

    return parseOr();
}

std::unique_ptr<Expr> ExpressionParser::parseOr()
{
    auto expr = parseAnd();
    while (match(TokenType::OR))
    {
        Token op = previous();
        auto right = parseAnd();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::parseAnd()
{
    auto expr = parseEquality();
    while (match(TokenType::AND))
    {
        Token op = previous();
        auto right = parseEquality();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::parseEquality()
{
    auto expr = parseComparison();
    while (match(TokenType::EQUAL_EQUAL, TokenType::NOT_EQUAL))
    {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::parseComparison()
{
    // -----------------------------------------------
    // Operator precedence:
    //  arithmetic > relational > logical operators
    //  * and / > + and - > comparisons > logical ops
    // -----------------------------------------------
    auto expr = parseAddition();

    while (match(TokenType::GREATER, TokenType::GREATER_EQUAL,
                 TokenType::LESS, TokenType::LESS_EQUAL))
    {
        Token op = previous();
        auto right = parseAddition();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> ExpressionParser::parseAddition() {
    auto expr = parseMultiplication();

    while (match(TokenType::PLUS, TokenType::MINUS)) {
        Token op = previous();
        auto right = parseMultiplication();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> ExpressionParser::parseMultiplication() {
    auto expr = parseUnaryExpression();

    while (match(TokenType::STAR, TokenType::SLASH)) {
        Token op = previous();
        auto right = parseUnaryExpression();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

std::unique_ptr<Expr> ExpressionParser::parseUnaryExpression()
{
    if (match(TokenType::NOT, TokenType::MINUS, TokenType::PLUS))
    {
        Token op = previous();
        auto right = parseUnaryExpression();
        return std::make_unique<UnaryExpr>(op, std::move(right));
    }
    return parseLiteralOrGroupOrVar();
}

std::unique_ptr<Expr> ExpressionParser::parseLiteralOrGroupOrVar()
{
    if (match(TokenType::FALSE)) {
        return std::make_unique<LiteralExpr>(Value(false));
    }

    if (match(TokenType::TRUE)) {
        return std::make_unique<LiteralExpr>(Value(true));
    }

    if (match(TokenType::INT, TokenType::DOUBLE, TokenType::STRING))
    {
        return std::make_unique<LiteralExpr>(previous().literal);
    }

    if (match(TokenType::IDENTIFIER))
    {
        return std::make_unique<VariableExpr>(previous().lexeme);
    }

    if (match(TokenType::LEFT_PAREN))
    {
        auto expr = parse();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw std::runtime_error("Unexpected expression.");
}

bool ExpressionParser::match(TokenType type)
{
    if (check(type))
    {
        advance();
        return true;
    }

    return false;
}

bool ExpressionParser::match(TokenType type1, TokenType type2)
{
    return match(type1) || match(type2);
}

bool ExpressionParser::match(TokenType type1, TokenType type2, TokenType type3)
{
    return match(type1) || match(type2) || match(type3);
}

bool ExpressionParser::match(TokenType type1, TokenType type2, TokenType type3, TokenType type4)
{
    return match(type1) || match(type2) || match(type3) || match(type4);
}

bool ExpressionParser::check(TokenType type)
{
    if (isAtEnd())
        return false;

    return peek().type == type;
}

Token ExpressionParser::advance()
{
    if (!isAtEnd())
        current++;

    return previous();
}

bool ExpressionParser::isAtEnd()
{
    return peek().type == TokenType::END_OF_FILE;
}

Token ExpressionParser::peek()
{
    return tokens[current];
}

Token ExpressionParser::previous()
{
    return tokens[current - 1];
}

Token ExpressionParser::consume(TokenType type, const std::string &message)
{
    if (check(type))
        return advance();

    throw std::runtime_error("Parse error: " + message);
}
