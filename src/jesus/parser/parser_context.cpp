
#include "parser_context.hpp"
#include "../interpreter/interpreter.hpp"
#include "interpreter/runtime/module.hpp"

ParserContext::ParserContext(
    std::vector<Token> tokens, Interpreter *interpreter,
    const std::string &moduleName, int current)
    : tokens(tokens), interpreter(interpreter), moduleName(moduleName), current(current) {}

bool ParserContext::varExistsInHierarchy(const std::string &name)
{
    return interpreter->varExistsInHierarchy(name);
}

void ParserContext::registerVarType(const VarType &type, const std::string &name)
{
    interpreter->registerVarType(type, name);
}

VariableAddress ParserContext::declareVar(const VarType &type, const std::string &name)
{
    return interpreter->declareVar(type, name);
}

void ParserContext::updatePolymorphicVarType(const std::string &name, const VarType &type)
{
    interpreter->updatePolymorphicVarType(name, type);
}

const std::shared_ptr<CreationType> ParserContext::getVarType(const std::string &varName)
{
    return interpreter->getVarType(varName);
}

std::shared_ptr<CreationType> ParserContext::resolveType(const std::string &name)
{
    return interpreter->resolveType(name);
}

void ParserContext::registerType(const std::shared_ptr<CreationType> &type)
{
    interpreter->registerParseTimeType(type);
}

bool ParserContext::isClassKnown(const std::string &className) const
{
    return interpreter->isClassKnown(className);
}

void ParserContext::registerClassName(const std::string &className)
{
    interpreter->registerClassName(className);
}

void ParserContext::addScope(std::shared_ptr<Heart> scope)
{
    interpreter->addScope(scope);
}

void ParserContext::popScope()
{
    interpreter->popScope();
}

VariableAddress ParserContext::resolveVariableAddress(const std::string &name)
{
    return interpreter
        ->currentModule
        ->symbol_table
        ->resolveVariableAddress(name);
}

const std::shared_ptr<CreationType> ParserContext::currentClassType()
{
    const auto &scopes = interpreter->currentModule->symbol_table->getScopes();

    // The las open "class:<Name>" scope is the class currently being parsed.
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it)
    {
        const std::string &scopeName = (*it)->scope_name;
        if (scopeName.rfind("class:", 0) == 0)
        {
            auto type = resolveType(scopeName.substr(6));
            if (type)
                return type;
        }
    }

    return nullptr;
}
