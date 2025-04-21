#include "lexer.hpp"
#include <sstream>
#include <ostream> // For std::ostream

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

    return TokenType::Word;
}

std::vector<Token> lex(const std::string &input)
{
    std::vector<Token> tokens;
    std::istringstream stream(input);
    std::string word;

    while (stream >> word)
    {
        tokens.push_back(Token(recognize_token_type(word), word, word));
    }

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
