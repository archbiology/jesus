#pragma once
#include "ast_node.hpp"
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
    Value evaluate(Heart *heart) override
    {


        Value l = left->evaluate(heart);
        Value r = right->evaluate(heart);

        std::cout<< " 🔹 BinaryOperationNode.left.result: " << std::visit(make_string_functor(), l) + "\n";
        std::cout<< " 🔹 BinaryOperationNode.right.result: " << std::visit(make_string_functor(), r) + "\n";

        if (!std::holds_alternative<double>(l) || !std::holds_alternative<double>(r)) {

            std::cout<< " 🤔 left of right are not number. Returning.\n";
            std::cout<< "   🤔 left.holds-double? " << (std::holds_alternative<double>(l) ?  "SIM" : "NAO") << "\n";
            std::cout<< "   🤔 right.holds-double? " << (std::holds_alternative<double>(r) ?  "SIM" : "NAO")<< "\n";


            std::cout<< "   🤔 left.holds-int? " << (std::holds_alternative<int>(l) ?  "SIM" : "NAO") << "\n";
            std::cout<< "   🤔 right.holds-int? " << (std::holds_alternative<int>(r) ?  "SIM" : "NAO")<< "\n";

            std::cout<< "   🤔 left.holds-str? " << (std::holds_alternative<std::string>(l) ?  "SIM" : "NAO") << "\n";
            std::cout<< "   🤔 right.holds-str? " << (std::holds_alternative<std::string>(r) ?  "SIM" : "NAO")<< "\n";


            return std::monostate{};
        }

        double leftNum = std::get<double>(l);
        double rightNum = std::get<double>(r);

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

        return std::monostate{};
    }

    void execute(Heart *heart) override {}

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
