#pragma once

#include <string>
#include "interpreter/interpreter.hpp"
#include "../lexer/lexer.hpp"

REGISTER_FOR_UML(
    Faith,
    .usesList({"Interpreter"})
        .methodsList({"execute"}));

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
    /**
     * @brief @param retainClasses keeps class definitions while optimizing.
     *
     * It is on only while executing an imported module: the importing module
     * holds methods that point back into the imported module's AST, so that
     * module must keep its classes alive. The main module leaves it off, so
     * its unused classes are eliminated as usual.
     */
    explicit Faith(bool useVm, bool keepAst, bool retainClasses = false)
        : useVm(useVm), keepAst(keepAst), retainClasses(retainClasses) {};

    /**
     * @brief Loads a file and execute it.
     *
     * "Faith without works is dead.” (James 2:26)
     */
    int execute(std::string filename);

private:
    bool useVm;
    bool keepAst;
    bool retainClasses;
    /**
     * @brief Interpret and execute the source of a file
     *
     * “By faith we understand…” (Hebrews 11:3)
     */
    void interpret(Interpreter &jesus, const std::string &source, const std::string &moduleName); // executes raw text
};
