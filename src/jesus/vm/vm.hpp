#pragma once

#include "chunk.hpp"

/**
 * @brief Context of the current method being executed.
 *
 * Pushed by CALL and popped by RETURN. Each frame carries:
 *
 * - chunk:       the bytecode being executed (the program, or a method body).
 * - ip:          the next instruction to execute inside that chunk.
 * - localsBase:  this frame's region inside the value stack;
 *                slot 0 of the method ("$self") lives at that index.
 */
struct CallFrame
{
    const Chunk *chunk;
    const Instruction *ip;
    std::size_t localsBase;
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
     * Evaluation stack used by the VM.
     *
     * Instructions push and pop values from this stack. The active frame's
     * locals also live here, at the bottom of the stack region owned by
     * that frame (see CallFrame::localsBase).
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
     */
    VM() = default;

    /**
     * Executes the loaded Chunk until a RETURN
     * instruction is encountered.
     */
    void run(const Chunk &chunk);
};
