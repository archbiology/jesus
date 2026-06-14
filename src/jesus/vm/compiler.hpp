#pragma once

#include "chunk.hpp"

#include "ast/expr/literal_expr.hpp"

#include "ast/stmt/print_stmt.hpp"

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
 *         LOAD_CONST 0
 *         LOAD_CONST 1
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

    void compileStmt(const Stmt &stmt);

    void compileExpr(const Expr &expr);

    void compileLiteralExpr(const LiteralExpr &expr);

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
};
