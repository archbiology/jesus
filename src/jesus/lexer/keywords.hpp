#pragma once
#include <unordered_map>
#include <string>
#include "token_type.hpp"

/**
 * @brief Centralized registry of all reserved keywords in Jesus language.
 *
 * "Every good and perfect gift is from above, coming down from the Father of the heavenly lights,
 * who does not change like shifting shadows." — James 1:17
 */
namespace Keywords
{
    inline const std::unordered_map<std::string, TokenType> RESERVED = {

        {"explain", TokenType::Explain},

        {"a", TokenType::A},
        {"an", TokenType::A},
        {"um", TokenType::A},
        {"uma", TokenType::A},

        {"is", TokenType::IS},
        {"é", TokenType::IS},

        {"not", TokenType::NOT},
        {"não", TokenType::NOT},

        {"and", TokenType::AND},
        {"e", TokenType::AND},

        {"or", TokenType::OR},
        {"ou", TokenType::OR},
        {"to", TokenType::TO},

        {"vs", TokenType::VERSUS},
        {"versus", TokenType::VERSUS},

        {"=", TokenType::EQUAL},
        {">", TokenType::GREATER},
        {">=", TokenType::GREATER_EQUAL},
        {"<", TokenType::LESS},
        {"<=", TokenType::LESS_EQUAL},

        {"no", TokenType::NO},

        {"yes", TokenType::YES},

        {"male", TokenType::MALE},
        {"female", TokenType::FEMALE},

        {"lightday", TokenType::LIGHDAY},
        {"skyday", TokenType::SKYDAY},
        {"treeday", TokenType::TREEDAY},
        {"lampday", TokenType::LAMPDAY},
        {"fishday", TokenType::FISHDAY},
        {"walkday", TokenType::WALKDAY},
        {"shabbat", TokenType::SHABBAT},
        {"matches", TokenType::MATCHES},

        {"(", TokenType::LEFT_PAREN},
        {")", TokenType::RIGHT_PAREN},
        {"*", TokenType::STAR},
        {"/", TokenType::SLASH},
        {"+", TokenType::PLUS},
        {"-", TokenType::MINUS},

        {"ask", TokenType::ASK},
        {"say", TokenType::SAY},
        {"warn", TokenType::WARN},

        {"amen", TokenType::AMEN},
        {"amém", TokenType::AMEN},

        {"let", TokenType::LET},
        {"there", TokenType::THERE},
        {"be", TokenType::BE},
        {"haja", TokenType::HAJA},

        {"from", TokenType::FROM},

        {"mod", TokenType::MOD},
        {"odd", TokenType::ODD},
        {"ímpar", TokenType::ODD},
        {"even", TokenType::EVEN},
        {"par", TokenType::EVEN},

        {"skip", TokenType::SKIP},
        {"pular", TokenType::SKIP},

        {"break", TokenType::BREAK},

        {"create", TokenType::CREATE},
        {"criar", TokenType::CREATE},

        {"if", TokenType::IF},
        {"se", TokenType::IF},

        {"try", TokenType::TRY},
        {"resist", TokenType::RESIST},
        {"repent", TokenType::REPENT},
        {"as", TokenType::AS},
        {"always", TokenType::ALWAYS},

        {"return", TokenType::RETURN},
        {"retornar", TokenType::RETURN},

        {"repeat", TokenType::REPEAT},
        {"repetir", TokenType::REPEAT},

        {"times", TokenType::TIMES},
        {"vezes", TokenType::TIMES},

        {"while", TokenType::WHILE},
        {"enquanto", TokenType::WHILE},

        {"forever", TokenType::FOREVER},
        {"sempre", TokenType::FOREVER},

        {"otherwise", TokenType::OTHERWISE},
        {"senão", TokenType::OTHERWISE},

        {"purpose", TokenType::PURPOSE},
        {"propósito", TokenType::PURPOSE},

        {"come", TokenType::COME},
        {"dots", TokenType::DOTS},
    };

    inline bool isReserved(const std::string &word)
    {
        return RESERVED.find(word) != RESERVED.end();
    }

    inline TokenType recognize(const std::string &word)
    {

        auto it = RESERVED.find(word);
        if (it != RESERVED.end())
            return it->second;

        return TokenType::IDENTIFIER;
    }
}
