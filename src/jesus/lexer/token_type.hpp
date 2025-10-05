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
    Explain,
    BeginNote,
    EndNote,
    Word,        // 6
    Unknown,

    A,              // Reserved for type checking: if varname is 'a' ClassName:...
    IS,             // 'is': ==
    NOT,            // not: !
    AND,            // and
    OR,             // or
    VERSUS,         // versus or vs (it's called XOR, represented as "^" in other languages)
    EQUAL,          // '=' for variable assignments. For 'equality' comparison, we use 'is' and 'is not'
    NOT_EQUAL,      // !=
    GREATER,        // >
    GREATER_EQUAL,  // >=
    LESS,           // <
    LESS_EQUAL,     // <=
    YES,            // 17
    NO,             // 18
    INT,            // 19
    DOUBLE,         // 20
    RAW_STRING,         // With single quotes, like in bash: 'hello world'
    FORMATTED_STRING,   // Double quotes: "Hi, {user}"
    IDENTIFIER,     // varname, attribute
    LEFT_PAREN,     // (
    RIGHT_PAREN,    // )

    PLUS,           // +
    MINUS,          // -
    STAR,           // *
    SLASH,          // /
    MOD,            // mod ('%' in other languages)
    ODD,            //
    EVEN,           //
    COLON,          // ':' (Begining of a block)
    SEMICOLON,      // ';' To separate method args by type: int x, y, z; text name, surname
    COMMA,          // ',' To separate var names: int x, y, z
    ARROW,          // '->' To define method return types. purpose praise() -> GloryToGod: amen

    LET,            // let there be Person: amen (class in other langs)
    THERE,          // let there be Person: amen (class in other langs)
    BE,             // let there be Person: amen (class in other langs)
    HAJA,           // haja Luz: amén (portuguese way of creating class)
    PURPOSE,        // create method: purpose praise(): amen
    RETURN,         // Function returns: return 144000
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
    WHILE,          // repeat while count < 100: ... amen
    TIMES,          // repeat 3 `times`
    FOREVER,        // repeat forever: ... amen
    SKIP,           // 'continue' in other languages.
    BREAK,          // loop break

    IF,
    OTHERWISE,

    AMEN,           // amen (end of blocks in Jesus lang; '}' in other langs)
    NEWLINE,        // \n
    END_OF_FILE
};
