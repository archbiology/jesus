#pragma once
#include "../ast/expr/expr.hpp"
#include "../spirit/heart.hpp"

/**
 * @brief Represents a binary operation between two AST nodes (e.g., age >= 18)
 *
 * This node performs a comparison between the evaluated values of two
 * child nodes (`left` and `right`) using a specified operator such as:
 * `>=`, `<=`, `==`, `!=`, `>`, or `<`.
 *
 * Example usage:
 *     BinaryOperationNode(">=", leftNode, rightNode);
 *     => Evaluates to "true" or "false" depending on the values.
 */
struct BinaryOperationNode : public Expr
{
    std::string op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;

    /**
     * @brief Construct a new BinaryOperationNode object (e.g., age >= 18)
     *
     * 📖 "But solid food is for the mature, who by constant use have trained themselves to distinguish good from evil."
     * — Hebrews 5:14
     *
     * @param op The binary operator as a string (e.g., "==", "!=").
     * @param left Pointer to the left operand node.
     * @param right Pointer to the right operand node.
     */
    BinaryOperationNode(std::string op, std::unique_ptr<Expr> left, std::unique_ptr<Expr> right)
        : op(op), left(std::move(left)), right(std::move(right)) {}
    /**
     * @brief Evaluates the binary operation and returns an optional value.
     *
     * @param heart The "Symbol table"
     * @return An optional string result: "true", "false", or std::nullopt if evaluation failed.
     */
    Value evaluate(Heart *heart) override
    {
        Value l = left->evaluate(heart);
        Value r = right->evaluate(heart);

        if (op == ">=")
            return Value(l >= r);

        if (op == "<=")
            return Value(l <= r);

        if (op == "==")
            return Value(l == r);

        if (op == "!=")
            return Value(l != r);

        if (op == ">")
            return Value(l > r);

        if (op == "<")
            return Value(l < r);

        if (op == "or")
            return Value(l.AS_BOOLEAN || r.AS_BOOLEAN);

        if (op == "and")
            return Value(l.AS_BOOLEAN && r.AS_BOOLEAN);

        if (op == "versus") {
            if (l.IS_BOOLEAN && r.IS_BOOLEAN) {
                // Logical XOR
                return Value(l.AS_BOOLEAN != r.AS_BOOLEAN);
            }

            if (l.IS_NUMBER && r.IS_BOOLEAN) {
                // Bitwise XOR
                return Value(l.toInt() ^ r.toInt());
            }

            return Value::formless(); // fallback if types mismatch
        }

        return Value::formless();
    }

    /**
     * @brief Returns a string representation of the node.
     *
     * "For nothing is hidden that will not be made manifest, nor is anything
     * secret that will not be known and come to light." — Luke 8:17
     */
    std::string toString() const override {

        std::string str = "BinaryOperationNode";

        str += "(operator: " + op;

        if (left) str += ", left: " + left->toString();
        if (right) str += ", right: " + right->toString();

        str += ")";

        return str;
    }
};
