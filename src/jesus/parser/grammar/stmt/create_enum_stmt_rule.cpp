#include "create_enum_stmt_rule.hpp"
#include "ast/stmt/create_enum_stmt.hpp"
#include "types/known_types.hpp"
#include "spirit/heart.hpp"
#include "interpreter/runtime/instance.hpp"
#include <stdexcept>
#include <vector>
#include <string>
#include <optional>

struct EnumMemberInfo
{
    std::string name;
    std::optional<std::string> explicitLabel;
    std::optional<int> explicitValue;
    int value;
    std::string label;
};

std::unique_ptr<Stmt> CreateEnumStmtRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::ENUM))
        return nullptr;

    if (!ctx.check(TokenType::IDENTIFIER))
        throw std::runtime_error("Expected en enum name after 'enum'.\n\nExample:\n  enum Status: ... amen");

    ctx.advance();
    std::string enumName = ctx.previous().lexeme;

    if (!ctx.match(TokenType::COLON))
        throw std::runtime_error(
            "Expected ':' after enum name '" + enumName + "'.\n\nExample:\n enum " + enumName + ": ... amen");

    ctx.consumeAllNewLines();

    // -------------
    // Parse members
    // -------------
    std::vector<EnumMemberInfo> members;

    while (!ctx.check(TokenType::AMEN) && !ctx.isAtEnd())
    {
        EnumMemberInfo member;

        // -----------
        // Member name
        // -----------
        if (!ctx.check(TokenType::IDENTIFIER))
            throw std::runtime_error(
                "Expected an member name in enum '" + enumName + "'.\n\n Example:\n  enum " + enumName +
                ":\n    Pending\n  amen");

        ctx.advance();
        member.name = ctx.previous().lexeme;

        // ---------------------------------------------------------
        // Optional arguments:
        //   MEMBER
        //   MEMBER('label')
        //   MEMBER(value)
        //   MEMBER(value, 'label')
        // ---------------------------------------------------------
        if (ctx.match(TokenType::LEFT_PAREN))
        {
            // --------------
            // First argument
            // --------------
            if (ctx.match(TokenType::RAW_STRING))
            {
                member.explicitLabel = ctx.previous().literal.toString();
            }
            else if (ctx.match(TokenType::INT))
            {
                member.explicitValue = std::stoi(ctx.previous().lexeme);
            }
            else
            {
                throw std::runtime_error(
                    "Expected a string label or numeric value inside parentheses for enum member '" + member.name +
                    "' in enum '" + enumName + "'.\n\nExamples:\n"
                    "  " + member.name + "\n"
                    "  " + member.name + "('Waiting')\n"
                    "  " + member.name + "(7)\n"
                    "  " + member.name + "(7, 'Waiting')\n"
                );
            }

            // ------------------------------------------------
            // Optional second argument: MEMBER(value, 'label')
            // ------------------------------------------------
            if (ctx.match(TokenType::COMMA))
            {
                if (!member.explicitValue.has_value())
                    throw std::runtime_error(
                        "Enum member '" + member.name + "' in enum '" + enumName +
                        "' must specify the value first when providing value and label together.\n\n"
                        " Example:\n  " +
                        member.name + "(value, 'label').");

                if (!ctx.match(TokenType::RAW_STRING))
                    throw std::runtime_error(
                        "Expected a string label as the second argument of enum member '" + member.name +
                        "' in enum '" + enumName + "'.\n\nExample:\n  " + member.name + "(123, 'label')");

                member.explicitLabel = ctx.previous().literal.toString();
            }

            if (!ctx.match(TokenType::RIGHT_PAREN))
                throw std::runtime_error(
                    "Expected ')' after arguments in enum member '" + member.name + "' in enum '" + enumName +
                    "'.\n\nExample:\n  " + member.name + "(123, 'label')");
        }

        // -------------------------
        // Resolve the numeric value
        // -------------------------
        if (member.explicitValue.has_value())
        {
            if (member.explicitValue.value() < 0)
                throw std::runtime_error(
                    "Enum member value must be non-negative. Got " + std::to_string(member.explicitValue.value()) +
                    ".\n\nExample:\n  RED(" + std::to_string(-member.explicitValue.value()) + ").");

            member.value = member.explicitValue.value();
        }
        else if (!members.empty())
        {
            member.value = members.back().value + 1;
        }
        else
        {
            member.value = 0;
        }

        // ------------------------------------------------------
        // Resolve the label (explicit label, or the member name)
        // ------------------------------------------------------
        member.label = member.explicitLabel.value_or(member.name);

        members.push_back(std::move(member));

        ctx.consumeAllNewLines();
    }

    if (members.empty())
        throw std::runtime_error(
            "Enum '" + enumName + "' must have at least one member.\n\nExample:\n  enum " + enumName +
            ":\n    Pending\n  amen");

    // --------------------
    // Create the enum type
    // --------------------
    auto classAttributes = std::make_shared<Heart>("enum:" + enumName);
    auto enumType = std::make_shared<CreationType>(
        PrimitiveType::Enum,
        enumName,
        ctx.moduleName,
        nullptr,
        classAttributes,
        std::vector<std::shared_ptr<IConstraint>>{});

    // ---------------------------------------------------------------------------
    // Register members as class attributes (the attribute holds the member value)
    // ---------------------------------------------------------------------------
    for (const auto &member : members)
    {
        const bool isParam = false;
        classAttributes->createVar(KnownTypes::INT, member.name, Value(member.value), isParam);
    }

    // ----------------------------------------------
    // Store each member's label as a class attribute
    // ----------------------------------------------
    for (const auto &member : members)
    {
        const bool isParam = false;
        classAttributes->createVar(KnownTypes::STRING, "label_" + member.name, Value(member.label), isParam);
    }

    // -------------------
    // Close the enum body
    // -------------------
    if (!ctx.match(TokenType::AMEN))
        throw std::runtime_error(
            "Expected 'amen' to close enum '" + enumName + "'.\n\nExample:\n  enum " + enumName +
            ":\n    Pending\n  amen");

    // ------------------------------------------------------------------
    // Register type and create a global variable holding an EnumInstance
    // ------------------------------------------------------------------
    ctx.registerType(enumType);
    ctx.registerClassName(enumName);
    ctx.declareVar(enumType, enumName);

    return std::make_unique<CreateEnumStmt>(enumName, ctx.moduleName, std::move(enumType));
}
