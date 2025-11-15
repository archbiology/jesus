#pragma once

#include <string>
#include "interpreter/interpreter.hpp"
#include "../lexer/lexer.hpp"

/**
 * @brief The executor of what is written in a file.jesus
 *
 * “Everything is possible for the one who believes.” (Mark 9:23)
 * “By faith we understand…” (Hebrews 11:3)
 * “Faith without works is dead.” (James 2:26)
 *
 * "Then war broke out in heaven. Michael and his angels fought against the dragon, and the dragon and his angels fought back.
 * But he was not strong enough, and they lost their place in heaven." — Revelation 12:7-8
 *
 * Usage:
 *  Faith michael(jesus);
 *  michael.execute(whatIsWritten);
 */
class Faith
{
public:
    explicit Faith(Interpreter &jesus);

    /**
     * @brief Loads a file and execute it.
     *
     * "Faith without works is dead.” (James 2:26)
     */
    int execute(const std::string &filename);

private:
    /**
     * @brief Interpret and execute the source of a file
     *
     * “By faith we understand…” (Hebrews 11:3)
     */
    void interpret(const std::string &source); // executes raw text

    Interpreter &jesus;
};
