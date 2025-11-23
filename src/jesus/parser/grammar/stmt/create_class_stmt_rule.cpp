#include "create_class_stmt_rule.hpp"
#include "../../../ast/stmt/create_class_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../../../types/known_types.hpp"
#include <stdexcept>

std::unique_ptr<Stmt> CreateClassStmtRule::parse(ParserContext &ctx)
{
    std::string stmt = "let there be";

    if (ctx.match(TokenType::HAJA))
    {
        stmt = "haja"; // haja Luz: amém
    }
    else
    {
        if (!ctx.match(TokenType::LET))
            return nullptr;

        if (!ctx.match(TokenType::THERE))
            return nullptr;

        if (!ctx.match(TokenType::BE))
            throw std::runtime_error("Expected 'be' after 'let there'");
    }

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected class name after '" + stmt + "'");

    std::string className = ctx.previous().lexeme;

    // -----------------
    // Class inheritance
    // -----------------
    std::string parentClassName = "";
    std::shared_ptr<CreationType> baseClassType = KnownTypes::resolve("creation", "core");
    if (ctx.match(TokenType::FROM))
    {
        if (!ctx.match(TokenType::IDENTIFIER))
            throw std::runtime_error("Expected base class name after 'from' in class declaration.");

        parentClassName = ctx.previous().lexeme;

        baseClassType = KnownTypes::resolve(parentClassName, "core");
        if (!baseClassType)
        {
            throw std::runtime_error("Unknown class type: '" + parentClassName + "'");
        }
    }

    // ---------------------------
    // The class may not have body
    // ---------------------------
    std::vector<std::shared_ptr<Stmt>> body;
    std::string module_name = ctx.moduleName;

    ctx.consumeAllNewLines();

    if (ctx.isAtEnd())
    {
        // Allowing 'empty-bodied' classes without ': amen'.
        // Just: let there be Light
        ctx.registerClassName(className);
        return std::make_unique<CreateClassStmt>(className, module_name, baseClassType, body);
    }

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after class name " + parentClassName + " in '" + stmt + "' statement.");

    ctx.consumeAllNewLines();

    // ----------
    // Class body
    // ----------
    auto attributes = std::make_shared<Heart>(className);
    ctx.addScope(attributes); // <🟢️>

    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        if (auto method = createMethod->parse(ctx))
        {
            body.push_back(std::move(method));
        }
        else if (auto attr = createVar->parse(ctx))
        {
            body.push_back(std::move(attr));
        }
        else
        {
            throw std::runtime_error("Unexpected statement inside class body.");
        }

        ctx.consumeAllNewLines();
    }
    ctx.popScope(); // </🟢️>

    if (ctx.isAtEnd())
    {
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error("Expected 'amen' after ':' in '" + stmt + "' to close class body.");

    ctx.registerClassName(className);
    return std::make_unique<CreateClassStmt>(className, module_name, baseClassType, body);
}
