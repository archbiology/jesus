#pragma once

#include <cstdint>
#include <string>

/**
 * @brief Instructions understood by the Jesus Virtual Machine.
 *
 * Each opcode (operation code) represents a single operation
 * that can be executed by the VM.
 *
 * Example:
 *
 *     PUSH_LITERAL
 *     PUSH_LITERAL
 *     ADD
 *     PRINT
 *
 * Opcodes are stored inside Chunk::code and interpreted
 * by the VM execution loop.
 */
enum class OpCode : uint8_t
{
    PUSH_LITERAL,
    CREATE_GLOBAL,
    READ_GLOBAL,
    WRITE_GLOBAL,

    ADD,
    SUBTRACT,
    MULTIPLY,
    DIVIDE,
    MODULO,

    EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,

    OR,
    AND,
    XOR,

    PRINT,

    JUMP,
    JUMP_IF_FALSE,

    CALL,
    RETURN
};

/**
 * @brief Converts an opcode into a human-readable name.
 */
std::string opcodeToString(OpCode opcode);
