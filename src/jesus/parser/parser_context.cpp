
#include "parser_context.hpp"
#include "../interpreter/interpreter.hpp"

ParserContext::ParserContext(std::vector<Token> tokens, Interpreter *interpreter, int current)
    : tokens(tokens), interpreter(interpreter), current(current) {}

void ParserContext::registerVarType(const std::string &type, const std::string &name)
{
    interpreter->registerVarType(type, name);
}

const bool ParserContext::variableExists(const std::string &varName) {
    return interpreter->variableExists(varName);
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
