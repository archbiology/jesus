#include "parser.hpp"
#include "grammar/jesus_grammar.hpp"
#include "parser_context.hpp"
#include "../ast/expr/variable_expr.hpp"
#include "../ast/stmt/repeat_times_stmt.hpp"
#include "../ast/stmt/output_statement.hpp"
#include <bits/stdc++.h> // for std::find_if and std::distance
#include <string>
#include <memory>

std::unique_ptr<Stmt> parse(const std::vector<Token> &tokens, ParserContext &context)
{
    const int tokens_count = tokens.size();
    if (!tokens_count)
        return nullptr;

    if (tokens_count == 1 && tokens[0].type == TokenType::IDENTIFIER)
    {
        // WARN because the person didn't use 'say'
        return std::make_unique<OutputStmt>(OutputType::WARN, std::make_unique<VariableExpr>(tokens[1].lexeme));
    }

    // repeat n times
    if (tokens_count >= 4 &&
        tokens[0].type == TokenType::REPEAT &&
        tokens[1].type == TokenType::INT &&
        tokens[2].type == TokenType::TIMES &&
        tokens[3].lexeme == ":")
    {
        int repeatCount = std::stoi(tokens[1].lexeme); // assuming it's an integer literal

        std::vector<Token> innerTokens(tokens.begin() + 4, tokens.end());

        std::unique_ptr<Stmt> innerStmt = parse(innerTokens, context);
        if (!innerStmt)
        {
            throw std::runtime_error("Could not parse body of 'repeat N times'");
        }

        return std::make_unique<RepeatTimesStmt>(repeatCount, std::move(innerStmt));
    }

    if ((tokens[0].type == TokenType::SAY || tokens[0].type == TokenType::WARN) && tokens_count >= 2)
    {
        std::vector<Token> expressionTokens(tokens.begin() + 1, tokens.end());
        ParserContext sub_context(expressionTokens, context.interpreter);
        auto expr = grammar::Expression->parse(sub_context);
        if (expr)
            return std::make_unique<OutputStmt>(
                tokens[0].type == TokenType::SAY ? OutputType::SAY : OutputType::WARN,
                std::move(expr));
    }

    int snapshot = context.snapshot();
    auto createVarTypeStmt = grammar::CreateVarType->parse(context);
    if (createVarTypeStmt)
        return createVarTypeStmt;

    context.restore(snapshot);
    auto createVarStmt = grammar::CreateVar->parse(context);
    if (createVarStmt)
        return createVarStmt;

    auto updateVarStmt = grammar::UpdateVar->parse(context);
    if (updateVarStmt)
        return updateVarStmt;

    // If no match, fall back to expression parsing
    auto expr = grammar::Expression->parse(context);
    if (!expr)
    {
        throw std::runtime_error("Unknown expression type (parser.cpp)");
    }

    // Using WARN because the person didn't call 'say'
    return std::make_unique<OutputStmt>(OutputType::WARN, std::move(expr));
}
