#include "create_method_stmt_rule.hpp"
#include "../../../ast/stmt/create_method_stmt.hpp"
#include "../../../ast/stmt/incomplete_block_stmt.hpp"
#include "../../../ast/stmt/return_stmt.hpp"
#include "../../../types/creation_type.hpp"
#include "../../parser_context.hpp"
#include "../../../types/known_types.hpp"
#include "../../../spirit/heart.hpp"
#include "../../../understanding/doctrine/law/ungodly_naming.hpp"
#include "lexer/keywords.hpp"
#include "../jesus_grammar.hpp"
#include <stdexcept>

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

            if (ctx.check(TokenType::IDENTIFIER))
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

            std::string typeStr = ctx.previous().lexeme;
            auto type = KnownTypes::resolve(typeStr, "core");
            if (!type)
            {
                throw std::runtime_error("Unknown param type: '" + typeStr + "'.");
            }

            params->createVar(type, name, Value(1), isParam); // FIXME: Validate `type` and allow initial values

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
        if (ctx.check(TokenType::IDENTIFIER))
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
            throw std::runtime_error("Unexpected statement inside method body.");
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
                if (body.empty())
                {
                    throw std::runtime_error(
                        "Method '" + methodName + "' with return type '" + returnType->toString() +
                        "' must end with an explicit return.");
                }

                auto lastStmt = body.back().get();
                auto ret = dynamic_cast<ReturnStmt *>(lastStmt);
                if (!ret)
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

    return std::make_unique<CreateMethodStmt>(
        methodName, std::move(params), returnType, std::move(body), isConstructor, isDestructor, attributeNames);
}
