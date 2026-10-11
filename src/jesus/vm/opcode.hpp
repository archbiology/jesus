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

    READ_LOCAL,
    WRITE_LOCAL,

    CREATE_INSTANCE,
    READ_ATTR,
    WRITE_ATTR,

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
    RETURN,

    // -----------------------------------------------------------------
    // Fused "read local, read local, binary op" opcodes.
    //
    // The compiler folds the common sequence
    //
    //     READ_LOCAL a
    //     READ_LOCAL b
    //     <binary op>
    //
    // into a single instruction whose operand packs both local slots as
    // (a in the low 16 bits, b in the high 16 bits). This removes two
    // stack pushes and two dispatches from the hottest loops.
    // -----------------------------------------------------------------
    ADD_LL,
    SUBTRACT_LL,
    MULTIPLY_LL,
    DIVIDE_LL,
    MODULO_LL,
    EQUAL_LL,
    NOT_EQUAL_LL,
    LESS_LL,
    LESS_EQUAL_LL,
    GREATER_LL,
    GREATER_EQUAL_LL
};

/**
 * @brief Converts an opcode into a human-readable name.
 */
std::string opcodeToString(OpCode opcode);
