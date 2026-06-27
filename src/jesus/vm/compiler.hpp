#pragma once

#include "chunk.hpp"

#include "ast/expr/binary_expr.hpp"
#include "ast/expr/create_instance_expr.hpp"
#include "ast/expr/literal_expr.hpp"
#include "ast/expr/variable_expr.hpp"

#include "ast/stmt/create_var_stmt.hpp"
#include "ast/stmt/create_class_stmt.hpp"
#include "ast/stmt/update_var_stmt.hpp"
#include "ast/stmt/print_stmt.hpp"
#include "ast/stmt/repeat_while_stmt.hpp"

/**
 * @brief Converts an Abstract Syntax Tree (AST) into bytecode.
 *
 * The Compiler is responsible for transforming parsed Jesus
 * language structures into instructions that can be executed
 * by the Virtual Machine.
 *
 * Compilation pipeline:
 *
 *     Jesus Source
 *            ↓
 *         Lexer
 *            ↓
 *         Parser
 *            ↓
 *           AST
 *            ↓
 *        Compiler
 *            ↓
 *          Chunk
 *            ↓
 *           VM
 *
 * Example:
 *
 * Source:
 *
 *     say 10 + 20
 *
 * AST:
 *
 *     OutputStmt
 *         └── BinaryExpr(+)
 *                 ├── NumberExpr(10)
 *                 └── NumberExpr(20)
 *
 * Generated Chunk:
 *
 *     Constants:
 *
 *         0 -> 10
 *         1 -> 20
 *
 *     Instructions:
 *
 *         PUSH_LITERAL 0
 *         PUSH_LITERAL 1
 *         ADD
 *         PRINT
 *         RETURN
 *
 * The Compiler does not execute code.
 * Its only responsibility is generating bytecode.
 */
class Compiler
{
public:
    /**
     * @brief Compiles a program AST into executable bytecode.
     *
     * @param program Root AST node.
     *
     * @return Chunk containing instructions and constants
     *         ready for execution by the VM.
     */
    Chunk compile(const std::vector<std::unique_ptr<Stmt>> &statements);

private:
    Chunk chunk;
    std::unordered_map<std::string, uint32_t> globals;
    std::unordered_map<std::string, std::shared_ptr<CreationType>> classes;

    void compileStmt(const Stmt &stmt);

    void compileExpr(const Expr &expr);

    void compileLiteralExpr(const LiteralExpr &expr);

    void compileBinaryExpr(const BinaryExpr &expr);

    void compilePrintStmt(const PrintStmt &stmt);

    uint32_t addConstant(const Value &value);

    /**
     * @brief Appends an instruction to the current Chunk.
     */
    void emit(OpCode opcode);

    /**
     * @brief Appends an instruction that requires an operand.
     *
     * Example: LOAD_CONST 3
     *  Opcode: LOAD_CONST
     *  Operand: 3
     */
    void emit(OpCode opcode, uint32_t operand);

    uint32_t registerGlobalVar(const std::string &name);
    void compileCreateVarStmt(const CreateVarStmt &stmt);
    uint32_t getGlobalVar(const std::string &name);
    void compileVariableExpr(const VariableExpr &expr);
    void compileUpdateVarStmt(const UpdateVarStmt &stmt);

    /**
     * Returns the current instruction index in the chunk.
     *
     * Used as a label for jumps and control flow targets.
     *
     * Meaning: Where am I in the bytecode stream?
     */
    uint32_t currentOffset() const;

    /**
     * @brief Emit an instruction whose operand will be patched later, like JUMP_IF_FALSE
     *
     * @param opcode the code to be emited
     * @return uint32_t the instruction index
     */
    /**
     * Emits a jump-like instruction with a temporary operand (0)
     * that will be patched later once the target address is known.
     *
     * @param opcode the code to be emited, like JUMP_IF_FALSE
     * @return uint32_t the instruction index so it can be patched.
     */
    uint32_t emitPlaceholder(OpCode opcode);

    /**
     * @brief Patches a previously emitted jump instruction with the current instruction position.
     *
     * @param instructionIndex the index of the instruction to be patched
     */
    void patchJump(uint32_t instructionIndex);

    /**
     * Compiles:
     *
     * repeat while condition:
     *     body
     * amen
     *
     * Control flow shape:
     *
     *  loopStart:
     *     evaluate condition
     *     if false → exit
     *     body
     *     jump loopStart
     *  exit:
     */
    void compileRepeatWhileStmt(const RepeatWhileStmt &stmt);

    void compileCreateClassStmt(const CreateClassStmt &stmt);
    void compileCreateInstanceExpr(const CreateInstanceExpr &expr);
};
