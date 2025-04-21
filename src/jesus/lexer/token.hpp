#pragma once

#include <string>
#include <variant>
#include "token_type.hpp"

using LiteralValue = std::variant<std::string, double, bool, std::nullptr_t>;

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

    /**
     * @brief The exact sequence of characters as it appeared in the source code.
     *
     * For example, in `print("Hello")`, the lexeme for the string token is `"Hello"`.
     *
     * "For truly I say to you, until heaven and earth pass away, not an iota, not a dot,
     * will pass from the Law until all is accomplished." – Matthew 5:18
     */
    std::string lexeme;

    /**
     * @brief The interpreted value of the token, if applicable (e.g., a number, string, boolean).
     *
     * This holds the actual runtime value the lexeme represents.
     *  lexeme  -> "123"
     *  literal -> 123
     *
     * "The unfolding of Your words gives light; it gives understanding to the simple." – Psalm 119:130
     */
    LiteralValue literal;

    Token(TokenType type, std::string lexeme, LiteralValue literal)
        : type(type), lexeme(std::move(lexeme)), literal(std::move(literal)) {}
};
