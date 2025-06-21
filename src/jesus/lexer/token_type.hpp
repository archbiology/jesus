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

    NOT,            // not
    AND,            // and
    OR,             // or
    VERSUS,         // versus or vs (it's called XOR, represented as "^" in other languages)
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    LESS,           // <
    LESS_EQUAL,     // <=
    FALSE,          // 17
    TRUE,           // 18
    INT,            // 19
    DOUBLE,         // 20
    STRING,         // 21
    IDENTIFIER,     // varname
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )

    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /

    SAY,            // "say" prints to stdout
    WARN,           // "warn" prints to stderr
    UPDATE,

    REPEAT,         // `repeat` 3 times
    TIMES,          // repeat 3 `times`

    END_OF_FILE
};
