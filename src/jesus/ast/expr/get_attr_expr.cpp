#include "get_attr_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/creation_type.hpp"
#include "../../parser/parser_context.hpp"
#include "interpreter/interpreter.hpp"
#include "interpreter/runtime/instance.hpp"

Value GetAttributeExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitGetAttribute(*this);
}

std::shared_ptr<CreationType> GetAttributeExpr::getReturnType(ParserContext &ctx) const
{
    return ctx.getVarType(attribute);
};

void GetAttributeExpr::assign(Interpreter &interpreter, const Value &value) const
{
    Value objectVal = object->accept(interpreter);

    std::shared_ptr<Instance> instance = objectVal.toInstance();

    if (!instance)
    {
        throw std::runtime_error(
            "Cannot access attribute '" + attribute + "' on '" + objectVal.toString() + "' value.\n" +
            "Tip: ensure the value is a valid object before accessing its attributes.");
    }

    instance->setAttribute(attribute, value);
}
