#pragma once

#include <vector>
#include <memory>
#include <unordered_map>
#include <string>

#include "ast/stmt/stmt.hpp"
#include "ast/expr/expr.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/expr/method_call_expr.hpp"

/**
 * @brief Replaces method call expressions with the inlined body of simple methods.
 *
 * MethodInliner identifies method calls whose bodies consist of simple return
 * statements and substitutes the method call directly with the method's body,
 * substituting parameter references with the call arguments.
 *
 * Example:
 *
 *     purpose add(number a, b):
 *         return a + b
 *     amen
 *
 *     say math.add(10, 20)
 *
 * becomes:
 *
 *     say 10 + 20
 *
 * "The Word became flesh and made his dwelling among us..."
 * — John 1:14
 */
class MethodInliner
{
  public:
    /**
     * @brief Executes method inlining on an AST.
     */
    void run(std::vector<std::unique_ptr<Stmt>> &program);

  private:
    void collectMethodsFromStmt(
        const Stmt &statement, std::unordered_map<std::string, const CreateMethodStmt *> &knownMethods);

    void optimizeBlock(
        std::vector<std::unique_ptr<Stmt>> &stmts,
        const std::unordered_map<std::string, const CreateMethodStmt *> &knownMethods);

    void optimizeBlock(
        std::vector<std::shared_ptr<Stmt>> &stmts,
        const std::unordered_map<std::string, const CreateMethodStmt *> &knownMethods);

    void optimizeStatement(
        Stmt &statement, const std::unordered_map<std::string, const CreateMethodStmt *> &knownMethods);

    std::unique_ptr<Expr> optimizeExpression(
        std::unique_ptr<Expr> expression,
        const std::unordered_map<std::string, const CreateMethodStmt *> &knownMethods);

    std::unique_ptr<Expr> inlineMethodCall(
        std::unique_ptr<MethodCallExpr> methodCall,
        const std::unordered_map<std::string, const CreateMethodStmt *> &knownMethods);

    /**
     * @brief Clone an expression,
     * replacing every parameter (x, y) with the corresponding argument (2, 3),
     * leaving everything else unchanged.
     *
     * This is the heart of the MethodInliner.
     *
     * The original method AST is NEVER modified, but cloned instead.
     *
     * Example:
     *
     *     purpose add(number x, number y):
     *         return x + y
     *     amen
     *
     *     add(2, 3)
     *
     * Before cloning, this method builds a mapping called argumentValues:
     *
     *     x -> LiteralExpr(2)
     *     y -> LiteralExpr(3)
     *
     * During clone, the code below:
     *
     *     return x + y
     *
     * becomes:
     *
     *     return 2 + 3
     *
     * Then:
     *
     *     add(2, 3)
     *
     * in the end becomes:
     *
     *     2 + 3
     *
     * The resulting AST is an inlined clone of the method body.
     *
     * @param expression LiteralExpr(7)
     * @param argumentValues  argumentValues["x"] = LiteralExpr(7);
     */
    std::unique_ptr<Expr> cloneExpressionReplacingParamsWithArgs(
        const Expr &expression,
        const std::unordered_map<std::string, const Expr *> &argumentValues,
        const Expr *objectExpr);
};
