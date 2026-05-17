#pragma once

#include "method.hpp"
#include <functional>

REGISTER_FOR_UML(
    NativeMethod,
    .packageName("interpreter.runtime")
        .parentsList({"IMethod"})
        .fieldsList({"fn: NativeFn"}));

using NativeFn = std::function<Value(Interpreter &, Value &obj, const std::vector<Value> &args)>;

class NativeMethod : public IMethod
{
private:
    NativeFn fn;

public:
    NativeMethod(
        std::string name,
        std::shared_ptr<Heart> params,
        std::shared_ptr<CreationType> returnType,
        NativeFn fn)
        : IMethod(std::move(name), std::move(params), std::move(returnType)),
          fn(std::move(fn)) {}

    Value call(Interpreter &interp, Value &object, const std::vector<Value> &args) override
    {
        return fn(interp, object, args);
    }
};
