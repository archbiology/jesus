#include "interpreter.hpp"
#include <stdexcept>

Value Interpreter::evaluate(std::unique_ptr<Expr> &expr)
{
    if (auto b = dynamic_cast<BinaryExpr *>(expr.get()))
        return visitBinary(b);

    if (auto u = dynamic_cast<UnaryExpr *>(expr.get()))
        return visitUnary(u);

    if (auto l = dynamic_cast<LiteralExpr *>(expr.get()))
        return visitLiteral(l);

    if (auto v = dynamic_cast<VariableExpr *>(expr.get()))
        return visitVariable(v);

    if (auto g = dynamic_cast<GroupingExpr *>(expr.get()))
        return visitGrouping(g);

    throw std::runtime_error("Unknown expression type");
}

void Interpreter::defineVariable(const std::string &name, const Value &value)
{
    heart.define(name, value);
}

void Interpreter::assignVariable(const std::string &name, const Value &value)
{
    heart.assign(name, value);
}

Value Interpreter::visitBinary(BinaryExpr *expr)
{
    Value left = evaluate(expr->left);
    TokenType op = expr->op.type;
    Value right = evaluate(expr->right);

    if (op == TokenType::PLUS)
    {
        if (std::holds_alternative<double>(left) && std::holds_alternative<double>(right))
            return std::get<double>(left) + std::get<double>(right);

        if (std::holds_alternative<std::string>(left) && std::holds_alternative<std::string>(right))
            return std::get<std::string>(left) + std::get<std::string>(right);

        throw std::runtime_error("Operands must be numbers or strings.");
    }

    if (op == TokenType::EQUAL_EQUAL)
        return left == right;

    if (op == TokenType::NOT_EQUAL)
        return left != right;

    if (op == TokenType::GREATER)
        return std::get<double>(left) > std::get<double>(right);

    if (op == TokenType::LESS)
        return std::get<double>(left) < std::get<double>(right);

    if (op == TokenType::AND)
        return isTruthy(left) && isTruthy(right);

    if (op == TokenType::OR)
        return isTruthy(left) || isTruthy(right);

    throw std::runtime_error("Unsupported binary operator.");
}

Value Interpreter::visitUnary(UnaryExpr *expr)
{
    TokenType op = expr->op.type;
    Value right = evaluate(expr->right);

    if (op == TokenType::NOT)
        return !isTruthy(right);

    throw std::runtime_error("Unsupported unary operator.");
}

Value Interpreter::visitLiteral(LiteralExpr *expr)
{
    return expr->value;
}

Value Interpreter::visitVariable(VariableExpr *expr)
{
    return heart.get(expr->name);
}

Value Interpreter::visitGrouping(GroupingExpr *expr)
{
    return evaluate(expr->expression);
}

bool Interpreter::isTruthy(const Value &value)
{
    if (std::holds_alternative<std::monostate>(value))
        return false;

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value);

    return true;
}

std::string Interpreter::valueToString(const Value &value)
{
    if (std::holds_alternative<std::monostate>(value))
        return "none";

    if (std::holds_alternative<std::string>(value))
        return std::get<std::string>(value);

    if (std::holds_alternative<double>(value))
        return std::to_string(std::get<double>(value));

    if (std::holds_alternative<bool>(value))
        return std::get<bool>(value) ? "true" : "false";

    return "unknown";
}

void Interpreter::execute(std::unique_ptr<Stmt> &stmt)
{
    if (auto set = dynamic_cast<SetStmt *>(stmt.get()))
        return visitSet(set);

    throw std::runtime_error("Unknown statement type.");
}

void Interpreter::visitSet(SetStmt *stmt)
{
    Value val = evaluate(stmt->value);
    heart.define(stmt->name, val);
}

Value Interpreter::visitConditional(ConditionalExpr *expr)
{
    Value conditionValue = evaluate(expr->condition);

    // Interpret the condition as boolean
    if (isTruthy(conditionValue))
    {
        return evaluate(expr->thenBranch);
    }
    else
    {
        return evaluate(expr->elseBranch);
    }
}
