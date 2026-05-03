#include "index_expr.hpp"
#include "interpreter/expr_visitor.hpp"
#include "types/known_types.hpp"
#include "interpreter/interpreter.hpp"

Value IndexExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitIndexExpr(*this);
}

std::shared_ptr<CreationType> IndexExpr::getReturnType(ParserContext &ctx) const
{
    return KnownTypes::CREATION;
};

void IndexExpr::assign(Interpreter &interpreter, const Value &value) const
{
    Value listVal = list->accept(interpreter);
    Value indexVal = index->accept(interpreter);

    if (!listVal.IS_LIST)
        throw std::runtime_error("Cannot assign to index of non-list.");

    if (!indexVal.IS_NUMBER)
        throw std::runtime_error("Index must be an integer.");

    auto &items = listVal.asList();
    int idx = indexVal.toInt();

    if (idx < 0 || idx >= items.size())
        throw std::runtime_error("Index " + std::to_string(idx) + " is not available.");

    items[idx] = std::make_shared<Value>(value);
}
