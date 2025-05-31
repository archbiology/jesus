#include "parser.hpp"
#include "expression_parser.hpp"
#include "../ast/let_there_be_node.hpp"
#include "../ast/identifier_node.hpp"
#include "../ast/value_node.hpp"
#include "../ast/reveal_node.hpp"
#include "../ast/binary_operation_node.hpp"
#include "../ast/expression_node.hpp"
#include "../ast/conditional_expression_node.hpp"
#include <bits/stdc++.h> // for std::find_if and std::distance
#include <string>

std::unique_ptr<ASTNode> parse(const std::vector<Token> &tokens)
{
    const int tokens_count = tokens.size();
    if (!tokens_count)
        return nullptr;

    if (tokens_count == 1 && tokens[0].type == TokenType::IDENTIFIER) {
        return std::make_unique<IdentifierNode>(tokens[0].lexeme);
    }

    // parse use "value" if condition otherwise "value"
    if (tokens[0].lexeme == "reveal" && tokens_count >= 2)
    {
        // Check if "if" exists in tokens
        auto ifIt = std::find_if(tokens.begin(), tokens.end(),
                               [](const Token &t)
                               { return t.lexeme == "if"; });

        if (ifIt != tokens.end())
        {

            int otherwiseIndex = -1;
            int ifIndex = std::distance(tokens.begin(), ifIt);
            ASTNode *elseValue = NULL;

            ASTNode *ifValue = new ValueNode(Value(tokens[1].lexeme));

            ASTNode *condition;
            if (tokens[ifIndex + 1].lexeme == "(")
            {
                int parenDepth = 1;
                size_t conditionStart = ifIndex + 1;
                size_t conditionEnd = conditionStart;

                // Find matching ')'
                while (conditionEnd < tokens.size() && parenDepth > 0)
                {
                    if (tokens[conditionEnd].lexeme == "(")
                        ++parenDepth;

                    else if (tokens[conditionEnd].lexeme == ")")
                        --parenDepth;

                    ++conditionEnd;
                }

                if (parenDepth != 1)
                {
                    throw std::runtime_error("Unmatched parentheses in if condition. parenDepth = " + std::to_string(parenDepth));
                }

                std::vector<Token> conditionTokens(tokens.begin() + conditionStart, tokens.begin() + conditionEnd);

                auto parsedCondition = parseExpression(conditionTokens);
                if (!parsedCondition)
                {
                    throw std::runtime_error("Failed to parse condition expression inside 'if'.");
                }

                condition = parsedCondition.release(); // make it raw pointer
            }
            else
                condition = new BinaryOperationNode(
                    tokens[ifIndex + 2].lexeme,                     // operator (e.g.: == )
                    new IdentifierNode(tokens[ifIndex + 1].lexeme), // left operand
                    new ValueNode(tokens[ifIndex + 3].literal));    // right operand

            // Parse "otherwise/else"
            for (size_t i = ifIndex + 1; i < tokens_count; ++i)
            {
                if (tokens[i].lexeme == "otherwise")
                {
                    otherwiseIndex = i;
                    elseValue = new ValueNode(Value(tokens[otherwiseIndex + 1].lexeme));
                    break;
                }
            }

            return std::make_unique<ConditionalExpressionNode>(condition, ifValue, elseValue);
        }

        return std::make_unique<RevealNode>(new IdentifierNode(tokens[1].lexeme));
    }

    if (tokens_count >= 3 &&
        tokens[0].lexeme == "let" &&
        tokens[1].lexeme == "there" &&
        tokens[2].lexeme == "be")
    {
        std::string identifier;
        Value value;

        if (tokens_count >= 4)
        {
            identifier = tokens[3].lexeme;
        }

        if (tokens_count >= 7 && tokens[4].lexeme == "set" && tokens[5].lexeme == "to")
        {
            value = tokens[6].literal;
        }

        auto idNode = std::make_unique<IdentifierNode>(identifier);
        auto valNode = std::make_unique<ValueNode>(value);

        return std::make_unique<LetThereBeNode>(std::move(idNode), std::move(valNode));
    }

    // If no match, fall back to expression parsing
    return parseExpression(tokens);
}

std::unique_ptr<ASTNode> parseExpression(const std::vector<Token> &tokens)
{
    try
    {
        ExpressionParser exprParser(tokens);
        auto expr = exprParser.parse();

        if (expr)
        {
            return std::make_unique<ExpressionNode>(std::move(expr));
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << "💥 Expression parsing error: " << e.what() << "\n";
    }

    return nullptr; // or an error node in the future
}
