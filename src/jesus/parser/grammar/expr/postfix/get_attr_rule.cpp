#include "get_attr_rule.hpp"
#include "../../../../ast/expr/get_attr_expr.hpp"
#include "../../../../ast/expr/variable_expr.hpp"
#include "../../../../ast/expr/method_call_expr.hpp"
#include "../../../../types/creation_type.hpp"
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
            std::string member = ctx.previous().lexeme;

            // If it is an attribute
            if (klass->attributeExists(member))
            {
                expr = std::make_unique<GetAttributeExpr>(std::move(expr), member);
            }
            // If it is a method
            else if (std::shared_ptr<Method> method = klass->findMethod(member))
            {
                expr = std::make_unique<MethodCallExpr>(std::move(expr), method);
            }
            else
            {
                throw std::runtime_error("Unknown member '" + member + "' in class " + klass->name);
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
