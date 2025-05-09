#include "parser.hpp"
#include "ast/let_there_be_node.hpp"
#include "ast/identifier_node.hpp"
#include "ast/value_node.hpp"
#include "ast/reveal_node.hpp"
#include "ast/binary_operation_node.hpp"
#include "ast/conditional_expression_node.hpp"
#include <bits/stdc++.h> // for std::find_if and std::distance

std::unique_ptr<ASTNode> parse(const std::vector<Token> &tokens)
{
    const int tokens_count = tokens.size();
    if (!tokens_count)
        return nullptr;

    // parse use "value" if condition otherwise "value"
    if (tokens[0].value == "reveal" && tokens_count >= 2)
    {
        // Check if "if" exists in tokens
        auto it = std::find_if(tokens.begin(), tokens.end(),
                               [](const Token &t)
                               { return t.value == "if"; });

        if (it != tokens.end())
        {

            int otherwiseIndex = -1;
            int ifIndex = std::distance(tokens.begin(), it);
            ASTNode *elseValue = NULL;

            ASTNode *ifValue = new ValueNode(tokens[1].value);
            ASTNode *condition = new BinaryOperationNode(
                tokens[ifIndex + 2].value,                     // operator (e.g.: == )
                new IdentifierNode(tokens[ifIndex + 1].value), // left operand
                new ValueNode(tokens[ifIndex + 3].value));     // right operand

            // Parse "otherwise/else"
            for (size_t i = ifIndex + 1; i < tokens_count; ++i)
            {
                if (tokens[i].value == "otherwise")
                {
                    otherwiseIndex = i;
                    elseValue = new ValueNode(tokens[otherwiseIndex + 1].value);
                    break;
                }
            }

            return std::make_unique<ConditionalExpressionNode>(condition, ifValue, elseValue);
        }

        return std::make_unique<RevealNode>(new IdentifierNode(tokens[1].value));
    }

    if (tokens_count >= 3 &&
        tokens[0].value == "let" &&
        tokens[1].value == "there" &&
        tokens[2].value == "be")
    {
        std::string identifier;
        std::string value;

        if (tokens_count >= 4)
        {
            identifier = tokens[3].value;
        }

        if (tokens_count >= 7 && tokens[4].value == "set" && tokens[5].value == "to")
        {
            value = tokens[6].value;
        }

        auto idNode = std::make_unique<IdentifierNode>(identifier);
        auto valNode = std::make_unique<ValueNode>(value);

        return std::make_unique<LetThereBeNode>(std::move(idNode), std::move(valNode));
    }

    return nullptr; // or an error node in the future
}
