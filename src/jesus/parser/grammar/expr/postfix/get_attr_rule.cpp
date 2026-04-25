#include "get_attr_rule.hpp"
#include "ast/expr/get_attr_expr.hpp"
#include "ast/expr/variable_expr.hpp"
#include "ast/expr/method_call_expr.hpp"
#include "ast/expr/index_expr.hpp"
#include "types/creation_type.hpp"
#include "types/known_types.hpp"
#include "parser/helpers/member.hpp"
#include "parser/grammar/jesus_grammar.hpp"
#include <memory>

std::unique_ptr<Expr> GetAttributeRule::parse(ParserContext &ctx)
{
    // Start with a primary expression to get the 'instance/object'
    auto expr = primary->parse(ctx);
    if (!expr)
        return nullptr;

    // -------------------------------------------
    // Parse postfix operations (chainable)
    // Primary → Postfix → Postfix → Postfix → ...
    // -------------------------------------------
    while (true)
    {
        // =========================
        // 1. LIST INDEX: expr[...]
        // =========================
        if (ctx.match(TokenType::LEFT_BRACKET))
        {
            auto indexExpr = grammar::Expression->parse(ctx);

            if (!indexExpr)
                throw std::runtime_error("Expected a value inside []. Example: list[0]");

            if (!ctx.match(TokenType::RIGHT_BRACKET))
                throw std::runtime_error("Expected ']' after index.");

            auto type = expr->getReturnType(ctx);
            if (!type->isA(KnownTypes::LIST))
                throw std::runtime_error("Cannot access index on type '" + type->name + "'. Expected a list.");

            type = indexExpr->getReturnType(ctx);
            if (!(type->isA(KnownTypes::INT)))
                throw std::runtime_error("Index must be an integer. Got '" + type->name + "' instead.");

            expr = std::make_unique<IndexExpr>(std::move(expr), std::move(indexExpr));

            continue;
        }

        // =============================
        // 2. ATTRIBUTE / METHOD ACCESS
        // =============================
        if (ctx.match(TokenType::IDENTIFIER))
        {
            // ----------------------------
            // Resolve attribute or method
            // ----------------------------
            if (auto varExpr = dynamic_cast<VariableExpr *>(expr.get()))
            {
                std::shared_ptr<CreationType> klass = ctx.getVarType(varExpr->name);
                std::string name = ctx.previous().lexeme;

                auto member = klass->findMember(name, klass);
                if (!member)
                {
                    throw std::runtime_error("Unknown member '" + name + "' in class " + klass->name);
                }

                // -----------------
                // ATTRIBUTE ACCESS
                // -----------------
                if (member->isAttribute())
                {
                    expr = std::make_unique<GetAttributeExpr>(std::move(expr), name);
                }

                // ------------
                // METHOD CALL
                // ------------
                else if (member->isMethod())
                {
                    std::vector<std::unique_ptr<Expr>> args;

                    // If the next token(s) indicate arguments, parse them
                    if (!ctx.check(TokenType::NEWLINE) && !ctx.check(TokenType::END_OF_FILE))
                    {
                        do
                        {
                            auto argExpr = primary->parse(ctx); // parse any expression

                            if (!argExpr)
                                throw std::runtime_error("Expected argument for method " + name);

                            args.push_back(std::move(argExpr));

                        } while (ctx.match(TokenType::COMMA));
                    }

                    expr = std::make_unique<MethodCallExpr>(std::move(expr), member->method, std::move(args));
                }
                else
                {
                    throw std::runtime_error("Unknown member '" + name + "' in class " + klass->name);
                }
            }
            else
            {
                throw std::runtime_error("Cannot access member on this expression: '" + expr->toString() + "'");
            }

            continue;
        }

        // --------------------
        // No more postfix ops
        // --------------------
        break;
    }
    return expr;
}

std::string GetAttributeRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "GetAttributeExprRule(...)";

    visited.insert(this);

    return "GetAttributeExprRule()";
}
