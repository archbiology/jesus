#include "lexer.hpp"
#include <ostream> // For std::ostream
#include <string>
#include "../utils/string_utils.hpp"

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

    if (word == "not" || word == "não")
        return TokenType::NOT;

    if (word == "and" || word == "e")
        return TokenType::AND;

    if (word == "or" || word == "ou")
        return TokenType::OR;

    if (word == "vs" || word == "versus")
        return TokenType::VERSUS;

    if (word == "=")
        return TokenType::EQUAL;

    if (word == "==")
        return TokenType::EQUAL_EQUAL;

    if (word == "!=")
        return TokenType::NOT_EQUAL;

    if (word == ">") // greater than
        return TokenType::GREATER;

    if (word == ">=") // greater or equals to
        return TokenType::GREATER_EQUAL;

    if (word == "<") // less than
        return TokenType::LESS;

    if (word == "<=") // less or equal to
        return TokenType::LESS_EQUAL;

    if (word == "no")
        return TokenType::NO;

    if (word == "yes")
        return TokenType::YES;

    if (word == "male")
        return TokenType::MALE;

    if (word == "female")
        return TokenType::FEMALE;

    if (word == "lightday")
        return TokenType::LIGHDAY;

    if (word == "skyday")
        return TokenType::SKYDAY;

    if (word == "treeday")
        return TokenType::TREEDAY;

    if (word == "lampday")
        return TokenType::LAMPDAY;

    if (word == "fishday")
        return TokenType::FISHDAY;

    if (word == "walkday")
        return TokenType::WALKDAY;

    if (word == "shabbat")
        return TokenType::SHABBAT;

    if (word == "matches")
        return TokenType::MATCHES;

    if (word == "between")
        return TokenType::BETEWEEN;

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

    if (word == "ask")
        return TokenType::ASK;

    if (word == "say")
        return TokenType::SAY;

    if (word == "amen" || word == "amém")
        return TokenType::AMEN;

    if (word == "warn")
        return TokenType::WARN;

    if (word == "let")
        return TokenType::LET;

    if (word == "there")
        return TokenType::THERE;

    if (word == "be")
        return TokenType::BE;

    if (word == "haja")
        return TokenType::HAJA;

    if (isInteger(word))
        return TokenType::INT;

    if (isDouble(word))
        return TokenType::DOUBLE;

    if (word[0] == '"')
        return TokenType::STRING;

    if (word == "create" || word == "criar")
        return TokenType::CREATE;

    if (word == "if" || word == "se")
        return TokenType::IF;

    if (word == "otherwise" || word == "senão")
        return TokenType::OTHERWISE;

    if (word == "purpose" || word == "propósito")
        return TokenType::PURPOSE;

    return TokenType::IDENTIFIER;
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

    const std::string quote = "\""; // single ASCII quote
    const std::string space = " ";
    const std::string tab = "\t";
    const std::string newline = "\n";
    const std::string carriage = "\r";

    while (i < utf8_input.size())
    {
        c = utf8_input[i];
        if (c == space || c == tab || c == newline || c == carriage)
        {
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

        if (c == quote)
        {
            // Handle string literal
            size_t end = i + 1;
            std::string str;
            while (end < utf8_input.size() && utf8_input[end] != quote)
            {
                str += utf8_input[end];
                ++end;
            }

            if (end == utf8_input.size())
                throw std::runtime_error("Unterminated string literal");

            tokens.emplace_back(TokenType::STRING, "\"" + str + "\"", Value(str));
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

        if (c == "=" || c == "!" || c == "<" || c == ">")
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
