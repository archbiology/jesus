#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../ast/stmt/stmt.hpp"
#include "../../types/creation_type.hpp"
#include "instance.hpp"

class Interpreter; // Forward declaration

class Method
{
public:
    const std::string name;
    const std::shared_ptr<Heart> params;
    const std::vector<std::shared_ptr<Stmt>> body;

    const std::shared_ptr<CreationType> klass;

    Method(std::string name,
           std::shared_ptr<Heart> params,
           std::vector<std::shared_ptr<Stmt>> body,
           std::shared_ptr<CreationType> klass)
        : name(std::move(name)), params(std::move(params)), body(std::move(body)), klass(std::move(klass)) {}

    Value call(Interpreter &interp, Instance instance, std::vector<Value> args);
};
