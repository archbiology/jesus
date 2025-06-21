#include "lexer.hpp"
#include <ostream> // For std::ostream
#include <string>
#include <cctype>

/**
 * @brief Check if a string is a double
 *
 * "For the word of God is alive and active. Sharper than any double-edged sword, it penetrates even to dividing soul and spirit, joints and marrow; it judges the thoughts and attitudes of the heart."
 * — Hebrews 4:12
 *
 * @param word the word to be checked
 */
bool isDouble(const std::string &word)
{
    try
    {
        size_t pos;
        std::stod(word, &pos);

        // Make sure the whole string was converted
        return pos == word.length();
    }
    catch (...)
    {
        return false;
    }
}

/**
 * @brief Check if a string is an integer.
 *
 * @param word the workd to be checked
 */
bool isInteger(const std::string &word)
{
    try
    {
        size_t pos;
        std::stoi(word, &pos);

        // Make sure the whole string was converted
        return pos == word.length();
    }
    catch (...)
    {
        return false;
    }
}

/**
 * @brief Check if a character is a valid start of an identifier (e.g., variable name).
 *
 * @param c The characther to be checked
 * @return true if it is a letter or underline (_)
 * @return false otherwise
 */
bool isIdentifierStart(char c)
{
    return std::isalpha(c) || c == '_';
}

/**
 * @brief Check if a character is a valid part of an identifier.
 *
 * @param c The characther to be checked
 * @return true if it is a letter, a number, or underline.
 * @return false otherwise.
 */
bool isIdentifierChar(char c)
{
    return std::isalnum(c) || c == '_';
}

TokenType recognize_token_type(const std::string &word)
{
    if (word == "note:")
        return TokenType::Note;

    if (word == "todo:")
        return TokenType::Todo;

    if (word == "warn:")
        return TokenType::Warn;

    if (word == "explain:")
        return TokenType::Explain;

    if (word == "begin")
        return TokenType::BeginNote;

    if (word == "end")
        return TokenType::EndNote;

    if (word == "not")
        return TokenType::NOT;

    if (word == "and")
        return TokenType::AND;

    if (word == "or")
        return TokenType::OR;

    if (word == "vs" || word == "versus")
        return TokenType::VERSUS;

    if (word == "==")
        return TokenType::EQUAL_EQUAL;

    if (word == "!=")
        return TokenType::NOT_EQUAL;

    if (word == ">")
        return TokenType::GREATER;

    if (word == ">=")
        return TokenType::GREATER_EQUAL;

    if (word == "<")
        return TokenType::LESS;

    if (word == "<=")
        return TokenType::LESS_EQUAL;

    if (word == "false")
        return TokenType::FALSE;

    if (word == "true")
        return TokenType::TRUE;

    if (word == "(")
        return TokenType::LEFT_PAREN;

    if (word == ")")
        return TokenType::RIGHT_PAREN;

    if (word == "*")
        return TokenType::STAR;

    if (word == "/")
        return TokenType::SLASH;

    if (word == "+")
        return TokenType::PLUS;

    if (word == "-")
        return TokenType::MINUS;

    if (word == "say")
        return TokenType::SAY;

    if (word == "warn")
        return TokenType::WARN;

    if (isInteger(word))
        return TokenType::INT;

    if (isDouble(word))
        return TokenType::DOUBLE;

    if (word[0] == '"')
        return TokenType::Word;

    if (word == "repeat")
        return TokenType::REPEAT;

    if (word == "times")
        return TokenType::TIMES;

    if (word == "amen")
        return TokenType::AMEN;

    if (word == "\n")
        return TokenType::NEW_LINE;

    return TokenType::IDENTIFIER;
}

/**
 * @brief Tokenize the input string character by character, allowing expression grouping without spaces.
 *
 * @param input
 * @return std::vector<Token>
 */
std::vector<Token> lex(const std::string &input)
{
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < input.size())
    {
        char c = input[i];

        // / Special handling for actual newline character
        if (c == '\n') {
            tokens.emplace_back(TokenType::NEW_LINE, "\n", Value("\n"));
            i++;
            continue;
        }

        // Skip whitespace
        if (std::isspace(c))
        {
            i++;
            continue;
        }

        // Handle punctuation/single-char tokens
        if (c == '(')
        {
            tokens.emplace_back(TokenType::LEFT_PAREN, "(", Value("("));
            i++;
            continue;
        }

        if (c == ')')
        {
            tokens.emplace_back(TokenType::RIGHT_PAREN, ")", Value(")"));
            i++;
            continue;
        }

        if (c == '+')
        {
            tokens.emplace_back(TokenType::PLUS, "+", Value("+"));
            i++;
            continue;
        }

        if (c == '-')
        {
            tokens.emplace_back(TokenType::MINUS, "-", Value("-"));
            i++;
            continue;
        }

        if (c == '*')
        {
            tokens.emplace_back(TokenType::STAR, "*", Value("*"));
            i++;
            continue;
        }

        if (c == '/')
        {
            tokens.emplace_back(TokenType::SLASH, "/", Value("/"));
            i++;
            continue;
        }

        if (c == '"')
        {
            // Handle string literal
            size_t start = i + 1;
            size_t end = input.find('"', start);

            if (end == std::string::npos)
                throw std::runtime_error("Unterminated string literal");

            std::string str = input.substr(start, end - start);
            tokens.emplace_back(TokenType::Word, "\"" + str + "\"", Value(str));
            i = end + 1;
            continue;
        }

        if (std::isdigit(c))
        {
            // Number (int or double)
            size_t start = i;
            while (i < input.size() && (std::isdigit(input[i]) || input[i] == '.'))
                i++;

            std::string number = input.substr(start, i - start);
            if (isInteger(number))
                tokens.emplace_back(TokenType::INT, number, Value(std::stoi(number)));

            else if (isDouble(number))
                tokens.emplace_back(TokenType::DOUBLE, number, Value(std::stod(number)));

            else
                throw std::runtime_error("Invalid number format: " + number);

            continue;
        }

        if (isIdentifierStart(c))
        {
            // Keyword or identifier
            size_t start = i;
            while (i < input.size() && isIdentifierChar(input[i]))
                i++;

            std::string word = input.substr(start, i - start);
            tokens.emplace_back(recognize_token_type(word), word, Value(word));
            continue;
        }

        if (c == '=' || c == '!' || c == '<' || c == '>')
        {
            // Comparison operators
            std::string op;
            op += c;
            if (i + 1 < input.size() && input[i + 1] == '=')
                op += input[++i];

            tokens.emplace_back(recognize_token_type(op), op, Value(op));
            i++;
            continue;
        }

        if (c == ':')
        {
            tokens.emplace_back(TokenType::COLON, ":", Value(":"));
            i++;
            continue;
        }

        // Unexpected character
        throw std::runtime_error("Unexpected character: " + std::string(1, c));
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "EOF", Value().formless());
    return tokens;
}

std::ostream &operator<<(std::ostream &os, const TokenType &type)
{
    switch (type)
    {
    case TokenType::Note:
        return os << "Note";

    case TokenType::Todo:
        return os << "Todo";

    case TokenType::Warn:
        return os << "Warn";

    case TokenType::Explain:
        return os << "Explain";

    case TokenType::BeginNote:
        return os << "BeginNote";

    case TokenType::EndNote:
        return os << "EndNote";

    case TokenType::Word:
        return os << "Word";

    case TokenType::Unknown:
        return os << "Unknown";

    default:
        return os << "Invalid";
    }
}
