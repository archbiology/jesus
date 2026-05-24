#include "dict_rule.hpp"
#include "ast/expr/dict_expr.hpp"
#include "ast/stmt/incomplete_block_stmt.hpp"
#include "types/known_types.hpp"

std::unique_ptr<Expr> DictRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::LEFT_BRACE))
        return nullptr;

    std::vector<std::pair<std::unique_ptr<Expr>, std::unique_ptr<Expr>>> entries;

    std::shared_ptr<CreationType> keyType = nullptr;
    std::shared_ptr<CreationType> valueType = nullptr;

    ctx.consumeAllNewLines();

    if (!ctx.check(TokenType::RIGHT_BRACE))
    {
        do
        {
            ctx.consumeAllNewLines();

            auto key = expression->parse(ctx);
            if (!key)
                throw std::runtime_error("Expected dict key.");

            ctx.consumeAllNewLines();

            if (!ctx.match(TokenType::COLON))
                throw std::runtime_error("Expected ':' after dict key.");

            auto value = expression->parse(ctx);
            if (!value)
                throw std::runtime_error("Expected dict value.");

            auto currentKeyType = key->getReturnType(ctx);
            auto currentValueType = value->getReturnType(ctx);

            entries.push_back({std::move(key), std::move(value)});

            ctx.consumeAllNewLines();

            /* ------------------------------------------------------------
             * Dict key/value type inference
             * ------------------------------------------------------------
             *
             * Dicts infer both their key type and value type automatically
             * during parsing.
             *
             * Rules:
             *
             *   {}                             -> dict<creation, creation>
             *   {"name": "Jesus"}              -> dict<text, text>
             *   {"name": "Jesus", "age": 33}   -> dict<text, creation>
             *   {"a": 1, "b": 2}               -> dict<text, int>
             *   {1: "one", 2: "two"}           -> dict<int, text>
             *   {1: "one", "two": 2}           -> dict<creation, creation>
             *
             * Inference strategy:
             *
             * - The first entry defines the initial key and value types.
             *
             * - All subsequent entries are compared independently:
             *
             *      keyType   vs currentKeyType
             *      valueType vs currentValueType
             *
             * - If all keys share the same type, the dict keeps that key type.
             * - If all values share the same type, the dict keeps that value type.
             * - If a different key type appears, the key type falls back to `creation`.
             * - If a different value type appears, the value type falls back to `creation`.
             *
             * This allows Python/JavaScript-like dict syntax while preserving
             * strong typing whenever possible.
             * ------------------------------------------------------------
             */
            if (!keyType)
            {
                keyType = currentKeyType;
            }
            else if (keyType != KnownTypes::CREATION && currentKeyType != keyType)
            {
                keyType = KnownTypes::CREATION;
            }

            if (!valueType)
            {
                valueType = currentValueType;
            }
            else if (valueType != KnownTypes::CREATION && currentValueType != valueType)
            {
                valueType = KnownTypes::CREATION;
            }

        } while (ctx.match(TokenType::COMMA));
    }

    if (!ctx.match(TokenType::RIGHT_BRACE))
    {
        throw std::runtime_error("Expected '}' to close dict.");
    }

    // Empty dict defaults to creation
    if (!keyType)
        keyType = KnownTypes::CREATION;

    if (!valueType)
        valueType = KnownTypes::CREATION;

    auto dictType = KnownTypes::makeDictType(keyType, valueType);

    return std::make_unique<DictExpr>(std::move(entries), dictType);
}
