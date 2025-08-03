
#pragma once

#include <atomic>
#include "expr_visitor.hpp"
#include "stmt_visitor.hpp"
#include "../ast/expr/expr.hpp"
#include "../ast/expr/binary_expr.hpp"
#include "../ast/expr/conditional_expr.hpp"
#include "../ast/expr/unary_expr.hpp"
#include "../ast/expr/literal_expr.hpp"
#include "../ast/expr/variable_expr.hpp"
#include "../ast/expr/grouping_expr.hpp"
#include "../ast/stmt/stmt.hpp"
#include "../ast/stmt/create_var_type_stmt.hpp"
#include "../ast/stmt/create_var_stmt.hpp"
#include "../ast/stmt/update_var_stmt.hpp"
#include "../ast/stmt/output_statement.hpp"
#include "../ast/stmt/repeat_while_stmt.hpp"
#include "../ast/stmt/repeat_times_stmt.hpp"
#include "../ast/stmt/for_each_stmt.hpp"
#include "../ast/stmt/break_stmt.hpp"
#include "../ast/stmt/continue_stmt.hpp"
#include "../spirit/heart.hpp"

/**
 * @brief Interprets expressions of the language and evaluates their result.
 *
 * "But it is the spirit in a person, the breath of the Almighty, that gives them understanding."
 * — Job 32:8
 *
 * This class interprets user-defined expressions and produces runtime values,
 * though true understanding and interpretation come only from God.
 *
 * AST nodes hold structure, Interpreter holds behavior.
 *
 * 🌟 Why This Helps for Future JIT & Debugging
 *
 *   Want to debug? Wrap the Interpreter::execute() methods with trace logs or breakpoints.
 *   Want to switch to JIT? Swap Interpreter with a JITCompiler.
 *   Want to record execution? Just add hooks in Interpreter.
 */
class Interpreter : public ExprVisitor, public StmtVisitor
{
public:
    explicit Interpreter(Heart *heart) : heart(*heart)
    {
        Interpreter::instance = this;
        setSignalHandler();
    }

    /**
     * @brief Set to false when user cancels (E.g.: pressing CTRL+C or CTRL+D)
     */
    std::atomic<bool> keep_running = true;
    static Interpreter *instance; // to be set when constructed. Not creating the variable, just telling the compile that it exists;

    /**
     * @brief Evaluates a given expression and returns its computed value.
     *
     * "Give me understanding, so that I may keep your law and obey it with all my heart."
     * — Psalm 119:34
     */
    Value evaluate(const std::unique_ptr<Expr> &expr);

    void createVariable(const std::string &name, const Value &value);
    void updateVariable(const std::string &name, const Value &value);

    /**
     * @brief Executes a single statement node in the abstract syntax tree (AST).
     *
     * This method dynamically dispatches the runtime behavior of a statement,
     * such as variable assignment, loop execution, or output operations.
     * It delegates control to the appropriate visitor method
     * (visitBinary, visitRepeatWhile, etc) based on the dynamic type of the
     * statement.
     *
     * Statements are side-effect-driven instructions and do not produce
     * direct values like expressions do. Therefore, this function returns void.
     *
     * Example usage:
     * @code
     *  std::unique_ptr<Stmt> stmt = std::make_unique<ForEachStmt>(...);
     *  interpreter.execute(stmt);
     * @endcode
     *
     * "Do not merely listen to the word, and so deceive yourselves. Do what it says."
     * — James 1:22
     *
     * Just as we are called not only to hear God's Word but to act on it,
     * this method does not evaluate the statement intellectually — it **executes**
     * it, causing the program to act.
     */
    void execute(const std::unique_ptr<Stmt> &stmt);

private:
    static void signalHandler(int signum);
    static void setSignalHandler();

    /**
     * @brief It acts like a symbol table — each variable has a name (string)
     *
     * “The good man brings good things out of the good stored up in his heart…” — Luke 6:45
     */
    Heart heart;

    // 🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️
    // 🟢️ Visit expression methods 🟢️
    // 🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️

    /**
     * @brief Evaluates a binary expression such as addition, comparison, etc.
     *
     * “But you must not eat from the tree of the knowledge of good and evil, for when you eat from it you will certainly die.”
     * — Genesis 2:17
     *
     * good + evil = deadly knowledge
     */
    Value visitBinary(const BinaryExpr &expr) override;

