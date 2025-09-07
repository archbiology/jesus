#include "ask_expr_rule.hpp"
#include "../../../ast/expr/ask_expr.hpp"
#include "../../../ast/expr/literal_expr.hpp"
#include "../../../ast/expr/variable_expr.hpp"
#include "../../../types/known_types.hpp"

#include <memory>

std::unique_ptr<Expr> AskExprRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::ASK))
        return nullptr;

    if (ctx.match(TokenType::STRING))
    {
        Value prompt = ctx.previous().literal;
        return std::make_unique<AskExpr>(std::make_unique<LiteralExpr>(prompt, KnownTypes::STRING));
    }

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected a text literal or a text-typed variable after 'ask' (e.g., ask \"What is your age?\" or ask question).");

    std::string varName = ctx.previous().lexeme;

    std::shared_ptr<CreationType> varType = ctx.getVarType(varName);
    if (!varType)
    {
        throw std::runtime_error("Variable '" + varName + "' not found. Are you really sure it was declared?");
    }

    if (varType->primitive_type != PrimitiveType::Text)
    {
        throw std::runtime_error("Error: 'ask' expects a variable of primitive type 'text' as its prompt, but received variable '" + varName + "' of primitive type '" + varType->primitiveTypeName() + "'.");
    }

    return std::make_unique<AskExpr>(std::make_unique<VariableExpr>(varName));
}

std::string AskExprRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "AskExpr(...)";

    visited.insert(this);

    // return "AskExpr(" + expression->toStr(visited) +  ")";
    return "AskExpr(?)";
}