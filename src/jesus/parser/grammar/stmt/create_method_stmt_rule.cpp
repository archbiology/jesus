#include "create_method_stmt_rule.hpp"
#include "../../../ast/stmt/create_method_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../../../ast/stmt/if_stmt.hpp"
#include "../../../ast/stmt/print_stmt.hpp"
#include "../../../ast/stmt/return_stmt.hpp"
#include "../../../interpreter/runtime/method.hpp"
#include "../../../types/creation_type.hpp"
#include "../../parser_context.hpp"
#include "../../../types/known_types.hpp"
#include "../../../spirit/heart.hpp"
#include "../../../understanding/doctrine/law/ungodly_naming.hpp"
#include "lexer/keywords.hpp"
#include "../jesus_grammar.hpp"
#include <stdexcept>

namespace
{
/**
 * @brief Check if the method always have a `return`
 */
bool alwaysReturns(const Stmt *stmt)
{
    if (dynamic_cast<const ReturnStmt *>(stmt))
        return true;

    if (auto ifStmt = dynamic_cast<const IfStmt *>(stmt))
    {
        if (ifStmt->thenBranch.empty() || ifStmt->otherwiseBranch.empty())
            return false;

        return alwaysReturns(ifStmt->thenBranch.back().get()) && alwaysReturns(ifStmt->otherwiseBranch.back().get());
    }

    return false;
}

/**
 * @brief Registers the method into the class currently being parsed, right after
 * its signature is known but before its body — so the method can call itself
 * (recursion) and any sibling at parse time. Also promotes public/protected/private
 * constructor parameters into instance attributes.
 */
void addMethodToClass(ParserContext &ctx, CreateMethodStmt *methodStmt)
{
    auto klass = ctx.currentClassType();
    if (!klass)
    {
        throw std::runtime_error("Method '" + methodStmt->name + "' must be declared inside a class body.");
    }

    // -----------------------------------
    // Only one constructor / destructor
    // -----------------------------------
    if (methodStmt->isConstructor && klass->methods.count("__alpha__"))
    {
        throw std::runtime_error("Class '" + klass->name + "' already has a constructor '__alpha__'.");
    }
    if (methodStmt->isDestructor && klass->methods.count("__omega__"))
    {
        throw std::runtime_error("Class '" + klass->name + "' already has a destructor '__omega__'.");
    }

    // ----------------------------------------------------------------
    // Promote public/protected/private constructor parameters to
    // instance attributes as soon as the signature is known, so that
    // methods parsed afterwards can reference them at parse time.
    // ----------------------------------------------------------------
    if (methodStmt->isConstructor)
    {
        for (const auto &[paramName, access] : methodStmt->attributeNames)
        {
            auto paramType = methodStmt->params->getVarType(paramName);
            klass->class_attributes->createVar(paramType, paramName, Value(), /*isParam=*/false);
        }
    }

    klass->addMethod(
        methodStmt->name,
        std::make_shared<Method>(
            methodStmt->name,
            methodStmt->params,
            methodStmt,
            methodStmt->returnType,
            methodStmt->isConstructor ? methodStmt->attributeNames
                                      : std::vector<std::pair<std::string, std::string>>{}));
}

/**
 * @brief Enforce Dependency Inversion by not allowing instantiations (`= SomeClass()`)
 * inside a method/constructor/destructor body.
 *
 * It records the name of the method being parsed in
 * `ParserContext::dependencyInversionEnforcedScope` so error messages can
 * reference the actual method being written (`purpose praise()`,
 * `__alpha__`, or `__omega__`).
 *
 * The restriction starts after the method signature is parsed.
 * This keeps parameter default values such as `engine: Engine = Engine()`
 * legal because they belong to the parameter list, not the body.
 *
 * The restriction is automatically removed when the body finishes,
 * also when parsing stops because of an error.
 */
class EnforceDependencyInversion
{
public:
    EnforceDependencyInversion(ParserContext &ctx, std::string parsedMethodName)
        : m_ctx(ctx), m_previous(ctx.dependencyInversionEnforcedScope)
    {
        // Keeping the previous value (instead of clearing to "") lets a
        // pathological nested method parse restore the outer ban correctly.
        m_ctx.dependencyInversionEnforcedScope = std::move(parsedMethodName);
    }

