#pragma once

#include "../lexer/token.hpp"
#include "../lexer/token_type.hpp"
#include "../semantic/semantic_analyser.hpp"

#include <string>
#include <vector>
#include <stdexcept>

class Interpreter; // Forward declaration

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
    Interpreter *interpreter = nullptr;

    explicit ParserContext(std::vector<Token> tokens, Interpreter *interpreter, int current = 0);

    /**
     * @brief Returns the current token without advancing.
     *
     * "Watch and pray so that you will not fall into temptation." – Matthew 26:41
     *
     * @return Token The current token in the stream
     */
    const Token &peek() const
    {
        return tokens[current];
    }

    /**
     * @brief Returns the most recently consumed token
     *
     * "Remember the former things of old: for I am God, and there is none else." – Isaiah 46:9
     *
     * @return Token The last token consumed.
     */
    const Token &previous() const
    {
        if (current == 0)
            throw std::runtime_error("No previous token");

        return tokens[current - 1];
    }

    /**
     * @brief Advances to the next token if not at end and returns the previous one.
     *
     * "Let us go on to maturity, not laying again the foundation..." – Hebrews 6:1
     *
     * @return Token The token that was just passed over.
     */
    const Token &advance()
    {
        if (!isAtEnd())
            current++;

        return previous();
    }

    /**
     * @brief Checks if the next token matches expected type and advances if true.
     *
     * "Test all things; hold fast what is good." – 1 Thessalonians 5:21
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

    /**
     * @brief Checks if the current token matches any of the given types and advances if true.
     *
     * "Two are better than one, because they have a good return for their labor." – Ecclesiastes 4:9
     */
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
     * @brief Checks if current token is of the given type without advancing.
     *
     * "Examine yourselves, to see whether you are in the faith." – 2 Corinthians 13:5
     */
    bool check(TokenType type) const
    {
        if (isAtEnd())
            return false;

        return peek().type == type;
    }

    /**
     * @brief Check if all tokens have been consumed
     *
     * "I am the Alpha and the Omega, the First and the Last, the Beginning and the End." – Revelation 22:13
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

    /**
     * @brief Return current token position
     */
    int snapshot() const
    {
        return this->current;
    }

    void restore(int snapshot)
    {
        current = snapshot;
    }

    void registerVarType(const std::string &varName, const std::string &typeName)
    {
        SemanticAnalyzer::registerVarType(varName, typeName);
    }

    void registerClassName(const std::string &className)
    {
        SemanticAnalyzer::registerClassName(className);
    }

    const CreationType *getVarType(const std::string &varName)
    {
        return SemanticAnalyzer::getVarType(varName);
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
