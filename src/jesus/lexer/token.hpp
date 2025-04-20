#include <string>
#include "token_type.hpp"

/**
 * @brief Represents a lexical token with a type and string value.
 *
 * Tokens are the result of breaking down a line of input text.
 * Each token has a `TokenType` and an associated `value`.
 */
class Token
{
public:
    TokenType type;
    std::string value;
};
