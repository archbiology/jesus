#include "get_attr_rule.hpp"
#include "ast/expr/get_attr_expr.hpp"
#include "ast/expr/method_call_expr.hpp"
#include "ast/expr/index_expr.hpp"
#include "ast/expr/enum_member_expr.hpp"
#include "types/creation_type.hpp"
#include "types/known_types.hpp"
#include "types/composite/dict_type.hpp"
#include "parser/helpers/member.hpp"
#include "parser/grammar/jesus_grammar.hpp"
#include "parser/grammar/argument_binding.hpp"
#include "../still_parsing_multiline_arguments.hpp"
#include "interpreter/runtime/method.hpp"
#include <memory>
#include <optional>

std::unique_ptr<Expr> GetAttributeRule::parse(ParserContext &ctx)
{
    // Start with a primary expression to get the 'instance/object'
    auto expr = primary->parse(ctx);
    if (!expr)
        return nullptr;

    // -------------------------------------------
    // Parse postfix operations (chainable)
    // Primary → Postfix → Postfix → Postfix → ...
    // -------------------------------------------
    while (true)
    {
        // =========================
        // 1. LIST INDEX: expr[...]
        // =========================
        if (ctx.match(TokenType::LEFT_BRACKET))
        {
            auto indexExpr = grammar::Expression->parse(ctx);

            if (!indexExpr)
                throw std::runtime_error("Expected a value inside []. Examples: numbers[0], person['name']");

            if (!ctx.match(TokenType::RIGHT_BRACKET))
                throw std::runtime_error("Expected ']' after index.");

            auto collectionType = expr->getReturnType(ctx);
            if (!collectionType->isA(KnownTypes::LIST) && !collectionType->isA(KnownTypes::DICT))
                throw std::runtime_error("Type '" + collectionType->name + "' does not support [ ].");

            auto accessType = indexExpr->getReturnType(ctx);

            if (collectionType->isA(KnownTypes::LIST))
            {
                if (!accessType->isA(KnownTypes::INT))
                    throw std::runtime_error("List indexes must be integers. Got '" + accessType->name + "' instead.");
            }
            else if (auto dictType = std::dynamic_pointer_cast<DictType>(collectionType))
            {
                if (!accessType->isA(dictType->keyType))
                {
                    throw std::runtime_error(
                        "This dict expects keys of type '" + dictType->keyType->name + "'. Got '" + accessType->name + "' instead.");
                }
            }

            expr = std::make_unique<IndexExpr>(std::move(expr), std::move(indexExpr));

            continue;
        }

        // =============================
        // 2. ATTRIBUTE / METHOD ACCESS
        // =============================
        if (ctx.check(TokenType::ALPHA))
        {
            throw std::runtime_error("Constructor '__alpha__' cannot be called directly.");
        }

        if (ctx.check(TokenType::OMEGA))
        {
            throw std::runtime_error("Destructor '__omega__' cannot be called directly.");
        }

        if (ctx.match(TokenType::IDENTIFIER))
        {
            // ----------------------------
            // Resolve attribute or method
            // ----------------------------
            std::shared_ptr<CreationType> klass = expr->getReturnType(ctx);
            std::string name = ctx.previous().lexeme;

            // -----------------------------------------------
            // Enum member access: EnumName MEMBER
            // If the base expression's type is an enum and the
            // attribute matches a member, create an EnumMemberExpr.
            // -----------------------------------------------
            if (klass && klass->isEnum())
            {
                auto member = klass->findMember(name, klass);
                if (member && member->isAttribute())
                {
                    auto addr = member->declaring_class->class_attributes->resolveVariableAddress(member->attr_name);
                    int value = member->declaring_class->class_attributes->getVar(addr).toInt();

                    // Read the label if one is stored; otherwise it defaults to the member name
                    std::string label = name;
                    std::string labelKey = "label_" + name;
                    if (member->declaring_class->class_attributes->localVarExists(labelKey))
                    {
                        auto labelAddr = member->declaring_class->class_attributes->resolveVariableAddress(labelKey);
                        label = member->declaring_class->class_attributes->getVar(labelAddr).toString();
                    }

                    expr = std::make_unique<EnumMemberExpr>(klass, name, label, value);
                    continue;
                }
            }

            // -----------------------------------------------
            // .label / .value on enums:
            //   Status Pending label    Status Pending value
            //   e label                 e value     (e holds an enum)
            // The runtime decides the concrete result based on the
            // evaluated enum instance.
            // -----------------------------------------------
            if (name == "label" || name == "value")
            {
                if (klass && klass->isEnum())
                {
                    VariableAddress dummyAddress{0, 0};
                    expr = std::make_unique<GetAttributeExpr>(std::move(expr), name, dummyAddress);
                    continue;
                }
            }

            auto member = klass->findMember(name, klass);
            if (!member)
            {
                throw std::runtime_error("Unknown member '" + name + "' in class " + klass->name);
            }

            // -----------------
            // ATTRIBUTE ACCESS
            // -----------------
            if (member->isAttribute())
            {
                auto address = member->declaring_class->class_attributes->resolveVariableAddressInHierarchy(name);
                expr = std::make_unique<GetAttributeExpr>(std::move(expr), name, address);
            }

            // ------------
            // METHOD CALL
            // ------------
            else if (member->isMethod())
            {
                std::vector<grammar::Argument> rawArgs;

                // If the next token(s) indicate arguments, parse them
                if (!ctx.check(TokenType::NEWLINE) && !ctx.check(TokenType::END_OF_FILE))
                {
                    int argsSnapshot = ctx.snapshot();
                    auto expectedParams = member->method->params->paramsCount;
                    if (expectedParams > 0)
                        do
                        {
                            // ---------------------------
                            // Arguments in multiple lines
                            // ---------------------------
                            ctx.consumeAllNewLines();
                            if (ctx.isAtEnd())
                            {
                                ctx.restore(argsSnapshot);
                                throw StillParsingMultilineArgumentsSignal{};
                            }

                            // -------------------------------------
                            // Detect a named argument: name='Jesus'
                            // -------------------------------------
                            std::optional<std::string> argName;
                            if (ctx.check(TokenType::IDENTIFIER))
                            {
                                int snap = ctx.snapshot();
                                const Token &token = ctx.advance();
                                if (ctx.check(TokenType::EQUAL))
                                {
                                    ctx.advance(); // consume the '='
                                    argName = token.lexeme;
                                }
                                else
                                {
                                    ctx.restore(snap);
                                }
                            }

                            auto argExpr = primary->parse(ctx); // parse any expression

                            if (!argExpr)
                                throw std::runtime_error("Expected argument for method " + name);

                            rawArgs.push_back({argName, std::move(argExpr)});

                        } while (ctx.match(TokenType::COMMA));
                }

                auto args = grammar::bindArgumentsToParameters(
                    std::move(rawArgs),
                    member->method->params->getParameterNames(),
                    member->method->params,
                    "Method",
                    name);

                expr = std::make_unique<MethodCallExpr>(std::move(expr), member->method, std::move(args), ctx.interpreter);
                expr->validate(ctx);
            }
            else
            {
                throw std::runtime_error("Unknown member '" + name + "' in class " + klass->name);
            }

            continue;
        }

        // --------------------
        // No more postfix ops
        // --------------------
        break;
    }
    return expr;
}

std::string GetAttributeRule::toStr(GrammarRuleHashTable &visited) const
{
    if (visited.count(this))
        return "GetAttributeExprRule(...)";

    visited.insert(this);

    return "GetAttributeExprRule()";
}
