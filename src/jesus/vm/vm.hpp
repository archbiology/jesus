#pragma once

#include "chunk.hpp"

#include "ast/stmt/create_method_stmt.hpp"

#include <unordered_map>

/**
 * @brief Context of the current method being executed.
 *
 * Pushed by CALL and popped by RETURN. Each frame carries:
 *
 * - chunk:  the bytecode being executed (the program, or a method body).
 * - ip:     the next instruction to execute inside that chunk.
 * - locals: the local slots of the running method; slot 0 is "$self".
 */
struct CallFrame
{
    const Chunk *chunk;
    const Instruction *ip;
    std::vector<Value> locals;
};

/**
 * @brief Executes Jesus bytecode.
 *
 * The VM reads instructions from a Chunk and executes
 * them sequentially.
 *
 * Components:
 *
 * - chunk:
 *     Compiled bytecode currently being executed.
 *
 * - ip (Instruction Pointer):
 *     Points to the next instruction to execute.
 *
 * - stack:
 *     Temporary evaluation stack used by instructions.
 *
 * Example:
 *
 *     PUSH_LITERAL 1
 *     PUSH_LITERAL 2
 *     ADD
 *
 * Execution:
 *
 *     []       -> initial stack
 *     [1]
 *     [1,2]
 *     [3]
 *
 * The VM continues until a RETURN instruction is reached.
 */
class VM
{
private:
    /**
     * The bytecode of each method body.
     * Looked up on CALL through the Method's definition pointer.
     */
    const std::unordered_map<const CreateMethodStmt *, Chunk> &methodChunks;

    /**
     * Evaluation stack used by the VM.
     *
     * Instructions push and pop values from this stack.
     */
    std::vector<Value> stack;
    std::vector<Value> globals;

    /**
     * Active method invocations;
     * the program itself (global context) is the bottom frame.
     */
    std::vector<CallFrame> frames;

public:
    /**
     * @brief Default constructor
     *
     * @param methodChunks the bytecode of each method body
     */
    explicit VM(const std::unordered_map<const CreateMethodStmt *, Chunk> &methodChunks);

    /**
     * Executes the loaded Chunk until a RETURN
     * instruction is encountered.
     */
    void run(const Chunk &chunk);
};
