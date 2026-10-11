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
    MODULO_LOCAL_LITERAL_STORE,

    // Forms where the LEFT operand is already on top of the operand stack
    // and only the RIGHT operand has to be fetched. The result replaces the
    // left operand in place, so no extra stack slot is created. Operand is
    // the right-hand local slot / constant index in the low 16 bits.
    ADD_LOCAL_RIGHT,
    SUBTRACT_LOCAL_RIGHT,
    MULTIPLY_LOCAL_RIGHT,
    DIVIDE_LOCAL_RIGHT,
    MODULO_LOCAL_RIGHT,
    EQUAL_LOCAL_RIGHT,
    NOT_EQUAL_LOCAL_RIGHT,
    LESS_LOCAL_RIGHT,
    LESS_EQUAL_LOCAL_RIGHT,
    GREATER_LOCAL_RIGHT,
    GREATER_EQUAL_LOCAL_RIGHT,

    ADD_LITERAL_RIGHT,
    SUBTRACT_LITERAL_RIGHT,
    MULTIPLY_LITERAL_RIGHT,
    DIVIDE_LITERAL_RIGHT,
    MODULO_LITERAL_RIGHT,
    EQUAL_LITERAL_RIGHT,
    NOT_EQUAL_LITERAL_RIGHT,
    LESS_LITERAL_RIGHT,
    LESS_EQUAL_LITERAL_RIGHT,
    GREATER_LITERAL_RIGHT,
    GREATER_EQUAL_LITERAL_RIGHT,

    // Comparison-and-branch: used when a comparison result only feeds a
    // conditional jump. They evaluate the comparison and jump when it is
    // false, without materialising a boolean Value. The operand holds the
    // same packed operands as the matching comparison opcode; the jump
    // target is the operand of the instruction that immediately follows.
    JUMP_IF_NOT_EQUAL_LOCAL_LOCAL,
    JUMP_IF_NOT_NOT_EQUAL_LOCAL_LOCAL,
    JUMP_IF_NOT_LESS_LOCAL_LOCAL,
    JUMP_IF_NOT_LESS_EQUAL_LOCAL_LOCAL,
    JUMP_IF_NOT_GREATER_LOCAL_LOCAL,
    JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL,

    JUMP_IF_NOT_EQUAL_LOCAL_LITERAL,
    JUMP_IF_NOT_NOT_EQUAL_LOCAL_LITERAL,
    JUMP_IF_NOT_LESS_LOCAL_LITERAL,
    JUMP_IF_NOT_LESS_EQUAL_LOCAL_LITERAL,
    JUMP_IF_NOT_GREATER_LOCAL_LITERAL,
    JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL,

    JUMP_IF_NOT_EQUAL_LITERAL_LOCAL,
    JUMP_IF_NOT_NOT_EQUAL_LITERAL_LOCAL,
    JUMP_IF_NOT_LESS_LITERAL_LOCAL,
    JUMP_IF_NOT_LESS_EQUAL_LITERAL_LOCAL,
    JUMP_IF_NOT_GREATER_LITERAL_LOCAL,
    JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL,

    JUMP_IF_NOT_EQUAL_LOCAL_RIGHT,
    JUMP_IF_NOT_NOT_EQUAL_LOCAL_RIGHT,
    JUMP_IF_NOT_LESS_LOCAL_RIGHT,
    JUMP_IF_NOT_LESS_EQUAL_LOCAL_RIGHT,
    JUMP_IF_NOT_GREATER_LOCAL_RIGHT,
    JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT,

    JUMP_IF_NOT_EQUAL_LITERAL_RIGHT,
    JUMP_IF_NOT_NOT_EQUAL_LITERAL_RIGHT,
    JUMP_IF_NOT_LESS_LITERAL_RIGHT,
    JUMP_IF_NOT_LESS_EQUAL_LITERAL_RIGHT,
    JUMP_IF_NOT_GREATER_LITERAL_RIGHT,
    JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT,

    // Store variants of the *_RIGHT arithmetic forms. The left operand is on
    // top of the stack (as for *_RIGHT), the right operand is fetched from a
    // local or constant in the low 16 bits, and the low 16 bits of the
    // destination local slot are the high 16 bits. The result is written
    // straight to the destination and the left operand is discarded, so
    // `x = (x2 - y2) + cx` needs no WRITE_LOCAL.
    ADD_LOCAL_RIGHT_STORE,
    SUBTRACT_LOCAL_RIGHT_STORE,
    MULTIPLY_LOCAL_RIGHT_STORE,
    DIVIDE_LOCAL_RIGHT_STORE,
    MODULO_LOCAL_RIGHT_STORE,

    ADD_LITERAL_RIGHT_STORE,
    SUBTRACT_LITERAL_RIGHT_STORE,
    MULTIPLY_LITERAL_RIGHT_STORE,
    DIVIDE_LITERAL_RIGHT_STORE,
    MODULO_LITERAL_RIGHT_STORE
};

/**
 * @brief Converts an opcode into a human-readable name.
 */
std::string opcodeToString(OpCode opcode);