    /**
     * @brief Evaluates a unary expression like negation or logical NOT.
     *
     * “I and the Father are one.” — John 10:30
     */
    Value visitUnary(const UnaryExpr &expr) override;

    /**
     * @brief Evaluates a literal value like a number, string, or boolean.
     *
     * “And God said, ‘Let there be light,’ and there was light.”
     * — Genesis 1:3
     */
    Value visitLiteral(const LiteralExpr &expr) override;

    /**
     * @brief Resolves the value of a variable.
     *
     * “When the Lord saw that he had gone over to look, God called to him from within the bush, ‘Moses! Moses!’
     * And Moses said, ‘Here I am.’”
     * — Exodus 3:4–5
     */
    Value visitVariable(const VariableExpr &expr) override;

    /**
     * @brief Evaluates an AskExpr by evaluating its prompt expression.
     *
     * It processes the 'ask' expression node in the AST,
     * evaluating the prompt (which can be a string literal or a variable
     * that resolves to text) to produce the question string that will be
     * presented to the user.
     *
     * The returned Value represents the prompt message, ready to be
     * displayed or used for user input collection.
     *
     * "Ask, and it will be given to you..." — Matthew 7:7
     *
     * @return Value The evaluated prompt as a Value object (typically text).
     */
    Value visitAsk(const AskExpr &expr) override;

    /**
     * @brief Evaluates a grouped expression (expressions in parentheses).
     *
     * “When the day of Pentecost came, they were all together in one place.
     * Suddenly a sound like the blowing of a violent wind came from heaven and filled the whole house where they were sitting.
     * They saw what seemed to be tongues of fire that separated and came to rest on each of them.
     * All of them were filled with the Holy Spirit and began to speak in other tongues as the Spirit enabled them.”
     * — Acts 2:1–4
     */
    Value visitGrouping(const GroupingExpr &expr) override;

    Value visitConditional(const ConditionalExpr &expr) override;

    /**
     * @brief Converts a runtime value into a string representation.
     *
     * “For there is nothing hidden that will not be disclosed, and nothing concealed that will not be known or brought out into the open.”
     * — Luke 8:17
     */
    std::string valueToString(const Value &value);

    // 🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️
    // 🟢️ Visit statement methods 🟢️
    // 🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️🟢️

    void visitCreateVar(const CreateVarStmt &stmt) override;

    /**
     * @brief Handles the creation of a variable initialized via an 'ask' expression.
     *
     * This method drives the interactive flow of variable creation when
     * the initialization expression is an 'ask' prompt. It:
     * - Evaluates the prompt to obtain the question.
     * - Repeatedly asks the user for input until the provided value
     *   passes the CreationType's validation constraints.
     * - Creates the variable in the runtime environment with the validated value.
     *
     * This enables type-safe variable initialization with built-in validation.
     */
    void visitCreateVarWithAsk(const CreateVarWithAskStmt &stmt) override;

    void visitCreateVarType(const CreateVarTypeStmt &stmt) override;

    void visitUpdateVar(const UpdateVarStmt &stmt) override;

    void visitOutput(const OutputStmt &stmt) override;

    /**
     * repeat while condition:
     *   say "Still going..."
     */
    void visitRepeatWhile(const RepeatWhileStmt &stmt) override;

    /**
     * repeat 3 times:
     *   say "Jesus is Lord!"
     */
    void visitRepeatTimes(const RepeatTimesStmt &stmt) override;

    /**
     * set disciples to ["Peter", "James", "John"]
     * for each name in disciples:
     *    say name
     */
    void visitForEach(const ForEachStmt &stmt) override;

    /**
     * set disciples to ["Peter", "James", "John"]
     * for each name in disciples:
     *    if name == James:
     *      break # or stop
     *
     *    say name
     */
    void visitBreak(const BreakStmt &stmt) override;

    /**
     * set disciples to ["Peter", "James", "John"]
     * for each name in disciples:
     *    if name == James:
     *      continue # or next; skip;
     *
     *    say name
     */
    void visitContinue(const ContinueStmt &stmt) override;
};
