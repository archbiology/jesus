#pragma once

#include "expr.hpp"

REGISTER_FOR_UML(
    FormattedStringExpr,
    .packageName("ast.expr")
        .parentsList({"Expr"})
        .fieldsList({"raw", "parts"}));

/**
 * @brief AST node representing a formatted string literal.
 *
 * A formatted string is introduced with double quotes ("...") and may contain
 * placeholders in curly braces (e.g. "Hello, {name}").
 *
 * Parsing splits the string into:
 *   - literal parts (text between placeholders),
 *   - variable names (identifiers inside braces).
 *
 * At runtime, the interpreter evaluates each variable and constructs
 * the final string. This allows safe string interpolation without manual
 * concatenation.
 *
 * Compared to LiteralExpr:
 *   - LiteralExpr: stores a static, raw string (no substitution).
 *   - FormattedStringExpr: stores both literal parts and variables,
 *     enabling dynamic substitution at evaluation time.
 *
 * This design supports internationalization (i18n) and ensures safety by
 * checking that all variables are declared at parse time.
 *
 * "I too will have my say; I too will tell what I know." — Job 32:17
 */
class FormattedStringExpr : public Expr
{
public:
    const std::string raw;              // original string e.g., "Hello {name}"
    std::vector<std::string> parts;     // literal parts
    std::vector<std::unique_ptr<Expr>> expressions;

    explicit FormattedStringExpr(const std::string rawStr,
                                 std::vector<std::string> parts,
                                 std::vector<std::unique_ptr<Expr>> expressions)
        : raw(rawStr), parts(std::move(parts)), expressions(std::move(expressions)) {}

    Value evaluate(std::shared_ptr<Heart> heart) const override
    {
        return Value(raw);
    }

    Value accept(ExprVisitor &visitor) const override;

    /**
     * @brief Get the return type of the expression, so that variable
     *  creation and update can be enforced at parse time.
     *
     * "Flesh gives birth to flesh, but the Spirit gives birth to spirit." — John 3:6
     */
    std::shared_ptr<CreationType> getReturnType(ParserContext &ctx) const override;

    /**
     * @brief Returns a string representation of the expression.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    virtual std::string toString() const override { return "FormattedStringExpr('" + raw + ")'"; }
};
