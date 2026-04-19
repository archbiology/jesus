#pragma once
#include "stmt.hpp"

class Expr;

/**
 * @brief Start an HttpServer
 *
 * Usage:
 *  serve
 *
 *  serve on 80
 *
 *  create number port = 8080
 *  serve on port
 */
class ServeStmt : public Stmt
{
public:
    std::unique_ptr<Expr> portExpr; // default: 7000

    ServeStmt(std::unique_ptr<Expr> portExpr)
        : portExpr(std::move(portExpr)) {}

    void accept(StmtVisitor &visitor) const override;
};
