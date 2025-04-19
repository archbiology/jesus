#pragma once
#include "ast_node.hpp"
#include "value_node.hpp"
#include "spirit/heart.hpp"

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
struct BinaryOperationNode : public ASTNode
{
    std::string op;
    ASTNode *left;
    ASTNode *right;

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
    BinaryOperationNode(std::string op, ASTNode *left, ASTNode *right)
        : op(op), left(left), right(right) {}

    /**
     * @brief Evaluates the binary operation and returns an optional value.
     *
     * @param heart The "Symbol table"
     * @return An optional string result: "true", "false", or std::nullopt if evaluation failed.
     */
    std::optional<std::string> evaluate(Heart *heart)
    {
        auto l = left->evaluate(heart);
        auto r = right->evaluate(heart);

        if (!l.has_value() || !r.has_value())
            return std::nullopt;

        int leftNum = std::stoi(l.value());
        int rightNum = std::stoi(r.value());

        if (op == ">=")
            return (leftNum >= rightNum) ? "true" : "false";

        if (op == "<=")
            return (leftNum <= rightNum) ? "true" : "false";

        if (op == "==")
            return (leftNum == rightNum) ? "true" : "false";

        if (op == "!=")
            return (leftNum != rightNum) ? "true" : "false";

        if (op == ">")
            return (leftNum > rightNum) ? "true" : "false";

        if (op == "<")
            return (leftNum < rightNum) ? "true" : "false";

        return std::nullopt;
    }

    void execute(Heart *heart) override {}
};
