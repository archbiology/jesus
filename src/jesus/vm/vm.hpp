#pragma once

#include "chunk.hpp"

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
 *     LOAD_CONST 1
 *     LOAD_CONST 2
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
     * Compiled bytecode currently being executed.
     */
    Chunk *chunk;

    /**
     * Instruction Pointer.
     *
     * Points to the next bytecode instruction to execute.
     */
    Instruction *ip;

    /**
     * Evaluation stack used by the VM.
     *
     * Instructions push and pop values from this stack.
     */
    std::vector<Value> stack;

public:
    /**
     * Executes the loaded Chunk until a RETURN
     * instruction is encountered.
     */
    void run(const Chunk &chunk);
};
