#pragma once

#include <string>
#include <memory>
#include "../../spirit/heart.hpp"

class Module
{
public:
    std::string name;
    std::string file_path;

    // The module’s symbol table (its global scope)
    std::shared_ptr<Heart> scope = nullptr;

    Module(std::string name, std::string path, std::shared_ptr<Heart> scope_)
        : name(std::move(name)), file_path(std::move(path)), scope(std::move(scope_)) {}

    void createVar(const std::string &type, const std::string &name, const Value &value)
    {
        scope->createVar(type, name, value);
        ;
    }

    Value getVar(const std::string &name) const
    {
        return scope->getVar(name);
    }

    std::string toString()
    {

        std::string str = "{type: \"module\",\n name: \"" + name + "\",\n path: \"" + file_path + "\", \n members: {";

        str += scope->toString();
        str += " }\n}";
        return str;
    }
};
