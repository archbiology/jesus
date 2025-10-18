#pragma once

#include <string>
#include "token_type.hpp"
#include "../spirit/value.hpp"

/**
 * @brief Represents a lexical token with a type and string value.
 *
 * Tokens are the result of breaking down a line of input text.
 * Each token has a `TokenType` and an associated `value`.
 */
class Token
{
public:
    TokenType type;

    /**
     * @brief The exact sequence of characters as it appeared in the source code.
     *
     * For example, in `print("Hello")`, the lexeme for the string token is `"Hello"`.
     *
     * "For truly I say to you, until heaven and earth pass away, not an iota, not a dot,
     * will pass from the Law until all is accomplished." – Matthew 5:18
     */
    std::string lexeme;

    /**
     * @brief The interpreted value of the token, if applicable (e.g., a number, string, boolean).
     *
     * This holds the actual runtime value the lexeme represents.
     *  lexeme  -> "123"
     *  literal -> 123
     *
     * "The unfolding of Your words gives light; it gives understanding to the simple." – Psalm 119:130
     */
    Value literal;

    Token(TokenType type, std::string _lexeme, Value _literal)
        : type(type), lexeme(std::move(_lexeme)), literal(std::move(_literal))
    {

        switch (type)
        {
        case TokenType::INT:
            literal = Value(std::stoi(lexeme));
            break;

        case TokenType::DOUBLE:
            literal = Value(std::stod(lexeme));
            break;
        }
    }

    std::string toString()
    {
        switch (type)
        {
        case TokenType::A:
            return "A";
        case TokenType::LET:
            return "LET";
        case TokenType::THERE:
            return "THERE";
        case TokenType::BE:
            return "BE";
        case TokenType::HAJA:
            return "HAJA";
        case TokenType::FROM:
            return "FROM";
        case TokenType::CREATE:
            return "CREATE";
        case TokenType::PURPOSE:
            return "PURPOSE";
        case TokenType::RETURN:
            return "RETURN";
        case TokenType::Note:
            return "NOTE";
        case TokenType::Todo:
            return "TODO";
        case TokenType::Explain:
            return "EXPLAIN";
        case TokenType::BeginNote:
            return "BEGIN_NOTE";
        case TokenType::EQUAL:
            return "EQUAL"; // variable assignment
        case TokenType::IS:
            return "IS"; // equality  ('==' in other languages)
        case TokenType::NOT:
            return "NOT";
        case TokenType::AND:
            return "AND";
        case TokenType::OR:
            return "OR";
        case TokenType::VERSUS:
            return "VERSUS";
        case TokenType::NOT_EQUAL:
            return "NOT_EQUAL";
        case TokenType::GREATER:
            return "GREATER";
        case TokenType::GREATER_EQUAL:
            return "GREATER_EQUAL";
        case TokenType::LESS:
            return "LESS";
        case TokenType::LESS_EQUAL:
            return "LESS_EQUAL";
        case TokenType::YES:
            return "YES";
        case TokenType::NO:
            return "NO";
        case TokenType::LEFT_PAREN:
            return "LEFT_PAREN";
        case TokenType::RIGHT_PAREN:
            return "RIGHT_PAREN";

        case TokenType::STAR:
            return "STAR";

        case TokenType::SLASH:
            return "SLASH";
        case TokenType::MOD:
            return "MOD";
        case TokenType::ODD:
            return "ODD";
        case TokenType::EVEN:
            return "EVEN";

        case TokenType::PLUS:
            return "PLUS";

        case TokenType::MINUS:
            return "MINUS";
        case TokenType::COLON:
            return "COLON";
        case TokenType::SEMICOLON:
            return "SEMICOLON";
        case TokenType::COMMA:
            return "COMMA";
        case TokenType::ARROW:
            return "ARROW";

        case TokenType::INT:
            return "INT(" + lexeme + ")";
        case TokenType::DOUBLE:
            return "DOUBLE(" + lexeme + ")";
        case TokenType::IDENTIFIER:
            return "IDENTIFIER(" + lexeme + ")";

        case TokenType::RAW_STRING:
            return "RAW_STRING(" + lexeme + ")";

        case TokenType::FORMATTED_STRING:
            return "FORMATTED_STRING(" + lexeme + ")";

        case TokenType::SAY:
            return "SAY";
        case TokenType::ASK:
            return "ASK";
        case TokenType::WARN:
            return "WARN";
        case TokenType::UPDATE:
            return "UPDATE";
        case TokenType::Word:
            return "WORD(" + lexeme + ")";

        case TokenType::IF:
            return "IF";

        case TokenType::OTHERWISE:
            return "OTHERWISE";

        case TokenType::REPEAT:
            return "REPEAT";

        case TokenType::TIMES:
            return "TIMES";

        case TokenType::WHILE:
            return "WHILE";

        case TokenType::FOREVER:
            return "FOREVER";

        case TokenType::SKIP:
            return "SKIP";

        case TokenType::BREAK:
            return "BREAK";

        case TokenType::AMEN:
            return "AMEN";

        case TokenType::NEWLINE:
            return "NEWLINE";

        case TokenType::END_OF_FILE:
            return "EOF";

        default:
            return "🔴 Unknown.val(" + lexeme + ")";
        }
    }
};
