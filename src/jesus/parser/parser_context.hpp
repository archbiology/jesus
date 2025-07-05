#pragma once

#include "../lexer/token.hpp"
#include "../lexer/token_type.hpp"

#include <string>
#include <vector>
#include <stdexcept>

/**
 * @brief The context passed around during parsing, holding tokens and parsing position.
 *
 * It acts as a shared parsing state for all grammar rules
 * — keeping track of the token list, current position, and convenience functions
 *
 * “Wisdom is the principal thing; therefore get wisdom:
 * and with all thy getting get understanding.” — Proverbs 4:7
 */
class ParserContext
{
public:
    explicit ParserContext(const std::vector<Token> &tokens, int current=0)
        : tokens(tokens), current(current) {}

    /**
     * @brief Returns the current token without consuming it
     *
     * @return const Token&
     */
    const Token &peek() const
    {
        return tokens[current];
    }

    /**
     * @brief Returns the previous token
     *
     * @return const Token&
     */
    const Token &previous() const
    {
        if (current == 0)
            throw std::runtime_error("No previous token");

        return tokens[current - 1];
    }

    /**
     * @brief Advances to the next token if not at end
     *
     * @return const Token&
     */
    const Token &advance()
    {
        if (!isAtEnd())
            current++;

        return previous();
    }

    /**
     * @brief Checks if the next token matches expected type
     */
    bool match(TokenType type)
    {
        if (check(type))
        {
            advance();
            return true;
        }

        return false;
    }

    bool matchAny(const std::initializer_list<TokenType> &types)
    {
        for (auto type : types)
        {
            if (check(type))
            {
                advance();
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Checks if current token is of the given type
     */
    bool check(TokenType type) const
    {
        if (isAtEnd())
            return false;

        return peek().type == type;
    }

    /**
     * @brief Check if all tokens have been consumed
     */
    bool isAtEnd() const
    {
        return peek().type == TokenType::END_OF_FILE;
    }

    /**
     * @brief For error reporting
     */
    Token currentToken() const
    {
        return peek();
    }

    /**
     * @brief Rollback support (snapshot/rewind)
     */
    size_t position() const
    {
        return current;
    }

    void rewind(size_t pos)
    {
        current = pos;
    }

    ParserContext snapshot() const
    {
        ParserContext copy(this->tokens);
        copy.current = this->current;
        return copy;
    }

    std::string toString()
    {
        std::string str = "ParserContext(current: " + std::to_string(current) + ", tokens: ";

        for (auto token : tokens)
        {
            str += token.toString() + ", ";
        }

        str += ")";

        return str;
    }

private:
    std::vector<Token> tokens;
    size_t current = 0;
};
