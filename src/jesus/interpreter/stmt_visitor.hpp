#pragma once

#include "../ast/stmt/create_class_stmt.hpp"
#include "../ast/stmt/create_var_type_stmt.hpp"
#include "../ast/stmt/create_var_stmt.hpp"
#include "../ast/stmt/update_var_stmt.hpp"
#include "../ast/stmt/create_var_with_ask_stmt.hpp"
#include "../ast/stmt/update_var_with_ask_stmt.hpp"
#include "../ast/stmt/print_stmt.hpp"
#include "../ast/stmt/repeat_while_stmt.hpp"
#include "../ast/stmt/repeat_times_stmt.hpp"
#include "../ast/stmt/repeat_forever_stmt.hpp"
#include "../ast/stmt/for_each_stmt.hpp"
#include "../ast/stmt/break_stmt.hpp"
#include "../ast/stmt/skip_stmt.hpp"
#include "../ast/stmt/return_stmt.hpp"
#include "../ast/stmt/if_stmt.hpp"
#include "../ast/stmt/try_stmt.hpp"
#include "../ast/stmt/resist_stmt.hpp"

/**
 * @brief Interface for visiting and executing statement nodes in the AST.
 *
 * The StmtVisitor defines a set of methods that must be implemented
 * by any class that wants to interpret statement types.
 *
 * This allows the interpreter (or a debugger, tracer, etc.) to simplify
 * its execute method by calling stmt->accept(visitor) instead of
 * manually checking the statement type with conditionals or dynamic casts.
 *
 * Usage example:
 *
 *     class Interpreter : public StmtVisitor {
 *         void visitCreateVar(const CreateVarStmt& stmt) override;
 *         void visitOutput(const OutputStmt& stmt) override;
 *         // ...
 *     };
 *
 *     std::unique_ptr<Stmt> statement = std::make_unique<OutputStmt>(...);
 *     Interpreter interpreter;
 *     interpreter.execute(statement);
 *
 * Currently being called in the `execute` method of Interpreter:
 *     void Interpreter::execute(const std::unique_ptr<Stmt> &stmt) {
 *         stmt->accept(*this);
 *     }
 *
 * ---
 * "Do not forget to show hospitality to strangers, for by so doing
 * some people have shown hospitality to angels without knowing it."
 * — Hebrews 13:2
 */
class StmtVisitor
{
public:
    virtual void visitCreateClass(const CreateClassStmt &stmt) = 0;
    virtual void visitCreateVarType(const CreateVarTypeStmt &stmt) = 0;
    virtual void visitCreateVar(const CreateVarStmt &stmt) = 0;
    virtual void visitCreateVarWithAsk(const CreateVarWithAskStmt &stmt) = 0;
    virtual void visitUpdateVar(const UpdateVarStmt &stmt) = 0;
    virtual void visitUpdateVarWithAsk(const UpdateVarWithAskStmt &stmt) = 0;
    virtual void visitPrintStmt(const PrintStmt &stmt) = 0;
    virtual void visitRepeatWhile(const RepeatWhileStmt &stmt) = 0;
    virtual void visitRepeatTimes(const RepeatTimesStmt &stmt) = 0;
    virtual void visitRepeatForeverStmt(const RepeatForeverStmt &stmt) = 0;
    virtual void visitForEach(const ForEachStmt &stmt) = 0;
    virtual void visitBreak(const BreakStmt &stmt) = 0;
    virtual void visitSkipStmt(const SkipStmt &stmt) = 0;
    virtual void visitReturnStmt(const ReturnStmt &stmt) = 0;
    virtual void visitIfStmt(const IfStmt &stmt) = 0;
    virtual void visitTryStmt(const TryStmt &stmt) = 0;
    virtual void visitResistStmt(const ResistStmt &stmt) = 0;

    virtual ~StmtVisitor() = default;
};
