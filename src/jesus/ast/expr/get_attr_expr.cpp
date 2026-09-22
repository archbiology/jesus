#include "get_attr_expr.hpp"
#include "../../interpreter/expr_visitor.hpp"
#include "../../types/creation_type.hpp"
#include "../../parser/parser_context.hpp"
#include "interpreter/interpreter.hpp"
#include "interpreter/runtime/instance.hpp"
#include "enum_member_expr.hpp"

Value GetAttributeExpr::accept(ExprVisitor &visitor) const { return visitor.visitGetAttribute(*this); }

Value GetAttributeExpr::evaluate(std::shared_ptr<Heart> heart) const
{
    Value obj = object->evaluate(heart);

    std::shared_ptr<Instance> instance = obj.toInstance();

    if (!instance)
    {
        throw std::runtime_error(
            "Cannot access attribute '" + attribute + "' on '" + obj.toString() + "' value.\n" +
            "Tip: ensure the value is a valid object before accessing its attributes.");
    }

    return instance->getAttribute(address);
}

std::shared_ptr<CreationType> GetAttributeExpr::getReturnType(ParserContext &ctx) const
{
    // ----------------------------------------------
    // Enum: handle .label (text) and .value (number)
    // ----------------------------------------------
    std::shared_ptr<CreationType> baseType = object->getReturnType(ctx);
    if (baseType && baseType->isEnum())
    {
        if (attribute == "label")
            return KnownTypes::STRING;

        if (attribute == "value")
            return KnownTypes::INT;
    }

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

    interpreter.enforceAttributeAccess(instance->spirit, attribute);

    instance->setAttribute(address, value);
}
