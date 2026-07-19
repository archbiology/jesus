#pragma once

#include "spirit_of_the_lord.hpp"
#include "spirit_of_understanding.hpp"
#include <string>
#include <unordered_map>
#include "value.hpp"
#include "../understanding/core/REGISTER_FOR_UML.hpp"

#define STRINGIFY(x) #x

REGISTER_FOR_UML(
    Heart,
    .fieldsList({"variables", "semantics_analyzer: SpiritOfUnderstanding"})
        .methodsList({"clone", "createVar", "getVar", "updateVar",
                      "registerVarType", "updatePolymorphicVarType",
                      "registerClassName"}));

struct VariableAddress
{
    uint32_t slot;
    uint32_t scopeId;
};

/**
 * @brief The Heart class stores variables declared during execution.
 *
 * It acts like a symbol table — each variable has a name (string)
 * and an associated value (also a string for now).
 *
 * “For where your treasure is, there your heart will be also.” — Matthew 6:21
 *
 * “The good man brings good things out of the good stored up in his heart…” — Luke 6:45
 *
 * Heart holds what the program “believes” or “remembers”.
 */
class Heart : JesusProgrammingLanguage
{

public:
    uint32_t scopeId;
    const std::string scope_name;

    /**
     * @brief Default constructor
     *
     * @param scope_name the scope name. E.g.: global, className, methodName
     */
    explicit Heart(std::string scope_name, std::shared_ptr<Heart> parent = nullptr)
        : scope_name(scope_name),
          scopeId(nextId++),
          parent_attributes(std::move(parent)),
          semantics_analyzer(std::make_shared<SpiritOfUnderstanding>()) {}

    /**
     * @brief Construct a new Heart object
     *
     * @param new_scope_name The scope name of the final copy
     */
    std::shared_ptr<Heart> clone(const std::string &new_scope_name, std::shared_ptr<Heart> parent_attributes_override = nullptr) const
    {
        auto copy = std::make_shared<Heart>(new_scope_name);
        copy->slots = slots;
        copy->values = values;
        copy->paramsCount = paramsCount;
        copy->scopeId = scopeId;
        copy->variableOrder = variableOrder;
        copy->semantics_analyzer = semantics_analyzer;

        if (parent_attributes_override)
            copy->parent_attributes = parent_attributes_override;
        else
            copy->parent_attributes = parent_attributes ? parent_attributes->clone(new_scope_name + ":parent") : nullptr;

        return copy;
    }

    /**
     * @brief Checks whether a variable with the given name exists in the current local scope.
     *
     * It does not search in parent or global scopes.
     *
     * "Each one should test their own actions, then they can rejoice in themselves alone,
     * without comparing themselves to someone else." — Galatians 6:4
     */
    bool localVarExists(const std::string &name) const;

    /**
     * @brief Checks whether a variable or attribute with the given name exists
     * in the current scope or any parent class (inherited attributes).
     *
     * This function searches for the variable name first in the current context, and
     * then recursively in parent class attributes. It does *not* check any global or
     * external scope beyond the inheritance chain.
     *
     * 📖 "Be sure you know the condition of your flocks, give careful attention to your herds." — Proverbs 27:23
     *
     * @param name The name of the variable to check.
     * @return true if the variable is already defined, false otherwise.
     */
    bool varExistsInHierarchy(const std::string &name) const;

    /**
     * @brief Create a variable with the given name and value.
     * If the variable already exists, it will raise an exception.
     *
     * "And God called the light Day, and the darkness he called Night." - Genesis 1:5
     * Just as God gave names and meaning, this method gives values to variables
     *
     * @param type The name of the variable type
     * @param name The name of the variable (e.g., "age")
     * @param value The value to assign (e.g., "33")
     */
    void createVar(const VarType &type, const std::string &name, const Value &value, bool isParam = false);
    VariableAddress declareVar(const VarType &type, const std::string &name, bool isParam);

    /**
     * @brief Retrieves the value of a variable.
     *
     * ... “Therefore every scribe who has been made a disciple in the
     * Kingdom of Heaven is like a man who is a householder,
     * who brings out of his treasure new and old things.” - Matthew 13:52
     *
     * @param address The address of the variable to retrieve.
     * @return Value The value, which may be std::monostate if not found.
     */
    Value getVar(const VariableAddress address) const;

    /**
     * @brief Updates the value of an already existing variable.
     *
     * "Do not conform to the pattern of this world, but be transformed
     * by the renewing of your mind." — Romans 12:2
     *
     * @param name The name of the variable to update.
     * @param value The new value to assign to the variable.
     *
     * @throws std::runtime_error If the variable doesn't exist yet.
     */
    void updateVar(const uint32_t slot, const Value &value);
    void updateVar(const VariableAddress &address, const Value &value);

    bool isEmpty() const
    {
        return values.empty();
    }

    bool size() const
    {
        return values.size();
    }

    void registerVarType(const VarType &type, const std::string &name)
    {
        semantics_analyzer->registerVarType(type, name);
    }

    void updatePolymorphicVarType(const std::string &name, const VarType &type)
    {
        semantics_analyzer->updatePolymorphicVarType(name, type);
    }

    void registerClassName(const std::string &className)
    {
        semantics_analyzer->registerClassName(className);
    }

    bool isClassKnown(const std::string &className) const
    {
        return semantics_analyzer->isClassKnown(className);
    }

    const std::shared_ptr<CreationType> getVarType(const std::string &varName)
    {
        auto type = semantics_analyzer->getVarType(varName);
        return type;
    }

    const std::vector<std::string> &getVariableNames() const
    {
        return variableOrder;
    }

    void appendToString(std::string &out, std::unordered_set<std::string> &vars_printed) const
    {

        for (size_t i = 0; i < variableOrder.size(); ++i)
        {
            const auto &name = variableOrder[i];

            if (vars_printed.contains(name))
                continue;

            vars_printed.insert(name);

            out += "\n  " + name + ": \"" + values[i].toString() + "\",";
        }

        if (parent_attributes)
            parent_attributes->appendToString(out, vars_printed);
    }

    const std::string toString() const
    {
        std::string str = "";
        std::unordered_set<std::string> vars_printed;

        appendToString(str, vars_printed);

        bool removeLastComma = !str.empty();

        if (removeLastComma)
        {
            str.pop_back();
            str += "\n";
        }

        return str;
    }

    VariableAddress resolveVariableAddress(const std::string &name) const
    {
        if (!slots.contains(name))
        {
            throw std::runtime_error("No slot found in " + scope_name + " for : " + name);
        }

        return {.slot = slots.at(name), .scopeId = scopeId};
    }

    /**
     * @brief Resolves a variable slot walking the inheritance chain (child before parent).
     */
    VariableAddress resolveVariableAddressInHierarchy(const std::string &name) const
    {
        if (slots.contains(name))
            return {.slot = slots.at(name), .scopeId = scopeId};

        if (parent_attributes)
            return parent_attributes->resolveVariableAddressInHierarchy(name);

        throw std::runtime_error("No slot found in hierarchy of " + scope_name + " for: " + name);
    }

    int paramsCount = 0;

private:
    std::shared_ptr<Heart> parent_attributes = nullptr;
    std::unordered_map<std::string, uint32_t> slots;
    static uint32_t nextId;

    std::vector<std::string> variableOrder; // insertion order
    std::vector<Value> values;
    std::shared_ptr<SpiritOfUnderstanding> semantics_analyzer;
};
