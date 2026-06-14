#pragma once

#include <cstdint>

/**
 * @brief Instructions understood by the Jesus Virtual Machine.
 *
 * Each opcode (operation code) represents a single operation
 * that can be executed by the VM.
 *
 * Example:
 *
 *     LOAD_CONST
 *     LOAD_CONST
 *     ADD
 *     PRINT
 *
 * Opcodes are stored inside Chunk::code and interpreted
 * by the VM execution loop.
 */
enum class OpCode : uint8_t
{
    LOAD_CONST,
    LOAD_GLOBAL,
    STORE_GLOBAL,

    ADD,
    SUB,
    MUL,
    DIV,

    PRINT,

    JUMP,
    JUMP_IF_FALSE,

    CALL,
    RETURN
};
