#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include "../../spirit/heart.hpp"
#include "../../spirit/symbol_table.hpp"
#include "../../ast/stmt/stmt.hpp"

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

    /**
     * @brief Top-level AST nodes that must outlive execution of this module.
     *
     * A module may be imported by another module. Because Method::definition
     * is a raw pointer into the AST, the nodes are retained here so they stay
     * valid after the module's interpreter has finished running.
     *
     * "The stone the builders rejected has become the cornerstone." — Psalm 118:22
     */
    std::vector<std::unique_ptr<Stmt>> retainedAst;

    /**
     * @brief Imported names bound in this module, mapped to the full path of
     * the module they were imported from.
     *
     * Importing the same module twice in the same module must not declare the
     * imported name twice. This record lets the parser ignore a repeated
     * import while still reporting a real conflict when two different modules
     * expose the same name.
     */
    std::unordered_map<std::string, std::string> importedNames;

    explicit Module(std::string name, std::string path, std::shared_ptr<SymbolTable> symbol_table)
        : name(name), file_path(path), symbol_table(std::move(symbol_table)) {}

    void createVar(const VarType &type, const std::string &name, const Value &value, bool isParam)
    {
        symbol_table->createVar(type, name, value, isParam);
    }

    bool varExistsInHierarchy(const std::string &name)
    {
        return symbol_table->varExistsInHierarchy(name);
    }

    Value getVar(const VariableAddress address) const
    {
        return symbol_table->getVar(address);
    }

    VariableAddress resolveVariableAddress(std::string &name)
    {
        return symbol_table->resolveVariableAddress(name);
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
