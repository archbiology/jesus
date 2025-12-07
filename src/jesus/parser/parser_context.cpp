
#include "parser_context.hpp"
#include "../interpreter/interpreter.hpp"

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

void ParserContext::updatePolymorphicVarType(const std::string &name, const VarType &type)
{
    interpreter->updatePolymorphicVarType(name, type);
}

const std::shared_ptr<CreationType> ParserContext::getVarType(const std::string &varName)
{
    return interpreter->getVarType(varName);
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
