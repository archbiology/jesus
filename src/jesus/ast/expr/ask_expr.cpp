#include "ask_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/creation_type.hpp"

Value AskExpr::evaluate(std::shared_ptr<Heart> heart) const
{
    auto question = prompt->evaluate(heart);
    return question;
}

Value AskExpr::accept(ExprVisitor &visitor) const
{
    Value answer = visitor.visitAsk(*this);
    return answer;
}

std::shared_ptr<CreationType> AskExpr::getReturnType(ParserContext &ctx) const
{
    return prompt->getReturnType(ctx);
};
