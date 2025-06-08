
#pragma once

#include "value.hpp"
#include "../ast/expr/expr.hpp"
#include "../ast/expr/binary_expr.hpp"
#include "../ast/expr/conditional_expr.hpp"
#include "../ast/expr/unary_expr.hpp"
#include "../ast/expr/literal_expr.hpp"
#include "../ast/expr/variable_expr.hpp"
#include "../ast/expr/grouping_expr.hpp"
#include "../ast/stmt/stmt.hpp"
#include "../ast/stmt/set_stmt.hpp"
#include "../ast/stmt/output_statement.hpp"
#include "../ast/stmt/repeat_while_stmt.hpp"
#include "../ast/stmt/repeat_times_stmt.hpp"
#include "../ast/stmt/for_each_stmt.hpp"
#include "../spirit/heart.hpp"

/**
 * @brief Interprets expressions of the language and evaluates their result.
 *
 * "But it is the spirit in a person, the breath of the Almighty, that gives them understanding."
 * — Job 32:8
 *
 * This class interprets user-defined expressions and produces runtime values,
 * though true understanding and interpretation come only from God.
 */
class Interpreter
{
public:
    /**
     * @brief Evaluates a given expression and returns its computed value.
     *
     * "Give me understanding, so that I may keep your law and obey it with all my heart."
     * — Psalm 119:34
     */
    Value evaluate(std::unique_ptr<Expr> &expr);

    void defineVariable(const std::string &name, const Value &value);
    void assignVariable(const std::string &name, const Value &value);
    void execute(std::unique_ptr<Stmt> &stmt);

private:
    /**
     * @brief It acts like a symbol table — each variable has a name (string)
     *
     * “The good man brings good things out of the good stored up in his heart…” — Luke 6:45
     */
    Heart heart;

    /**
     * @brief Evaluates a binary expression such as addition, comparison, etc.
     *
     * “But you must not eat from the tree of the knowledge of good and evil, for when you eat from it you will certainly die.”
     * — Genesis 2:17
     *
     * good + evil = deadly knowledge
     */
    Value visitBinary(BinaryExpr *expr);

    /**
     * @brief Evaluates a unary expression like negation or logical NOT.
     *
     * “I and the Father are one.” — John 10:30
     */
    Value visitUnary(UnaryExpr *expr);

    /**
     * @brief Evaluates a literal value like a number, string, or boolean.
     *
     * “And God said, ‘Let there be light,’ and there was light.”
     * — Genesis 1:3
     */
    Value visitLiteral(LiteralExpr *expr);

    /**
     * @brief Resolves the value of a variable.
     *
     * “When the Lord saw that he had gone over to look, God called to him from within the bush, ‘Moses! Moses!’
     * And Moses said, ‘Here I am.’”
     * — Exodus 3:4–5
     */
    Value visitVariable(VariableExpr *expr);

    /**
     * @brief Evaluates a grouped expression (expressions in parentheses).
     *
     * “When the day of Pentecost came, they were all together in one place.
     * Suddenly a sound like the blowing of a violent wind came from heaven and filled the whole house where they were sitting.
     * They saw what seemed to be tongues of fire that separated and came to rest on each of them.
     * All of them were filled with the Holy Spirit and began to speak in other tongues as the Spirit enabled them.”
     * — Acts 2:1–4
     */
    Value visitGrouping(GroupingExpr *expr);

    /**
     * @brief Determines whether a value should be considered "truthy".
     *
     * “Jesus said to him, ‘I am the way, and the truth, and the life. No one comes to the Father except through me.’”
     * — John 14:6
     */
    bool isTruthy(const Value &value);

    /**
     * @brief Converts a runtime value into a string representation.
     *
     * “For there is nothing hidden that will not be disclosed, and nothing concealed that will not be known or brought out into the open.”
     * — Luke 8:17
     */
    std::string valueToString(const Value &value);

    void visitSet(SetStmt *stmt);

    Value visitConditional(ConditionalExpr *expr);

    void visitOutput(OutputStmt *stmt);

    /**
     * repeat while condition:
     *   say "Still going..."
     */
    void visitRepeatWhile(RepeatWhileStmt *stmt);

    /**
     * repeat 3 times:
     *   say "Jesus is Lord!"
     */
    void visitRepeatTimes(RepeatTimesStmt *stmt);

    /**
     * set disciples to ["Peter", "James", "John"]
     * for each name in disciples:
     *    say name
     */
    void visitForEach(ForEachStmt *stmt);
};
