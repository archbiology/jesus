#pragma once
#include "stmt.hpp"
#include "../expr/expr.hpp"
#include <memory>

enum class OutputType
{
    SAY,
    WARN,
};

/// Statement for printing output: say, praise, rebuke
/**
 * @brief The OutputStmt class represents the `say` and `warn` commands.
 *
 * It is responsible for displaying text to the screen.
 *
 * "He reveals deep and hidden things; He knows what is in the darkness, and light dwells with Him."
 * — Daniel 2:22
 */
class OutputStmt : public Stmt
{
public:
    OutputType type;
    std::unique_ptr<Expr> message;

    /**
     * @brief Construct a new Output Stmt object for printing text to screen.
     *
     * "Call to me and I will answer you and tell you great and unsearchable things you do not know."
     * — Jeremiah 33:3
     */
    OutputStmt(OutputType type, std::unique_ptr<Expr> message)
        : type(type), message(std::move(message)) {}

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override
    {
        std::string str = "OutputStmt(";

        str += (type == OutputType::SAY ? "SAY:" : "WARN: ");

        if (message)
            str += message->toString();

        str += ")";

        return str;
    }
};
