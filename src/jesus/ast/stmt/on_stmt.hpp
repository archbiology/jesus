#pragma once

#include "stmt.hpp"
#include <string>
#include <vector>
#include <memory>

/**
 * @brief Represents an event handler declaration in the Jesus language.
 *
 * Example:
 * @code
 * on http '/status':
 *     return 1
 * amen
 * @endcode
 *
 * In this example, the program declares that whenever an HTTP request
 * is received at the path `/status`, the http response is '1'.
 *
 * This abstraction is part of the event-driven model of the language,
 * where all interactions (HTTP, TCP, WebSocket, etc.) are treated as
 * events flowing through the system ("Mind"), and the program reacts
 * accordingly.
 *
 * "My sheep hear my voice, and I know them, and they follow me."
 * — John 10:27
 */
class OnStmt : public Stmt
{
public:
    std::string protocol;                    // "http"
    std::string path;                        // "/status"
    std::vector<std::shared_ptr<Stmt>> body; // return {status: true}

    OnStmt(std::string protocol,
           std::string path,
           std::vector<std::shared_ptr<Stmt>> body)
        : protocol(std::move(protocol)),
          path(std::move(path)),
          body(std::move(body)) {}

    void accept(StmtVisitor &visitor) const override;
};
