#include "create_var_stmt_rule.hpp"
#include "../../../ast/stmt/create_var_stmt.hpp"
#include "../../../ast/stmt/create_var_with_ask_stmt.hpp"
#include "../../../ast/expr/literal_expr.hpp"
#include "../../../types/known_types.hpp"
#include "../../../interpreter/runtime/instance.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> CreateVarStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::CREATE))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected variable type after 'create'");

    std::string varType_ = ctx.previous().lexeme;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected variable name after 'create type'");

    std::string varName = ctx.previous().lexeme;

    auto varType = KnownTypes::resolve(varType_, "core");
    if (!varType)
    {
        throw std::runtime_error("Unknown variable type: '" + varType_ + "'");
    }

    std::unique_ptr<Expr> value = nullptr;

    if (ctx.match(TokenType::EQUAL))
    {
        int snapshot = ctx.snapshot();
        if (ctx.match(TokenType::ASK))
        {
            ctx.restore(snapshot); // because 'ask->parse()' expects TokenType::ASK, but since it matched, it already advanced.

            std::unique_ptr<Expr> ask_expr = ask->parse(ctx);
            if (!ask_expr)
                throw std::runtime_error("Expected a text literal or a text-typed variable after 'ask' (e.g., ask \"What is your name?\" or ask question).");

            ctx.registerVarType(varType->name, varName);

            return std::make_unique<CreateVarWithAskStmt>(varType, varName, std::move(ask_expr));
        }
        value = expression->parse(ctx);
        if (!value)
            throw std::runtime_error("Expected expression after '=' in create statement.");

        std::string value_str = "";
        if (varType->isClass())
        {
            auto instance = std::make_shared<Instance>(varType); // FIXME: Creating an instance at parse time here.
            Value object = Value(instance);
            value = std::make_unique<LiteralExpr>(object, KnownTypes::CLASS); // TODO: Consider constructor and 'if' expression
        }
        // ------------------------------------------------------------
        // If the 'value' is a literal, validate it now, at parse time.
        // ------------------------------------------------------------
        else if (value->canEvaluateAtParseTime())
        {
            auto empty_scope = std::make_shared<Heart>("creating_var");
            Value literal = value->evaluate(empty_scope);

            varType->validate(literal);

            value_str = "'" + literal.toString() + "'";
        }

        auto valueType = value->getReturnType(ctx);
        bool typesMatch = varType->primitive_type == valueType->primitive_type; // FIXME: Negative and Positive would detect as same type
        if (!typesMatch)
        {
            if (value_str.empty())
                value_str = "type '" + valueType->name+"'";

            throw std::runtime_error("Invalid value " + value_str + " for variable '" + varName + "' declared as type " + varType->name);
        }

        ctx.registerVarType(varType->name, varName);
    }

    return std::make_unique<CreateVarStmt>(varType_, varName, std::move(value));
}
