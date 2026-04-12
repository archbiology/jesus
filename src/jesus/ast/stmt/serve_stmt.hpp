#pragma once
#include "stmt.hpp"

/**
 * @brief Start an HttpServer
 */
class ServeStmt : public Stmt
{
public:
    void accept(StmtVisitor &visitor) const override;
};
