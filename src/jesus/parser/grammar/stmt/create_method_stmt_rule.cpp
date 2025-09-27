#include "create_method_stmt_rule.hpp"
#include "../../../ast/stmt/create_method_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../../../ast/stmt/return_stmt.hpp"
#include "../../../types/creation_type.hpp"
#include "../../parser_context.hpp"
#include "../../../types/known_types.hpp"
#include "../jesus_grammar.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> CreateMethodStmtRule::parse(ParserContext &ctx)
{
    // ------------------------------------------------------
    // Grammar:
    //  purpose <name> ( <params>? ) -> returnType? : ... amen
    // ------------------------------------------------------

    if (!ctx.match(TokenType::PURPOSE))
        return nullptr;

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected method name after 'calling'");

    std::string methodName = ctx.previous().lexeme;

    // ----------
    // Parameters
    // ----------
    if (!ctx.match(TokenType::LEFT_PAREN))
        throw std::runtime_error("Expected '(' after method name in 'calling' statement.");

    auto params = std::make_shared<Heart>(methodName);
    ctx.addScope(params); // <🟢️>

    if (!ctx.check(TokenType::RIGHT_PAREN))
    {
        do
        {
            if (!ctx.match(TokenType::IDENTIFIER))
                throw std::runtime_error("Expected parameter type in method declaration.");

            std::string type = ctx.previous().lexeme;

            if (!ctx.match(TokenType::IDENTIFIER))
                throw std::runtime_error("Expected parameter name after type '" + type + "'.");

            std::string name = ctx.previous().lexeme;

            params->createVar(type, name, Value(1)); // FIXME: Validate `type` and allow initial values

        } while (ctx.match(TokenType::SEMICOLON)); // TODO: allow more args of same type: int x, y, z; string name, surname;
    }

    if (!ctx.match(TokenType::RIGHT_PAREN))
        throw std::runtime_error("Expected ')' after parameter list in method declaration.");

    // -----------
    // Return type
    // -----------
    std::shared_ptr<CreationType> returnType = KnownTypes::VOID; // default: nothing
    if (ctx.match(TokenType::ARROW))
    {
        if (!ctx.match(TokenType::IDENTIFIER))
            throw std::runtime_error("Expected return type after '->'.");

        std::string typeName = ctx.previous().lexeme;
        returnType = KnownTypes::resolve(typeName, "core");
        if (!returnType)
        {
            throw std::runtime_error("Unknown return type: '" + typeName + "'.");
        }
    }

    // -----------
    // Method body
    // -----------
    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after method signature.");

    std::vector<std::shared_ptr<Stmt>> body;

    ctx.consumeAllNewLines();

    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        if (auto stmt = updateVar->parse(ctx))
        {
            body.push_back(std::move(stmt));
        }
        else if (auto attr = createVar->parse(ctx))
        {
            body.push_back(std::move(attr));
        }
        else if (auto print = printStmt->parse(ctx))
        {
            body.push_back(std::move(print));
        }
        else if (ctx.match(TokenType::RETURN))
        {
            std::unique_ptr<Expr> returnExpr = nullptr;

            if (!ctx.check(TokenType::NEWLINE) && !ctx.check(TokenType::AMEN))
            {
                // Parse the expression after 'return'
                returnExpr = grammar::Expression->parse(ctx);
            }

            body.push_back(std::make_shared<ReturnStmt>(std::move(returnExpr)));
        }
        else
        {
            throw std::runtime_error("Unexpected statement inside method body.");
        }

        ctx.consumeAllNewLines();
    }
    ctx.popScope(); // </🟢️>

    if (ctx.isAtEnd())
    {
        // FIXME: Each time an IncompleteBlockStmt is returned, all code is parsed again. Too expensive.
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' to close method body.");

    // ---------------------------
    // Enforce correct return type
    // ---------------------------
    for (const auto &stmt : body)
    {
        if (auto ret = dynamic_cast<ReturnStmt *>(stmt.get()))
        {
            auto actualType = ret->getReturnType(ctx);

            if (!returnType->isCompatibleWith(actualType))
            {
                throw std::runtime_error(
                    "Type mismatch in method '" + methodName +
                    "': expected return type '" + returnType->toString() +
                    "', but found '" + actualType->toString() + "'.");
            }
        }
    }

    // -----------------------------
    // Enforce explicit return rules
    // -----------------------------
    if (!returnType->isVoid())
    {
        if (body.empty())
        {
            throw std::runtime_error(
                "Method '" + methodName + "' with return type '" +
                returnType->toString() + "' must end with an explicit return.");
        }

        auto lastStmt = body.back().get();
        auto ret = dynamic_cast<ReturnStmt *>(lastStmt);
        if (!ret)
        {
            throw std::runtime_error(
                "Method '" + methodName + "' with return type '" +
                returnType->toString() + "' must end with an explicit return.");
        }
    }

    return std::make_unique<CreateMethodStmt>(methodName, params, returnType, body, /*isGenesis=*/false);
}
