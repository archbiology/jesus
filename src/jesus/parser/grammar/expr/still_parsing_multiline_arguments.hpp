#pragma once

#include <exception>

/**
 * @brief Signals that a method call or object instantiation has
 *        multiline parameters and needs more input.
 *
 * This allows method calls and object instantiations to have their
 * parameters declared on multiple lines, for example:
 *
 *     create person = Person(
 *         'Jesus',
 *         33
 *     )
 *
 * The REPL (Disciple) catches this signal and keeps reading more lines
 * instead of reporting a parse error.
 *
 * It is not a real error. It means the parser reached the end of the
 * available input while waiting for the remaining parameters or the
 * closing ')' of the method call or object instantiation in REPL mode.
 *
 * This is the expression equivalent of IncompleteBlockStmt/inProgress(),
 * which is used when a block needs more lines.
 */
class StillParsingMultilineArgumentsSignal : public std::exception
{
  public:
    const char *what() const noexcept override { return "Still parsing multiline arguments"; }
};
