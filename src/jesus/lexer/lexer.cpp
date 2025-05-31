#include "lexer.hpp"
#include <sstream>
#include <ostream> // For std::ostream
#include <string>

bool isDouble(const std::string& word) {
    try {
        size_t pos;
        std::stod(word, &pos);

        // Make sure the whole string was converted
        return pos == word.length();
    } catch (...) {
        return false;
    }
}

bool isInteger(const std::string& word) {
    try {
        size_t pos;
        std::stoi(word, &pos);

        // Make sure the whole string was converted
        return pos == word.length();
    } catch (...) {
        return false;
    }
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

    if (word == "+")
        return TokenType::PLUS;

    if (isInteger(word))
        return TokenType::INT;

    if (isDouble(word))
        return TokenType::DOUBLE;

    if (word[0] == '"')
        return TokenType::Word;

    return TokenType::IDENTIFIER;
}

std::vector<Token> lex(const std::string &input)
{
    std::vector<Token> tokens;
    std::istringstream stream(input);
    std::string word;

    while (stream >> word)
    {
        tokens.push_back(Token(recognize_token_type(word), word, Value(word)));
    }

    tokens.push_back(Token(TokenType::END_OF_FILE, "EOF", Value().formless()));

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
