#pragma once

#include "../lexer/token.hpp"
#include "../ast/expr/expr.hpp"
#include <memory>
#include <vector>

/**
 * @brief Parses a stream of tokens into an expression AST.
 *
 * "I believed; therefore I have spoken." — 2 Corinthians 4:13
 */
class ExpressionParser
{
public:
    explicit ExpressionParser(const std::vector<Token> &tokens);

    /**
     * @brief Entry point to parse an expression.
     *
     * "Let all things be done decently and in order." – 1 Corinthians 14:40
     *
     * @return A parsed expression tree.
     */
    std::unique_ptr<Expr> parse();

private:
    const std::vector<Token> tokens;
    size_t current;

    /**
     * @brief Parses logical OR expressions: expr or expr
     *
     * "Two are better than one... for if they fall, one will lift up his companion." – Ecclesiastes 4:9-10
     *
     * @return std::unique_ptr<Expr>
     */
    std::unique_ptr<Expr> parseOr();

    std::unique_ptr<Expr> parseVersus(); // parseXor

    /**
     * @brief Parses logical AND expressions: expr and expr
     *
     * "Can two walk together, unless they are agreed?" – Amos 3:3
     *
     * @return std::unique_ptr<Expr>
     */
    std::unique_ptr<Expr> parseAnd();

    /**
     * @brief Parses equality expressions: == and !=
     *
     * "The LORD loves righteousness and justice." – Psalm 33:5
     *
     * @return std::unique_ptr<Expr>
     */
    std::unique_ptr<Expr> parseEquality();

    /**
     * @brief Parses comparison expressions: <, <=, >, >=
     *
     * "For the LORD sees not as man sees." – 1 Samuel 16:7
     *
     * Matthew 20:26-28 –
     *  26 "Not so with you. Instead, whoever wants to become great among you must be your servant,
     *  27 and whoever wants to be first must be your slave
     *  28 just as the Son of Man did not come to be served, but to serve, and to give his life as a ransom for many.”
     *
     * @return std::unique_ptr<Expr>
     */
    std::unique_ptr<Expr> parseComparison();

    std::unique_ptr<Expr> parseAddition();

    std::unique_ptr<Expr> parseMultiplication();

    /**
     * @brief Parses unary expressions: not kill
     *
     * "Test all things; hold fast what is good." – 1 Thessalonians 5:21
     *
     * @return std::unique_ptr<Expr>
     */
    std::unique_ptr<Expr> parseUnaryExpression();

    /**
     * @brief Parses literals, identifiers, and grouped expressions
     *
     * "Your word is a lamp to my feet and a light to my path." – Psalm 119:105
     *
     * @return std::unique_ptr<Expr>
     */
    std::unique_ptr<Expr> parseLiteralOrGroupOrVar();

    /**
     * @brief Checks if the current token matches the given type and consumes it if true.
     *
     * "Test all things; hold fast what is good." – 1 Thessalonians 5:21
     *
     * @param type The token type to check.
     * @return true if the current token matches and was consumed, false otherwise.
     */
    bool match(TokenType type);

    /**
     * @brief Checks if the current token matches either of the given types and consumes it if true.
     *
     * "Two are better than one, because they have a good return for their labor." – Ecclesiastes 4:9
     *
     * @param type1 First token type to check.
     * @param type2 Second token type to check.
     * @return true if a match occurred and was consumed, false otherwise.
     */
    bool match(TokenType type1, TokenType type2);

    /**
     * @brief Checks if the current token matches either of the given types and consumes it if true.
     *
     * @param type1 First token type to check.
     * @param type2 Second token type to check.
     * @param type3 Third token type to check.
     *
     * @return true if a match occurred and was consumed, false otherwise.
     */
    bool match(TokenType type1, TokenType type2, TokenType type3);

    /**
     * @brief Checks if the current token matches any of the given four types and consumes it if true.
     *
     * "The word of God is alive and active. Sharper than any double-edged sword..." – Hebrews 4:12
     *
     * @param type1 First token type to check.
     * @param type2 Second token type to check.
     * @param type3 Third token type to check.
     * @param type4 Fourth token type to check.
     * @return true if a match occurred and was consumed, false otherwise.
     */
    bool match(TokenType type1, TokenType type2, TokenType type3, TokenType type4);

    /**
     * @brief Checks if the current token matches the expected type without consuming it.
     *
     * "Examine yourselves, to see whether you are in the faith." – 2 Corinthians 13:5
     *
     * @param type The token type to check against.
     * @return true if the token matches, false otherwise.
     */
    bool check(TokenType type);

    /**
     * @brief Advances the parser to the next token and returns the previous one.
     *
     * "Let us go on to maturity, not laying again the foundation..." – Hebrews 6:1
     *
     * @return Token The token that was just passed over.
     */
    Token advance();

    /**
     * @brief Determines if the parser has reached the end of the input tokens.
     *
     * "I am the Alpha and the Omega, the First and the Last, the Beginning and the End." – Revelation 22:13
     *
     * @return true if at the end, false otherwise.
     */
    bool isAtEnd();

    /**
     * @brief Returns the current token without advancing.
     *
     * "Watch and pray so that you will not fall into temptation." – Matthew 26:41
     *
     * @return Token The current token in the stream.
     */
    Token peek();

    /**
     * @brief Returns the most recently consumed token.
     *
     * "Remember the former things of old: for I am God, and there is none else." – Isaiah 46:9
     *
     * @return Token The last token consumed.
     */
    Token previous();

    /**
     * @brief Consumes the current token if it matches the expected type, otherwise throws an error
     *
     * "Let your ‘Yes’ be ‘Yes,’ and your ‘No,’ ‘No’." – Matthew 5:37
     *
     * @param type    The expected TokenType that the current token should match.
     * @param message The error message to display if the token does not match the expected type.
     * @return Token  The consumed token if the match was successful.
     * @throws std::runtime_error If the current token does not match the expected type.
     */
    Token consume(TokenType type, const std::string &message);
};
