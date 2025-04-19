#pragma once
#include <string>
#include <vector>

/**
 * @brief Represents the type of a token recognized by the lexer.
 *
 * Each token type corresponds to a meaningful symbol or word
 * within the custom language's syntax.
 */
enum class TokenType
{
    Note,
    Todo,
    Warn,
    Explain,
    BeginNote,
    EndNote,
    Word,
    Unknown
};

/**
 * @brief Represents a lexical token with a type and string value.
 *
 * Tokens are the result of breaking down a line of input text.
 * Each token has a `TokenType` and an associated `value`.
 */
struct Token
{
    TokenType type;
    std::string value;
};

/**
 * @brief Lexical analyzer function.
 *
 * @param input A raw string line of text to tokenize.
 * @return std::vector<Token> A vector of tokens extracted from the input string.
 */
std::vector<Token> lex(const std::string &input);

/**
 * @brief Output stream overload for TokenType enum.
 *
 * @param os The output stream.
 * @param type The TokenType to convert to a string representation.
 * @return std::ostream& Reference to the modified output stream.
 */
std::ostream &operator<<(std::ostream &os, const TokenType &type);
