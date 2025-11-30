#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../ast/stmt/stmt.hpp"
#include "../../types/creation_type.hpp"
#include "instance.hpp"

class Interpreter; // Forward declaration

REGISTER_FOR_UML(
    Method,
    .packageName("interpreter.runtime")
        .fieldsList({"name", "params", "body", "returnType"}));

class ReturnSignal
{
public:
    Value value;

public:
    ReturnSignal(Value value) : value(std::move(value)) {}
};

class Method
{
public:
    const std::string name;
    const std::shared_ptr<Heart> params;
    const std::vector<std::shared_ptr<Stmt>> body;
    const std::shared_ptr<CreationType> returnType;

    Method(std::string name,
           std::shared_ptr<Heart> params,
           std::vector<std::shared_ptr<Stmt>> body,
           std::shared_ptr<CreationType> returnType)
        : name(std::move(name)), params(std::move(params)),
          body(std::move(body)), returnType(std::move(returnType)) {}

    Value call(Interpreter &interp, std::shared_ptr<Instance> instance, std::vector<Value> args);

    /**
     * @brief Get the return type, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx)
    {
        return returnType;
    }

    std::string toString()
    {
        std::string str = "{type: \"method\",\n params: {";
        if (!params->isEmpty())
        {
            str += params->toString();
        }
        str += " }\n}";
        return str;
    }
};
