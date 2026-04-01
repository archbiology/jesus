#include "bible_expr_rule.hpp"
#include "../../../../ast/expr/bible_expr.hpp"
#include "../../../../types/known_types.hpp"
#include "../../../../types/known_types.hpp"
#include "../../../../cli/bible.hpp"

std::unique_ptr<Expr> BibleExprRule::parse(ParserContext &ctx)
{
    if (!ctx.match(TokenType::BIBLE))
        return nullptr;

    auto arg = expression->parse(ctx);

    if (!arg)
        throw std::runtime_error(
            "Expected a Bible reference after 'bible'.\n"
            "Example: bible 'john 3:16'");

    // FAIL-FAST if literal
    if (arg->canEvaluateAtParseTime())
    {
        auto empty_scope = std::make_shared<Heart>("bible_rule");
        Value literal = arg->evaluate(empty_scope);
        auto ref_str = literal.toString();

        // Validate it is a valid reference
        auto ref = BibleCLI::parseScripture(ref_str);
    }

    return std::make_unique<BibleExpr>(std::move(arg));
}
