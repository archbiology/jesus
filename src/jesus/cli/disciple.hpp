#pragma once

#include <string>
#include "../interpreter/interpreter.hpp"
#include "../lexer/lexer.hpp"

REGISTER_FOR_UML(
    Disciple,
    .usesList({"Interpreter"})
        .methodsList({"repl", "walk"}));

/**
 * @brief REPL -> Read, Evaluate, Print, Loop
 *
 * Read: The system reads the command you have entered.
 * Evaluate: The system executes the command you entered.
 * Print: The system prints the result of the command. If there was an error, it prints the error message instead.
 * Loop: The system returns to the "Read" step, displaying a prompt, (Jesus), to wait for the next command.
 *
 * "until we all reach unity in the faith and in the knowledge of the Son of God and become mature,
 * attaining to the whole measure of the fullness of Christ." — Ephesians 4:13
 */
class Disciple
{
public:
    Disciple(Interpreter &interpreter);
    void walk();

    /**
     * @brief REPL -> Read, Evaluate, Print, Loop
     */
    void repl() { walk(); };

private:
    Interpreter &jesus;
    void processBuffer(std::string &buffer);
};
