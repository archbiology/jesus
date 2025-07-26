
#include "parser_context.hpp"
#include "../interpreter/interpreter.hpp"

ParserContext::ParserContext(std::vector<Token> tokens, Interpreter *interpreter, int current)
    : tokens(tokens), interpreter(interpreter), current(current) {}
