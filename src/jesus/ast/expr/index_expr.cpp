#include "index_expr.hpp"
#include "interpreter/expr_visitor.hpp"
#include "types/known_types.hpp"
#include "interpreter/interpreter.hpp"
#include "types/composite/list_type.hpp"
#include "types/composite/dict_type.hpp"

Value IndexExpr::accept(ExprVisitor &visitor) const
{
    return visitor.visitIndexExpr(*this);
}

std::shared_ptr<CreationType> IndexExpr::getReturnType(ParserContext &ctx) const
{
    auto type = collection->getReturnType(ctx);

    if (auto listType = std::dynamic_pointer_cast<ListType>(type))
        return listType->elementType;

    if (auto dictType = std::dynamic_pointer_cast<DictType>(type))
        return dictType->valueType;

    return KnownTypes::CREATION;
};

void IndexExpr::assign(Interpreter &interpreter, const Value &value) const
{
    Value container = collection->accept(interpreter);
    Value indexValue = index->accept(interpreter);

    // -----------------
    // LIST ASSIGNMENT
    // -----------------
    if (container.IS_LIST)
    {
        if (!indexValue.IS_NUMBER)
            throw std::runtime_error("List index must be an integer.");

        auto &list = container.asList();
        int index = indexValue.toInt();

        if (index < 0 || index >= list.size())
            throw std::runtime_error("Index " + std::to_string(index) + " is not available.");

        list[index] = std::make_shared<Value>(value);

        return;
    }

    // ----------------
    // DICT ASSIGNMENT
    // ----------------
    if (container.IS_DICT)
    {
        auto &dict = container.asDict();

        for (auto &[existingKey, existingValue] : dict)
        {
            if (*existingKey == indexValue)
            {
                existingValue = std::make_shared<Value>(value);
                return;
            }
        }

        // ------------------
        // New key insertion
        // ------------------
        dict.push_back({std::make_shared<Value>(indexValue), std::make_shared<Value>(value)});

        return;
    }

    throw std::runtime_error("Only lists and dicts support [ ] assignment.");
}
