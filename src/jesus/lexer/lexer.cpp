#include "lexer.hpp"
#include <ostream> // For std::ostream
#include <string>
#include "../utils/string_utils.hpp"
#include "keywords.hpp"

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
bool isIdentifierStart(const std::string &c)
{
    return utils::is_alpha_utf8(c) || c == "_";
}

/**
 * @brief Check if a character is a valid part of an identifier.
 *
 * @param c The characther to be checked
 * @return true if it is a letter, a number, or underline.
 * @return false otherwise.
 */
bool isIdentifierChar(const std::string &c)
{
    return utils::is_alpha_utf8(c) || utils::is_digit_utf8(c) || c == "_";
}

TokenType recognize_token_type(const std::string &word)
{

    if (isInteger(word))
        return TokenType::INT;

    if (isDouble(word))
        return TokenType::DOUBLE;

    return Keywords::recognize(word);
}

/**
 * @brief Tokenize the input string character by character, allowing expression grouping without spaces.
 *
 * @param input
 * @return std::vector<Token>
 */
std::vector<Token> lex(const std::string &raw_input)
{
    std::vector<Token> tokens;
    size_t i = 0;
    std::string c;

    auto utf8_input = utils::to_utf8(raw_input);

    const std::string single_quote = "'";  // single ASCII quote
    const std::string double_quote = "\""; // double quote
    const std::string space = " ";
    const std::string tab = "\t";
    const std::string newline = "\n";
    const std::string carriage = "\r";

    while (i < utf8_input.size())
    {
        c = utf8_input[i];
        if (c == space || c == tab)
        {
            i++;
            continue;
        }

        if (c == newline || c == carriage)
        {
            tokens.emplace_back(TokenType::NEWLINE, "\n", Value("\n"));
            i++;
            continue;
        }

        // Handle punctuation/single-char tokens
        if (c == "(")
        {
            tokens.emplace_back(TokenType::LEFT_PAREN, "(", Value("("));
            i++;
            continue;
        }

        if (c == ")")
        {
            tokens.emplace_back(TokenType::RIGHT_PAREN, ")", Value(")"));
            i++;
            continue;
        }

        if (c == "+")
        {
            tokens.emplace_back(TokenType::PLUS, "+", Value("+"));
            i++;
            continue;
        }

        if (c == "-")
        {
            // Lookahead for "->"
            if (i + 1 < utf8_input.size() && utf8_input[i + 1] == ">")
            {
                tokens.emplace_back(TokenType::ARROW, "->", Value("->"));
                i += 2; // consume both '-' and '>'
                continue;
            }

            tokens.emplace_back(TokenType::MINUS, "-", Value("-"));
            i++;
            continue;
        }

        if (c == "*")
        {
            tokens.emplace_back(TokenType::STAR, "*", Value("*"));
            i++;
            continue;
        }

        if (c == "/")
        {
            tokens.emplace_back(TokenType::SLASH, "/", Value("/"));
            i++;
            continue;
        }

        if (c == ":")
        {
            tokens.emplace_back(TokenType::COLON, ":", Value(":"));
            i++;
            continue;
        }

        if (c == ";")
        {
            tokens.emplace_back(TokenType::SEMICOLON, ";", Value(";"));
            i++;
            continue;
        }

        if (c == ",")
        {
            tokens.emplace_back(TokenType::COMMA, ",", Value(","));
            i++;
            continue;
        }

        if (c == "=")
        {
            tokens.emplace_back(TokenType::EQUAL, "=", Value("="));
            i++;
            continue;
        }

        if (c == single_quote || c == double_quote)
        {
            const std::string quote_char = c;
            size_t end = i + 1;
            std::string str;

            while (end < utf8_input.size() && utf8_input[end] != quote_char)
            {
                str += utf8_input[end];
                ++end;
            }

            if (end == utf8_input.size())
                throw std::runtime_error("Unterminated string literal");

            if (quote_char == single_quote)
            {
                tokens.emplace_back(TokenType::RAW_STRING, "'" + str + "'", Value(str));
            }
            else // double quote
            {
                tokens.emplace_back(TokenType::FORMATTED_STRING, "\"" + str + "\"", Value(str));
            }

            i = end + 1;
            continue;
        }

        if (utils::is_digit_utf8(c))
        {
            // Number (int or double)
            std::string number_str;

            // Collect all consecutive digits and decimal points
            while (i < utf8_input.size())
            {
                const std::string &d = utf8_input[i];
                if (utils::is_digit_utf8(d) || d == ".")
                {
                    number_str += d; // rebuild number as string
                    ++i;
                }
                else
                {
                    break;
                }
            }

            // Determine type and create token
            if (isInteger(number_str))
                tokens.emplace_back(TokenType::INT, number_str, Value(std::stoi(number_str)));
            else if (isDouble(number_str))
                tokens.emplace_back(TokenType::DOUBLE, number_str, Value(std::stod(number_str)));
            else
                throw std::runtime_error("Invalid number format: " + number_str);

            continue;
        }

        if (isIdentifierStart(c))
        {
            // Keyword or identifier
            size_t start = i;
            std::string word;

            while (i < utf8_input.size() && isIdentifierChar(utf8_input[i]))
            {
                word += utf8_input[i];
                i++;
            }

            tokens.emplace_back(recognize_token_type(word), word, Value(word));
            continue;
        }

        if (c == "<" || c == ">")
        {
            // Comparison operators
            std::string op;
            op += c;
            if (i + 1 < utf8_input.size() && utf8_input[i + 1] == "=")
                op += utf8_input[++i];

            tokens.emplace_back(recognize_token_type(op), op, Value(op));
            i++;
            continue;
        }

        if (c == "!")
        {
            std::string msg = "Use 'not' for negation instead of '!'.";

            if (i + 1 < utf8_input.size() && utf8_input[i + 1] == "=")
                msg = "Use 'is not' for inequality instead of '!='.";

            throw std::runtime_error(msg);
        }

        if (c == "%")
            throw std::runtime_error("Use 'mod' for modulo instead of '%'.");

        // Unexpected character
        throw std::runtime_error("Unexpected character: '" + c + "'");
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

    case TokenType::WARN:
        return os << "WARN";

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
