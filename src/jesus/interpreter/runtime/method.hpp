#pragma once

#include <string>
#include <vector>
#include <utility> // for std::pair
#include <memory>
#include "../../spirit/return_signal.hpp"
#include "../../spirit/heart.hpp"
#include "../../types/creation_type.hpp"
#include "instance.hpp"

class Interpreter; // Forward declaration
class ParserContext;
class CreateMethodStmt;

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
    /**
     * @brief The AST method declaration.
     *
     * The AST must remain the single source of truth for the method body.
     * The runtime method only points to the AST so that optimization passes
     * can safely optimize the method body in a single place.
     */
    const CreateMethodStmt *definition;

    /**
     * @brief Constructor parameters defined as
     * private/protected/public become instance attributes.
     * Each entry is (name, access).
     */
    const std::vector<std::pair<std::string, std::string>> attributeNames;

    Method(
        std::string name,
        std::shared_ptr<Heart> params,
        CreateMethodStmt *definition,
        std::shared_ptr<CreationType> returnType,
        std::vector<std::pair<std::string, std::string>> attributeNames = {})
        : IMethod(std::move(name), std::move(params), std::move(returnType)), definition(definition),
          attributeNames(std::move(attributeNames))
    {
    }

    Value call(Interpreter &interp, Value &object, const std::vector<Value> &args) override;
};