    ~EnforceDependencyInversion()
    {
        m_ctx.dependencyInversionEnforcedScope = m_previous;
    }

private:
    ParserContext &m_ctx;
    const std::string m_previous;
};
} // namespace

std::unique_ptr<Stmt> CreateMethodStmtRule::parse(ParserContext &ctx)
{
    // ------------------------------------------------------
    // Grammar:
    //  __alpha__ ( <params>? ) -> returnType? : ... amen
    //  __omega__ () : ... amen
    //  [ungodly] purpose <name> ( <params>? ) -> returnType? : ... amen
    //
    //  <params> ::= (name: type) (';' name: type)*
    // ------------------------------------------------------
    bool isConstructor = false;
    bool isDestructor = false;
    bool isUngodlyDeclared = false;
    std::string methodName;

    if (ctx.match(TokenType::ALPHA))
    {
        isConstructor = true;
        methodName = "__alpha__";
    }
    else if (ctx.match(TokenType::OMEGA))
    {
        isDestructor = true;
        methodName = "__omega__";
    }
    else
    {
        isUngodlyDeclared = ctx.match(TokenType::UNGODLY);

        if (!ctx.match(TokenType::PURPOSE))
            return nullptr;

        if (ctx.check(TokenType::IDENTIFIER))
        {
            ctx.advance();
        }
        else if (Keywords::isReservedWord(ctx.peek().lexeme))
        {
            std::string methodName = ctx.peek().lexeme;
            throw std::runtime_error(Keywords::reservedWordMsg(methodName, "method"));
        }
        else
        {
            throw std::runtime_error("Expected method name after 'purpose'");
        }

        methodName = ctx.previous().lexeme;
    }

    // -----------------------------------
    // Ungodly semantic validation
    // -----------------------------------
    if (!isConstructor && !isDestructor)
    {
        std::string bibleReference;
        bool nameIsUngodly = doctrine::law::isUngodlyName(methodName, bibleReference);

        if (nameIsUngodly && !isUngodlyDeclared)
        {
            doctrine::law::handleUngodlyNaming(
                "Method name '" + methodName + "' does not appear to reflect God's standards.\n" +
                "To hide this warning, declare the method explicitly as 'ungodly'.\n\n" +
                "Example:\n" +
                "   ungodly purpose " + methodName + "():\n   amen\n\n" +
                bibleReference);
        }
    }

    // ----------
    // Parameters
    // ----------
    if (!ctx.match(TokenType::LEFT_PAREN))
        throw std::runtime_error("Expected '(' after method name in 'calling' statement.");

    auto params = std::make_shared<Heart>("method:" + methodName);
    ctx.addScope(params); // <🟢️>
    const bool isParam = true;
    std::vector<std::pair<std::string, std::string>> attributeNames;
    std::vector<std::shared_ptr<Expr>> defaultValues;

    if (isDestructor)
    {
        if (!ctx.check(TokenType::RIGHT_PAREN))
            throw std::runtime_error("Destructor '__omega__' cannot have parameters.");
    }
    else if (!ctx.check(TokenType::RIGHT_PAREN))
    {
        do
        {
            // ----------------------------------
            // Allowing params in multiple lines.
            // ----------------------------------
            ctx.consumeAllNewLines();
            if (ctx.isAtEnd())
            {
                ctx.popScope();
                return std::make_unique<IncompleteBlockStmt>();
            }

            // -------------------------------------------------------------
            // Access modifiers (private/protected/public) are only allowed
            // on constructor parameters: they promote the parameter to an
            // instance attribute.
            // -------------------------------------------------------------
            std::string access = "";
            if (ctx.matchAny({TokenType::PRIVATE, TokenType::PROTECTED, TokenType::PUBLIC}))
            {
                if (!isConstructor)
                {
                    throw std::runtime_error(
                        "Access modifiers (private/protected/public) are only allowed on constructor "
                        "('__alpha__') parameters.");
                }
                access = ctx.previous().lexeme;
            }

            // -------------------------------------------------
            // Parameter syntax: name: type
            // (same style as variable declarations)
            // -------------------------------------------------
            if (ctx.check(TokenType::IDENTIFIER))
            {
                ctx.advance();
            }
            else if (Keywords::isReservedWord(ctx.peek().lexeme))
            {
                std::string paramName = ctx.peek().lexeme;
                throw std::runtime_error(Keywords::reservedWordMsg(paramName, "parameter"));
            }
            else
            {
                throw std::runtime_error("Expected parameter name in method declaration.");
            }

            std::string name = ctx.previous().lexeme;
            if (Keywords::isReservedWord(name))
            {
                throw std::runtime_error(Keywords::reservedWordMsg(name, "parameter"));
            }

            if (!ctx.match(TokenType::COLON))
                throw std::runtime_error("Expected ':' after parameter name '" + name + "' in method declaration.");

            std::shared_ptr<CreationType> type;
            if (ctx.match(TokenType::ITSELF))
            {
                type = ctx.currentClassType();
                if (!type)
                    throw std::runtime_error("'itself' can only be used as a type inside a class definition.");
            }
            else if (ctx.check(TokenType::IDENTIFIER))
            {
                ctx.advance();
            }
            else if (Keywords::isReservedWord(ctx.peek().lexeme))
            {
                std::string typeStr = ctx.peek().lexeme;
                throw std::runtime_error(Keywords::reservedWordMsg(typeStr, "type"));
            }
            else
            {
                throw std::runtime_error("Expected parameter type after ':' in method declaration.");
            }

            if (!type)
            {
                std::string typeStr = ctx.previous().lexeme;
                type = KnownTypes::resolve(typeStr, "core");
                if (!type)
                    type = ctx.resolveType(typeStr);

                if (!type)
                {
                    throw std::runtime_error("Unknown param type: '" + typeStr + "'.");
                }
            }

            params->createVar(type, name, Value(1), isParam); // FIXME: Validate `type` and allow initial values

            // -------------------------------------------------
            // Optional default value: engine: Engine = Engine()
            // -------------------------------------------------
            std::shared_ptr<Expr> defaultValue = nullptr;
            if (ctx.match(TokenType::EQUAL))
            {
                defaultValue = grammar::Expression->parse(ctx);
                if (!defaultValue)
                    throw std::runtime_error("Expected a default value after '=' for parameter '" + name + "'.");
            }

            defaultValues.push_back(std::move(defaultValue));

            if (!access.empty())
                attributeNames.push_back({name, access});

        } while (ctx.match(TokenType::COMMA));
    }

    // ----------------------------------
    // Allowing params in multiple lines.
    // ----------------------------------
    ctx.consumeAllNewLines();
    if (ctx.isAtEnd())
    {
        ctx.popScope();
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::RIGHT_PAREN))
        throw std::runtime_error("Expected ')' after parameter list in method declaration.");

    // ----------------------------------
    // Hidden '$self' variable ("I"/"my")
    // ----------------------------------
    // 'my name' / 'I like' inside the method body refer to the object the
    // method runs on. It lives as a hidden local (never a parameter, so it is
    // not bound by call arguments) so member lookups work at parse time.
    auto selfType = ctx.currentClassType();
    if (selfType)
        params->declareVar(selfType, SELF_VARIABLE, /*isParam=*/false);

    // -----------
    // Return type
    // -----------
    std::shared_ptr<CreationType> returnType = KnownTypes::VOID; // default: nothing
    if (isDestructor)
    {
        if (ctx.match(TokenType::ARROW))
            throw std::runtime_error("Destructor '__omega__' cannot have a return type.");
    }
    else if (ctx.match(TokenType::ARROW))
    {
        if (ctx.match(TokenType::ITSELF))
        {
            returnType = ctx.currentClassType();
            if (!returnType)
                throw std::runtime_error("'itself' can only be used as a type inside a class definition.");
        }
        else if (ctx.check(TokenType::IDENTIFIER))
        {
            ctx.advance();
        }
        else if (Keywords::isReservedWord(ctx.peek().lexeme))
        {
            std::string typeName = ctx.peek().lexeme;
            throw std::runtime_error(Keywords::reservedWordMsg(typeName, "type"));
        }
        else
        {
            throw std::runtime_error("Expected return type after '->'.");
        }

        if (!returnType || returnType->isVoid())
        {
            std::string typeName = ctx.previous().lexeme;
            returnType = KnownTypes::resolve(typeName, "core");
            if (!returnType)
                returnType = ctx.resolveType(typeName);

            if (!returnType)
            {
                throw std::runtime_error("Unknown return type: '" + typeName + "'.");
            }
        }
    }

    // -------------------------------------------------------------------
    // Build the AST node NOW — with an empty body — and make the method
    // known to the class right away, before its body is parsed. A method
    // can therefore call itself (recursion) and any sibling method at
    // parse time, exactly like any other call.
    // -------------------------------------------------------------------
    auto stmt = std::make_unique<CreateMethodStmt>(
        methodName,
        params,
        returnType,
        std::vector<std::unique_ptr<Stmt>>{},
        isConstructor,
        isDestructor,
        attributeNames);

    stmt->defaultValues = std::move(defaultValues);

    addMethodToClass(ctx, stmt.get());

    // -----------
    // Method body
    // -----------
    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error("Expected ':' after method signature.");

    // ------------------------------------------------------------------
    // Dependency Inversion: '= SomeClass()' must not appear inside a
    // method/constructor/destructor body. Dependencies are injected through
    // parameters (or provided as parameter default values), so the object is
    // never created by the class that consumes it.
    // ------------------------------------------------------------------
    EnforceDependencyInversion guard(ctx, methodName);

    std::vector<std::unique_ptr<Stmt>> body;

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
        else if (auto stmt = assign->parse(ctx))
        {
            body.push_back(std::move(stmt));
        }
        else if (auto stmt = grammar::IfStmt->parse(ctx))
            body.push_back(std::move(stmt));
        else if (ctx.match(TokenType::RETURN))
        {
            if (isDestructor)
                throw std::runtime_error("Destructor '__omega__' cannot contain a return statement.");

            std::unique_ptr<Expr> returnExpr = nullptr;

            if (!ctx.check(TokenType::NEWLINE) && !ctx.check(TokenType::AMEN))
            {
                // Parse the expression after 'return'
                returnExpr = grammar::Expression->parse(ctx);
            }

            body.push_back(std::make_unique<ReturnStmt>(std::move(returnExpr)));
        }
        else
        {
            // ----------------------------------------------------------------
            // Bare expression statement (method call / member read) written in
            // natural English, e.g.:
            //   I love myself
            //   you love me
            //   my title
            // ----------------------------------------------------------------
            int snapshot = ctx.snapshot();
            auto expr = grammar::Expression->parse(ctx);
            if (!expr)
            {
                ctx.restore(snapshot);
                throw std::runtime_error("Unexpected statement inside method body.");
            }
            body.push_back(std::make_unique<PrintStmt>(StmtType::WARN, std::move(expr)));
        }

        ctx.consumeAllNewLines();
    }

    if (ctx.isAtEnd())
    {
        ctx.popScope(); // </🟢️>
        // FIXME: Each time an IncompleteBlockStmt is returned, all code is parsed again. Too expensive.
        return std::make_unique<IncompleteBlockStmt>();
    }

    if (!ctx.match(TokenType::AMEN))
    {
        ctx.popScope(); // </🟢️>
        throw std::runtime_error("Expected 'amen' to close method body.");
    }

    // ---------------------------
    // Enforce correct return type
    // ---------------------------
    try
    {
        if (!isDestructor)
        {
            for (const auto &stmt : body)
            {
                if (auto ret = dynamic_cast<ReturnStmt *>(stmt.get()))
                {
                    auto actualType = ret->getReturnType(ctx);

                    if (!returnType->isCompatibleWith(actualType))
                    {
                        throw std::runtime_error(
                            "Type mismatch in method '" + methodName + "': expected return type '" +
                            returnType->toString() + "', but found '" + actualType->toString() + "'.");
                    }
                }
            }

            // -----------------------------
            // Enforce explicit return rules
            // -----------------------------
            if (!returnType->isVoid())
            {
                if (body.empty() || !alwaysReturns(body.back().get()))
                {
                    throw std::runtime_error(
                        "Method '" + methodName + "' with return type '" + returnType->toString() +
                        "' must end with an explicit return.");
                }
            }
        }
    }
    catch (...)
    {
        ctx.popScope(); // </🟢️>
        throw;
    }

    ctx.popScope(); // </🟢️>

    stmt->body = std::move(body);
    return std::move(stmt);
}
