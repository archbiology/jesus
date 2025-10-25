#pragma once
#include <string>
#include <vector>
#include "token.hpp"

class Lexer
{
    bool isParsingMultilineComment = false;

public:
    /**
     * @brief Lexical analyzer function.
     *
     * @param input A raw string line of text to tokenize.
     * @return std::vector<Token> A vector of tokens extracted from the input string.
     */
    std::vector<Token> tokenize(const std::string &input);

    /**
     * @brief Output stream overload for TokenType enum.
     *
     * @param os The output stream.
     * @param type The TokenType to convert to a string representation.
     * @return std::ostream& Reference to the modified output stream.
     */
    friend std::ostream &operator<<(std::ostream &os, const TokenType &type);

    bool insideMultilineComment() { return isParsingMultilineComment; }
};
