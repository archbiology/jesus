#pragma once

/**
 * @brief Represents the type of a token recognized by the lexer.
 *
 * Each token type corresponds to a meaningful symbol or word
 * within the language's syntax.
 */
enum class TokenType
{
    Note,
    Todo,
    Warn,
    Explain,
    BeginNote,
    EndNote,
    Word,        // 6
    Unknown,

    NOT,         // 8
    AND,
    OR,
    EQUAL_EQUAL, // 11
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    FALSE,       // 17
    TRUE,        // 18
    INT,         // 19
    DOUBLE,      // 20
    STRING,      // 21
    IDENTIFIER,  // 22
    LEFT_PAREN,  // 23
    RIGHT_PAREN, // 24

    PLUS,

    SAY,
    WARN,
    UPDATE,

    END_OF_FILE
};
