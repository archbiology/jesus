#pragma once

#include <string>
#include <memory>
#include "../../spirit/heart.hpp"
#include "../../spirit/symbol_table.hpp"

REGISTER_FOR_UML(
    Module,
    .fieldsList({"name", "file_path", "scope: Heart"})
        .methodsList({"createVar", "getVar"}));

class Module
{
public:
    const std::string name;
    const std::string file_path;

    /**
     * @brief It acts like a symbol table — each variable has a name (string)
     *
     * "The good man brings good things out of the good stored up in his heart…" — Luke 6:45
     */
    std::shared_ptr<SymbolTable> symbol_table;

    explicit Module(std::string name, std::string path, std::shared_ptr<SymbolTable> symbol_table)
        : name(name), file_path(path), symbol_table(std::move(symbol_table)) {}

    void createVar(const VarType &type, const std::string &name, const Value &value)
    {
        symbol_table->createVar(type, name, value);
    }

    bool varExistsInHierarchy(const std::string &name) {
        return symbol_table->varExistsInHierarchy(name);
    }

    Value getVar(const std::string &name) const
    {
        return symbol_table->getVar(name);
    }

    std::shared_ptr<CreationType> getVarType(const std::string &name) const
    {
        return symbol_table->getVarType(name);
    }

    std::string toString()
    {

        std::string str = "{type: \"module\",\n name: \"" + name + "\",\n path: \"" + file_path + "\", \n members: {";

        str += symbol_table->currentScope()->toString();
        str += " }\n}";
        return str;
    }
};
