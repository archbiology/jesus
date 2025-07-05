#include "parser.hpp"
#include "grammar/jesus_grammar.hpp"
#include "parser_context.hpp"
#include "../ast/identifier_node.hpp"
#include "../ast/expr/literal_expr.hpp"
#include "../ast/expr/variable_expr.hpp"
#include "../ast/binary_operation_node.hpp"
#include "../ast/expr/conditional_expr.hpp"
#include "../ast/stmt/repeat_times_stmt.hpp"
#include "../ast/stmt/output_statement.hpp"
#include "../ast/stmt/create_var_stmt.hpp"
#include <bits/stdc++.h> // for std::find_if and std::distance
#include <string>
#include <memory>

std::unique_ptr<Stmt> parse(const std::vector<Token> &tokens)
{
    const int tokens_count = tokens.size();
    if (!tokens_count)
        return nullptr;

    if (tokens_count == 1 && tokens[0].type == TokenType::IDENTIFIER)
    {
        // WARN because the person didn't use 'say'
        return std::make_unique<OutputStmt>(OutputType::WARN,  std::make_unique<VariableExpr>(tokens[1].lexeme));

    }

    if (tokens_count >= 4 &&
        tokens[0].type == TokenType::REPEAT &&
        tokens[1].type == TokenType::INT &&
        tokens[2].type == TokenType::TIMES &&
        tokens[3].lexeme == ":")
    {
        int repeatCount = std::stoi(tokens[1].lexeme); // assuming it's an integer literal

        std::vector<Token> innerTokens(tokens.begin() + 4, tokens.end());

        std::unique_ptr<Stmt> innerStmt = parse(innerTokens);
        if (!innerStmt)
        {
            throw std::runtime_error("Could not parse body of 'repeat N times'");
        }

        return std::make_unique<RepeatTimesStmt>(repeatCount, std::move(innerStmt));
    }

    // parse use "value" if condition otherwise "value"
    if ((tokens[0].type == TokenType::SAY || tokens[0].type == TokenType::WARN)  && tokens_count >= 2)
    {
        // Check if "if" exists in tokens
        auto ifIt = std::find_if(tokens.begin(), tokens.end(),
                               [](const Token &t)
                               { return t.lexeme == "if"; });

        if (ifIt != tokens.end())
        {

            int otherwiseIndex = -1;
            int ifIndex = std::distance(tokens.begin(), ifIt);
            std::unique_ptr<Expr> condition;
            std::unique_ptr<Expr> ifValue = std::make_unique<LiteralExpr>(Value(tokens[1].lexeme));
            std::unique_ptr<Expr> elseValue;

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

                ParserContext context(conditionTokens);
                condition = grammar::Expression->parse(context);
                if (!condition)
                {
                    throw std::runtime_error("Failed to parse condition expression inside 'if'.");
                }
            }
            else
                condition = std::make_unique<BinaryOperationNode>(
                    tokens[ifIndex + 2].lexeme,                     // operator (e.g.: == )
                    std::make_unique<IdentifierNode>(tokens[ifIndex + 1].lexeme), // left operand
                    std::make_unique<LiteralExpr>(tokens[ifIndex + 3].literal)  // right operand
                );

            // Parse "otherwise/else"
            for (size_t i = ifIndex + 1; i < tokens_count; ++i)
            {
                if (tokens[i].lexeme == "otherwise")
                {
                    otherwiseIndex = i;
                    elseValue = std::make_unique<LiteralExpr>(Value(tokens[otherwiseIndex + 1].lexeme));
                    break;
                }
            }

            return std::make_unique<OutputStmt>(OutputType::WARN,  std::make_unique<ConditionalExpr>(std::move(condition), std::move(ifValue), std::move(elseValue)));
        }

        // Using WARN because the person didn't call 'say'
        if (tokens[1].type == TokenType::IDENTIFIER)
            return std::make_unique<OutputStmt>(OutputType::WARN,  std::make_unique<VariableExpr>(tokens[1].lexeme));

        return std::make_unique<OutputStmt>(OutputType::WARN, std::make_unique<LiteralExpr>(tokens[1].literal));
    }

    if (tokens_count >= 3 &&
        tokens[0].lexeme == "let" &&
        tokens[1].lexeme == "there" &&
        tokens[2].lexeme == "be")
    {
        std::string identifier;
        std::unique_ptr<Expr> value;

        if (tokens_count >= 4)
        {
            identifier = tokens[3].lexeme;
        }

        if (tokens_count >= 7 && tokens[4].lexeme == "set" && tokens[5].lexeme == "to")
        {
            value = std::make_unique<LiteralExpr>(tokens[6].literal);
        }

        return std::make_unique<CreateVarStmt>(identifier, std::move(value));
    }

    // If no match, fall back to expression parsing
    ParserContext context(tokens);
    auto expr = grammar::Expression->parse(context);
    if (!expr)
    {
        throw std::runtime_error("Unknown expression type");
    }

    // Using WARN because the person didn't call 'say'
    return std::make_unique<OutputStmt>(OutputType::WARN, std::move(expr));
}
