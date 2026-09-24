#include "create_class_stmt_rule.hpp"
#include "ast/stmt/create_class_stmt.hpp"
#include "ast/stmt/incomplete_block_stmt.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/stmt/create_var_stmt.hpp"
#include "types/known_types.hpp"
#include "understanding/doctrine/law/ungodly_naming.hpp"
#include "lexer/keywords.hpp"
#include <stdexcept>

static std::unordered_map<std::string, std::string> collectAttributeAccessModifiers(
    const std::vector<std::unique_ptr<Stmt>> &body)
{
    // ---------------------------------------------------------------
    // Extract public/protected/private modifiers from class attributes.
    // e.g. __alpha__(private age: number): each new instance of this
    // class carries an 'age' attribute automatically.
    // ---------------------------------------------------------------
    std::unordered_map<std::string, std::string> attributeAccess;
    for (const auto &member : body)
    {
        if (auto methodStmt = dynamic_cast<CreateMethodStmt *>(member.get()))
        {
            if (methodStmt->isConstructor && !methodStmt->attributeNames.empty())
            {
                for (const auto &[paramName, access] : methodStmt->attributeNames)
                    attributeAccess[paramName] = access;
            }
        }
    }

    return attributeAccess;
}

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

    bool isUngodlyDeclared = ctx.match(TokenType::UNGODLY);

    if (ctx.check(TokenType::IDENTIFIER))
    {
        ctx.advance();
    }
    else if (Keywords::isReservedWord(ctx.peek().lexeme))
    {
        std::string className = ctx.peek().lexeme;
        throw std::runtime_error(Keywords::reservedWordMsg(className, "class"));
    }
    else
    {
        throw std::runtime_error("Expected class name after '" + stmt + "'");
    }

    std::string className = ctx.previous().lexeme;

    std::string bibleReference;
    bool nameIsUngodly = doctrine::law::isUngodlyName(className, bibleReference);
    if (nameIsUngodly && !isUngodlyDeclared)
    {
        doctrine::law::handleUngodlyNaming(
            "Class name '" + className + "' does not appear to reflect God's standards.\n"
            "To hide this warning, declare the class explicitly as 'ungodly'.\n\n"
            "Example:\n"
            "   let there be ungodly " + className + ":\n   amen\n\n"
            + bibleReference
        );
    }

    // -----------------
    // Class inheritance
    // -----------------
    std::string parentClassName = "";
    std::shared_ptr<CreationType> baseClassType = KnownTypes::resolve("creation", "core");
    if (ctx.match(TokenType::FROM))
    {
        if (ctx.check(TokenType::IDENTIFIER))
        {
            ctx.advance();
        }
        else if (Keywords::isReservedWord(ctx.peek().lexeme))
        {
            std::string parentName = ctx.peek().lexeme;
            throw std::runtime_error(Keywords::reservedWordMsg(parentName, "class"));
        }
        else
        {
            throw std::runtime_error("Expected base class name after 'from' in class declaration.");
        }

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
    auto attributes = std::make_shared<Heart>("class:" + className, baseClassType->class_attributes);
    std::vector<std::unique_ptr<Stmt>> body;
    std::string module_name = ctx.moduleName;

    // -------------------------------------------------------------------------
    // Register the class before parsing its body so that methods defined inside
    // the class can already refer to the class itself through 'I'/'my'.
    // -------------------------------------------------------------------------
    std::vector<std::shared_ptr<IConstraint>> constraints;
    auto userClass = std::make_shared<CreationType>(
        PrimitiveType::Class, className, module_name, baseClassType, attributes, constraints);
    ctx.registerType(userClass);
    ctx.registerClassName(className);

    try
    {
    ctx.consumeAllNewLines();

    if (ctx.isAtEnd())
    {
        // Allowing 'empty-bodied' classes without ': amen'.
        // Just: let there be Light
        userClass->attributeAccess = collectAttributeAccessModifiers(body);
        return std::make_unique<CreateClassStmt>(className, module_name, baseClassType, std::move(body), std::move(userClass));
    }

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after class name " + parentClassName + " in '" + stmt + "' statement.");

    ctx.consumeAllNewLines();

    // ----------
    // Class body
    // ----------
    ctx.addScope(attributes); // <🟢️>

    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        if (auto method = createMethod->parse(ctx))
        {
            // ------------------------------------------------------------
            // The method was already registered into the class by
            // CreateMethodStmtRule (before its own body was parsed), so a
            // method can call itself and its siblings recursively.
            // ------------------------------------------------------------
            body.push_back(std::move(method));
        }
        else if (auto attr = createVar->parse(ctx))
        {
            // ---------------------------------------------------------
            // Dependency Inversion: attributes are dependencies injected
            // through the '__alpha__' constructor. A class body may only
            // hold methods, so declaring an attribute here is an error.
            // ---------------------------------------------------------
            auto attrStmt = dynamic_cast<CreateVarStmt *>(attr.get());
            std::string attrName = attrStmt ? attrStmt->name : "<unnamed>";
            std::string attrType = "creation";
            if (attrStmt)
            {
                try
                {
                    auto resolvedType = ctx.getVarType(attrName);
                    if (resolvedType)
                        attrType = resolvedType->name;
                }
                catch (...)
                {
                }
            }

            throw std::runtime_error(
                "Class '" + className + "' cannot declare attribute '" + attrName +
                "' inside its body. Only methods are allowed there.\n\n"
                "To enforce Dependency Inversion, instance state and dependencies "
                "must be declared through the '__alpha__' constructor, where they "
                "are explicitly provided from outside the class.\n\n"
                "Declare the attribute in '__alpha__' using an access modifier so "
                "it becomes an instance attribute:\n\n"
                "    __alpha__(public " + attrName + ": " + attrType + "):\n"
                "    amen");
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

    }
    catch (...)
    {
        // ---------------------------------------------------------------
        // Clean up the parse-time registration if the class definition
        // (or one of its methods) fails to parse, so a partially-defined
        // class never remains visible to later statements (e.g. in REPL).
        // ---------------------------------------------------------------
        ctx.unregisterClassName(className);
        throw;
    }

    userClass->attributeAccess = collectAttributeAccessModifiers(body);
    return std::make_unique<CreateClassStmt>(className, module_name, baseClassType, std::move(body), std::move(userClass));
}
