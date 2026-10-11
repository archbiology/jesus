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
    // Fused opcodes that avoid the operand stack for local arithmetic.
    //
    // The compiler folds common operand-fetch + binary-op sequences into a
    // single instruction, removing stack pushes, pops and dispatches from
    // hot numeric loops. The suffix says where each operand comes from:
    //
    //   LOCAL_LOCAL          left = local, right = local
    //   LOCAL_LITERAL        left = local, right = literal constant
    //   LITERAL_LOCAL        left = literal constant, right = local
    //   LOCAL_LOCAL_STORE    like LOCAL_LOCAL, but writes the result
    //                        straight into a destination local instead of
    //                        pushing it (so no stack traffic at all).
    //
    // Operand packing is documented next to each family's handler in vm.cpp.
    // -----------------------------------------------------------------
    ADD_LOCAL_LOCAL,
    SUBTRACT_LOCAL_LOCAL,
    MULTIPLY_LOCAL_LOCAL,
    DIVIDE_LOCAL_LOCAL,
    MODULO_LOCAL_LOCAL,
    EQUAL_LOCAL_LOCAL,
    NOT_EQUAL_LOCAL_LOCAL,
    LESS_LOCAL_LOCAL,
    LESS_EQUAL_LOCAL_LOCAL,
    GREATER_LOCAL_LOCAL,
    GREATER_EQUAL_LOCAL_LOCAL,

    ADD_LOCAL_LOCAL_STORE,
    SUBTRACT_LOCAL_LOCAL_STORE,
    MULTIPLY_LOCAL_LOCAL_STORE,
    DIVIDE_LOCAL_LOCAL_STORE,
    MODULO_LOCAL_LOCAL_STORE,

    // Operand is a local and a literal constant, in the order named by the
    // opcode (LOCAL_LITERAL = local on the left, LITERAL_LOCAL = literal on
    // the left). Packed as: local slot in the low 16 bits, constant index in
    // the high 16 bits.
    ADD_LOCAL_LITERAL,
    SUBTRACT_LOCAL_LITERAL,
    MULTIPLY_LOCAL_LITERAL,
    DIVIDE_LOCAL_LITERAL,
    MODULO_LOCAL_LITERAL,
    EQUAL_LOCAL_LITERAL,
    NOT_EQUAL_LOCAL_LITERAL,
    LESS_LOCAL_LITERAL,
    LESS_EQUAL_LOCAL_LITERAL,
    GREATER_LOCAL_LITERAL,
    GREATER_EQUAL_LOCAL_LITERAL,

    ADD_LITERAL_LOCAL,
    SUBTRACT_LITERAL_LOCAL,
    MULTIPLY_LITERAL_LOCAL,
    DIVIDE_LITERAL_LOCAL,
    MODULO_LITERAL_LOCAL,
    EQUAL_LITERAL_LOCAL,
    NOT_EQUAL_LITERAL_LOCAL,
    LESS_LITERAL_LOCAL,
    LESS_EQUAL_LITERAL_LOCAL,
    GREATER_LITERAL_LOCAL,
    GREATER_EQUAL_LITERAL_LOCAL,

    // Arithmetic forms that write their result straight into a local, for
    // patterns like `i = i + 1`. A LOCAL_LITERAL_STORE operand packs:
    // local slot (bits 0-7), constant index (bits 8-23), destination (24-31).
    ADD_LOCAL_LITERAL_STORE,
    SUBTRACT_LOCAL_LITERAL_STORE,
    MULTIPLY_LOCAL_LITERAL_STORE,
    DIVIDE_LOCAL_LITERAL_STORE,
    MODULO_LOCAL_LITERAL_STORE
};

/**
 * @brief Converts an opcode into a human-readable name.
 */
std::string opcodeToString(OpCode opcode);
