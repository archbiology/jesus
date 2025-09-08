
#pragma once

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
#include "../ast/stmt/print_stmt.hpp"
#include "../ast/stmt/repeat_while_stmt.hpp"
#include "../ast/stmt/repeat_times_stmt.hpp"
#include "../ast/stmt/for_each_stmt.hpp"
#include "../ast/stmt/break_stmt.hpp"
#include "../ast/stmt/continue_stmt.hpp"
#include "../spirit/heart.hpp"
#include "../spirit/symbol_table.hpp"

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
    explicit Interpreter(SymbolTable &symbol_table) : symbol_table(symbol_table) {}
    /**
     * @brief Evaluates a given expression and returns its computed value.
     *
     * "Give me understanding, so that I may keep your law and obey it with all my heart."
     * — Psalm 119:34
     */
    Value evaluate(const std::unique_ptr<Expr> &expr);

    void createVariable(const std::string &type, const std::string &name, const Value &value);
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
    void execute(const std::shared_ptr<Stmt> &stmt);
    inline void loves(const std::unique_ptr<Stmt> &stmt) { execute(stmt); }

    std::shared_ptr<CreationType> getVarType(const std::string &varName)
    {
        return symbol_table.getVarType(varName);
    }

    void registerVarType(const std::string &type, const std::string &name)
    {
        symbol_table.registerVarType(type, name);
    }

    void registerClassName(const std::string &className)
    {
        symbol_table.registerClassName(className);
    }

    void addScope(std::shared_ptr<Heart> scope)
    {
        symbol_table.addScope(scope);
    }

    void popScope()
    {
        symbol_table.popScope();
    }

private:
    /**
     * @brief It acts like a symbol table — each variable has a name (string)
     *
     * “The good man brings good things out of the good stored up in his heart…” — Luke 6:45
     */
    SymbolTable symbol_table;

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

    Value visitCreateInstanceExpr(const CreateInstanceExpr &expr) override;

    /**
     * @brief Resolves the value of an attribute.
     *
     * "Likewise, every good tree bears good fruit, but a bad tree bears bad fruit.”
     * — Matthew 7:17
     */
    Value visitGetAttribute(const GetAttributeExpr &expr) override;

    Value visitMethodCallExpr(const MethodCallExpr &expr) override;

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
     * @brief Asks for a value and validate it.
     *
     * This method drives the interactive flow of variable creation/update when
     * the expression is an 'ask' prompt. It:
     * - Evaluates the prompt to obtain the question.
     * - Repeatedly asks the user for input until the provided value
     *   passes the CreationType's validation constraints.
     * - Return the validated value
     *
     * This enables type-safe variable assignment with built-in validation.
     */
    Value askAndValidate(const std::shared_ptr<Expr> ask_expr, std::shared_ptr<CreationType> var_type);

    /**
     * @brief Handles the creation of a variable initialized via an 'ask' expression.
     *
     * * E.g.: create text name = ask "What is your name? "
     */
    void visitCreateVarWithAsk(const CreateVarWithAskStmt &stmt) override;

    void visitCreateClass(const CreateClassStmt &stmt) override;

    void visitCreateVarType(const CreateVarTypeStmt &stmt) override;

    void visitUpdateVar(const UpdateVarStmt &stmt) override;

    /**
     * @brief Handles the update of a variable using 'ask' expression.
     *
     * E.g.: age = ask "What is your age again?"
     */
    void visitUpdateVarWithAsk(const UpdateVarWithAskStmt &stmt) override;

    void visitPrintStmt(const PrintStmt &stmt) override;

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
