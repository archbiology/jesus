#pragma once

#include "spirit_of_the_lord.hpp"
#include "spirit_of_understanding.hpp"
#include <string>
#include <unordered_map>
#include "value.hpp"

#define STRINGIFY(x) #x

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
    const std::string scope_name;

    /**
     * @brief Default constructor
     *
     * @param scope_name the scope name. E.g.: global, className, methodName
     */
    explicit Heart(std::string scope_name) : scope_name(std::move(scope_name)), semantics_analyzer(std::make_shared<SpiritOfUnderstanding>()) {}

    /**
     * @brief Construct a new Heart object
     *
     * @param scope_name The scope name of the final copy
     * @param other The original that will be used as a copy
     */
    std::shared_ptr<Heart> clone(const std::string &new_scope_name) const
    {
        auto copy = std::make_shared<Heart>(scope_name);
        copy->variables = variables;
        copy->variableOrder = variableOrder;
        copy->semantics_analyzer = semantics_analyzer;
        return copy;
    }

    /**
     * @brief Checks whether a variable with the given name already exists.
     *
     * 📖 "Be sure you know the condition of your flocks, give careful attention to your herds." — Proverbs 27:23
     *
     * @param name The name of the variable to check.
     * @return true if the variable is already defined, false otherwise.
     */
    bool varExists(const std::string &name) const;

    /**
     * @brief Create a variable with the given name and value.
     * If the variable already exists, it will raise an exception.
     *
     * "And God called the light Day, and the darkness he called Night." - Genesis 1:5
     * Just as God gave names and meaning, this method gives values to variables
     *
     * @param type The name of the variable type (e.g., "int")
     * @param name The name of the variable (e.g., "age")
     * @param value The value to assign (e.g., "33")
     */
    void createVar(const std::string &type, const std::string &name, const Value &value);

    /**
     * @brief Retrieves the value of a variable.
     *
     * ... “Therefore every scribe who has been made a disciple in the
     * Kingdom of Heaven is like a man who is a householder,
     * who brings out of his treasure new and old things.” - Matthew 13:52
     *
     * @param name The name of the variable to retrieve.
     * @return Value The value, which may be std::monostate if not found.
     */
    Value getVar(const std::string &name) const;

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
    void updateVar(const std::string &name, const Value &value);

    bool isEmpty() const
    {
        return variables.empty();
    }

    void registerVarType(const std::string &type, const std::string &name)
    {
        semantics_analyzer->registerVarType(type, name);
    }

    void registerClassName(const std::string &className)
    {
        semantics_analyzer->registerClassName(className);
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

    const std::string toString() const
    {
        std::string str = "";
        bool removeLastComma = false;

        for (auto &pair : variables)
        {
            const std::string &key = pair.first;
            const Value &value = pair.second;

            str += "\n  " + key + ": \"" + value.toString() + "\",";
            removeLastComma = true;
        }

        if (removeLastComma)
        {
            str.pop_back();
            str += "\n";
        }

        return str;
    }

private:
    std::vector<std::string> variableOrder; // insertion order
    std::unordered_map<std::string, Value> variables;
    std::shared_ptr<SpiritOfUnderstanding> semantics_analyzer;
};
