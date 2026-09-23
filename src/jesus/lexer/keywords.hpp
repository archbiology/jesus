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
        {"saying", TokenType::SAYING},
        {"warning", TokenType::WARNING},
        {"yellow", TokenType::YELLOW},
        {"red", TokenType::RED},
        {"green", TokenType::GREEN},
        {"blue", TokenType::BLUE},
        {"bold", TokenType::BOLD},
        {"italic", TokenType::ITALIC},

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

        {"type", TokenType::TYPE},

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

        {"foreach", TokenType::FOREACH},
        {"in", TokenType::IN},

        {"otherwise", TokenType::OTHERWISE},
        {"senão", TokenType::OTHERWISE},

        {"purpose", TokenType::PURPOSE},
        {"propósito", TokenType::PURPOSE},

        {"private", TokenType::PRIVATE},
        {"protected", TokenType::PROTECTED},
        {"public", TokenType::PUBLIC},

        {"I", TokenType::MY},
        {"my", TokenType::MY},
        {"me", TokenType::MY},
        {"myself", TokenType::MY},
        {"itself", TokenType::ITSELF},

        {"__alpha__", TokenType::ALPHA},
        {"__omega__", TokenType::OMEGA},

        {"come", TokenType::COME},
        {"dots", TokenType::DOTS},

        {"ungodly", TokenType::UNGODLY},

        {"ast", TokenType::AST},
        {"bytecode", TokenType::BYTECODE},
        {"vm", TokenType::VM},
        {"memory", TokenType::MEMORY},

        {"giants", TokenType::GIANTS},
        {"confess", TokenType::CONFESS},
        {"bible", TokenType::BIBLE},

        {"on", TokenType::ON},
        {"serve", TokenType::SERVE},
        {"json", TokenType::JSON},

        {"enum", TokenType::ENUM},
    };

    inline bool isReservedWord(const std::string &word)
    {
        return RESERVED.find(word) != RESERVED.end();
    }

    /**
     * Generates an error message for an attempt to use a reserved word as a name.
     *
     * @param word The reserved word being used.
     * @param kind The kind of entity the user is trying to name:
     *          "class", "variable", "method", or "type".
     */
    inline std::string reservedWordMsg(const std::string &word, const std::string &kind)
    {
        return "'" + word + "' is a reserved word and cannot be used as a " + kind + " name.\nTry '" + word + "_" +
               kind + "' instead.";
    }

    inline TokenType recognize(const std::string &word)
    {

        auto it = RESERVED.find(word);
        if (it != RESERVED.end())
            return it->second;

        return TokenType::IDENTIFIER;
    }
}
