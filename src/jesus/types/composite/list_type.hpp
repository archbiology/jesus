#pragma once

#include "../creation_type.hpp"

class Interpreter; // Forward declaration

/**
 * @brief Runtime representation of list<T>.
 */
class ListType : public CreationType
{
public:
    std::shared_ptr<CreationType> elementType;

    ListType(std::shared_ptr<CreationType> elementType, std::shared_ptr<CreationType> parent, std::string name = "list")
        : CreationType(PrimitiveType::Collection, name, "core", parent),
          elementType(elementType)
    {
        registerMethods();
    }

    bool validate(const Value &value) const override
    {
        if (!value.IS_LIST)
            return false;

        for (const auto &v : value.asList())
        {
            if (!elementType->validate(*v))  // FIXME: Isn't this '*' slow?
                return false;
        }

        return true;
    }

private:
    void registerMethods();

    static Value addItem(Interpreter &interp, Value &list, const std::vector<Value> &args);

    static Value removeItem(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.remove_at(index)
     *
     * @return Value removed value, or Value::formless() if index out of bounds
     */
    static Value removeAt(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.remove_first(): Remove the first item in the list
     *
     * @return Value the removed value, or Value::formeless() if the list is empty
     */
    static Value removeFirst(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.remove_last(): remove the last item in the list
     *
     * @return Value the removed value, or Value::formeless() if list is empty.
     */
    static Value removeLast(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.contains(value): check if the list contains a value
     *
     * @return Value Value(true) or Value(false)
     */
    static Value contains(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.count(): count how many itens there is in the list
     *
     * @return Value the count
     */
    static Value count(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.is_empty(): Check if the list is empty or not
     *
     * @return Value Value(true) or Value(false)
     */
    static Value isEmpty(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.first(): Return the first item in the list
     *
     * @return Value the first item, or Value::formless() if list is empty
     */
    static Value first(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.last(): Return the last item in the list
     *
     * @return Value the last item, or Value::formless() if list is empty
     */
    static Value last(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.clear(): remove all items in the list
     *
     * @return Value Value::formless()
     */
    static Value clear(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.reverse(): reverse the items in the list.
     *
     * @return Value Value::formless()
     */
    static Value reverse(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief list.sort(): sorts the list. Applies only to numbers and strings.
     *
     * @return Value Value::formeless()
     */
    static Value sort(Interpreter &, Value &list, const std::vector<Value> &args);

    /**
     * @brief b = a.copy(): deep copies a list
     *
     * @return Value the copied list.
     */
    static Value copy(Interpreter &, Value &list, const std::vector<Value> &args);
};
