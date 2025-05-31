#pragma once

#include <string>
#include <unordered_map>
#include "value.hpp"

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
class Heart
{
public:
    /**
     * @brief Sets a variable with the given name and value.
     * If the variable already exists, it will be updated.
     *
     * "And God called the light Day, and the darkness he called Night." - Genesis 1:5
     * Just as God gave names and meaning, this method gives values to variables
     *
     * @param name The name of the variable (e.g., "age")
     * @param value The value to assign (e.g., "33")
     */
    void define(const std::string &name, const Value& value);

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
    Value get(const std::string& name) const;

    void assign(const std::string& name, const Value& value);

private:
    std::unordered_map<std::string, Value> variables;
};
