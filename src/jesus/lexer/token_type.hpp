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
    EQUAL,          // =
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    LESS,           // <
    LESS_EQUAL,     // <=
    YES,            // 17
    NO,             // 18
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
    COLON,          // ':' (Begining of a block)

    LET,            // let there be Person: amen (class in other langs)
    THERE,          // let there be Person: amen (class in other langs)
    BE,             // let there be Person: amen (class in other langs)
    CREATE,         // create days = 7
    ASK,            // create int age = ask "What is your age?"
    SAY,            // "say" prints to stdout
    WARN,           // "warn" prints to stderr
    UPDATE,
    MATCHES,        // create text Confirm matches "(Y|y)es"
    BETEWEEN,       // create real Percentage between 0 and 100

    MALE,           // male
    FEMALE,         // female

    // Week days
    LIGHDAY,        // lighday = Sunday = 1
    SKYDAY,         // skyday = Monday = 2
    TREEDAY,        // treeday = Tuesday = 3
    LAMPDAY,        // lampday = Wednesday = 4
    FISHDAY,        // fishday = Thursday = 5
    WALKDAY,        // walkday = man and animal = Friday = 6
    SHABBAT,        // shabbat = saturday = 7

    REPEAT,         // `repeat` 3 times
    TIMES,          // repeat 3 `times`

    IF,
    OTHERWISE,

    AMEN,           // amen (end of blocks in Jesus lang; '}' in other langs)
    END_OF_FILE
};
