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
    Word,
    Unknown,

    NOT,
    AND,
    OR,
    EQUAL_EQUAL,
    NOT_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    FALSE,
    TRUE,
    NUMBER,
    STRING,
    IDENTIFIER,
    LEFT_PAREN,
    RIGHT_PAREN,

    PLUS,

    END_OF_FILE
};
