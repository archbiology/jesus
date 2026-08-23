#include "create_class_stmt_rule.hpp"
#include "ast/stmt/create_class_stmt.hpp"
#include "ast/stmt/incomplete_block_stmt.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/stmt/create_var_stmt.hpp"
#include "interpreter/runtime/method.hpp"
#include "types/known_types.hpp"
#include "understanding/doctrine/law/ungodly_naming.hpp"
#include <stdexcept>

static bool addMethodToClass(
    std::shared_ptr<CreationType> &userClass,
    const CreateMethodStmt *methodStmt,
    bool &hasConstructor,
    bool &hasDestructor)
{
    if (methodStmt->isConstructor)
    {
        if (hasConstructor)
        {
            throw std::runtime_error("Class '" + userClass->name + "' already has a constructor '__alpha__'.");
        }
        hasConstructor = true;

        // -------------------------------------------------------
        // Register the constructor as a method so that it can be
        // found and invoked automatically whenever a new instance
        // of this class is created.
        // -------------------------------------------------------
        userClass->addMethod(
            methodStmt->name,
            std::make_shared<Method>(
                methodStmt->name,
                methodStmt->params,
                methodStmt->body,
                methodStmt->returnType,
                methodStmt->attributeNames));

        return true;
    }

    if (methodStmt->isDestructor)
    {
        if (hasDestructor)
        {
            throw std::runtime_error("Class '" + userClass->name + "' already has a destructor '__omega__'.");
        }
        hasDestructor = true;

        // -------------------------------------------------------------
        // Register the destructor as a method so that it can be
        // found and invoked automatically when an instance passes away.
        // -------------------------------------------------------------
        userClass->addMethod(
            methodStmt->name,
            std::make_shared<Method>(methodStmt->name, methodStmt->params, methodStmt->body, methodStmt->returnType));

        return true;
    }

    userClass->addMethod(
        methodStmt->name,
        std::make_shared<Method>(methodStmt->name, methodStmt->params, methodStmt->body, methodStmt->returnType));

    return false;
}

static std::unordered_map<std::string, std::string> collectAttributeAccessModifiers(
    const std::vector<std::shared_ptr<Stmt>> &body)
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

    if (!ctx.match(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected class name after '" + stmt + "'");

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
    auto attributes = std::make_shared<Heart>("class:" + className, baseClassType->class_attributes);
    std::vector<std::shared_ptr<Stmt>> body;
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

    bool hasConstructor = false;
    bool hasDestructor = false;

    ctx.consumeAllNewLines();

    if (ctx.isAtEnd())
    {
        // Allowing 'empty-bodied' classes without ': amen'.
        // Just: let there be Light
        userClass->attributeAccess = collectAttributeAccessModifiers(body);
        return std::make_unique<CreateClassStmt>(className, module_name, baseClassType, body, std::move(userClass));
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
            // Register access-modified constructor parameters as class
            // attributes as soon as the constructor is parsed, so that
            // method bodies parsed afterwards can reference them (variable
            // resolution happens at parse time).
            // ------------------------------------------------------------
            if (auto methodStmt = dynamic_cast<CreateMethodStmt *>(method.get()))
            {
                if (methodStmt->isConstructor && !methodStmt->attributeNames.empty())
                {
                    for (const auto &[paramName, access] : methodStmt->attributeNames)
                    {
                        auto paramType = methodStmt->params->getVarType(paramName);
                        attributes->createVar(paramType, paramName, Value(), /** isParam = */ false);
                    }
                }

                addMethodToClass(userClass, methodStmt, hasConstructor, hasDestructor);
            }

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

    userClass->attributeAccess = collectAttributeAccessModifiers(body);
    return std::make_unique<CreateClassStmt>(className, module_name, baseClassType, body, std::move(userClass));
}
