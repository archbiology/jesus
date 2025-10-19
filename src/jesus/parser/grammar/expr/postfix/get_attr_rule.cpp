#include "get_attr_rule.hpp"
#include "../../../../ast/expr/get_attr_expr.hpp"
#include "../../../../ast/expr/variable_expr.hpp"
#include "../../../../ast/expr/method_call_expr.hpp"
#include "../../../../types/creation_type.hpp"
#include "../../../helpers/member.hpp"
#include <memory>

std::unique_ptr<Expr> GetAttributeRule::parse(ParserContext &ctx)
{
    // Start with a primary expression to get the 'instance/object'
    auto expr = primary->parse(ctx);
    if (!expr)
        return nullptr;

    // Parse more identifiers for attribute access: say person name
    while (ctx.match(TokenType::IDENTIFIER))
    {
        if (auto varExpr = dynamic_cast<VariableExpr *>(expr.get()))
        {
            std::shared_ptr<CreationType> klass = ctx.getVarType(varExpr->name);
            std::string name = ctx.previous().lexeme;

            auto member = klass->findMember(name, klass);
            if (!member)
            {
                throw std::runtime_error("Unknown member '" + name + "' in class " + klass->name);
            }

            if (member->isAttribute())
            {
                expr = std::make_unique<GetAttributeExpr>(std::move(expr), name);
            }
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
            throw std::runtime_error("Not prepared for this expression yet: '" + expr->toString() + "'");
        }
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
