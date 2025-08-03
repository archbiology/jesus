#include "ask_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"

Value AskExpr::evaluate(Heart *heart) const
{
    auto question = prompt->evaluate(heart);
    return question;
}

Value AskExpr::accept(ExprVisitor &visitor) const
{
    Value answer = visitor.visitAsk(*this);
    return answer;
}
