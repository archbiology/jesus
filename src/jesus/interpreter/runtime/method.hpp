#pragma once

#include <string>
#include <vector>
#include <memory>
#include "../../ast/stmt/stmt.hpp"
#include "../../spirit/return_signal.hpp"
#include "../../spirit/heart.hpp"
#include "../../types/creation_type.hpp"
#include "instance.hpp"

class Interpreter; // Forward declaration
class ParserContext;

REGISTER_FOR_UML(
    IMethod,
    .packageName("interpreter.runtime")
        .fieldsList({"name", "params", "returnType"}));

REGISTER_FOR_UML(
    Method,
    .packageName("interpreter.runtime")
        .parentsList({"IMethod"})
        .fieldsList({"body"}));

class IMethod
{
public:
    const std::string name;
    const std::shared_ptr<Heart> params;
    const std::shared_ptr<CreationType> returnType;

    virtual ~IMethod() = default;

    virtual Value call(
        Interpreter &interp,
        Value &object,
        const std::vector<Value> &args) = 0;

    virtual std::shared_ptr<CreationType> getReturnType(ParserContext &ctx)
    {
        return returnType;
    }

    virtual std::string toString() const
    {
        std::string str = "{type: \"method\", name: \"" + name + "\", params: {";
        if (!params->isEmpty())
        {
            str += params->toString();
        }
        str += " }}";
        return str;
    }

protected:
    IMethod(
        std::string name,
        std::shared_ptr<Heart> params,
        std::shared_ptr<CreationType> returnType)
        : name(std::move(name)),
          params(std::move(params)),
          returnType(std::move(returnType)) {}
};

class Method : public IMethod
{
public:
    const std::vector<std::shared_ptr<Stmt>> body;

    Method(std::string name,
           std::shared_ptr<Heart> params,
           std::vector<std::shared_ptr<Stmt>> body,
           std::shared_ptr<CreationType> returnType)
        : IMethod(std::move(name), std::move(params), std::move(returnType)),
          body(std::move(body)) {}

    Value call(Interpreter &interp, Value &object, const std::vector<Value> &args) override;
};
