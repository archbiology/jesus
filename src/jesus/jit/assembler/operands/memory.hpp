#pragma once

#include "register.hpp"

#include <cstdint>

namespace jit
{

/**
 * @brief Describes the size of a value stored in or read from a memory location.
 *
 * These names follow x86 terminology:
 *      Dword = doubleword = 32 bits (4 bytes).
 *      Qword = quadword   = 64 bits (8 bytes).
 *
 * Example:
 *   DwordMemory(Register::RSP, -4)
 *
 * Represents the 4-byte memory location at [RSP - 4].
 * Assembly: mov eax, dword ptr [rsp - 4]
 *
 * The size of a memory value is independent of the target architecture.
 * Even in 64-bit mode, x86-64 instructions can read and write 32-bit values.
 */
struct MemoryOperand
{
    /**
     * @brief Register (normally RSP) containing the starting address.
     */
    Register base_register;

    /**
     * @brief Offset in bytes, added to the base (RSP) address.
     *
     * Examples:
     *      [RSP - 8]  -> offset = -8
     *      [RSP + 16] -> offset = 16
     */
    std::int32_t offset;

    MemoryOperand(Register base_register, std::int32_t offset) : base_register(base_register), offset(offset)
    {
    }
};

/**
 * @brief The 32-bit (4-byte) memory operand.
 *
 * Commonly used for 32-bit integers.
 */
struct DwordMemory : MemoryOperand
{
    // inherit the base class's constructors
    using MemoryOperand::MemoryOperand;
};

/**
 * @brief The 64-bit (8-byte) memory operand.
 *
 * Commonly used for 64-bit integers, pointers, and double-precision
 * floating-point values.
 */
struct QwordMemory : MemoryOperand
{
    // inherit the base class's constructors
    using MemoryOperand::MemoryOperand;
};

/**
 * @brief Creates a memory operand that accesses 4 bytes (32 bits).
 *
 * It is used to write to or read from memory.
 */
inline DwordMemory dword_ptr(Register base_register, std::int32_t offset)
{
    return DwordMemory(base_register, offset);
}

/**
 * @brief Creates a memory operand that accesses 8 bytes (64 bits).
 *
 * It is used to write to or read from memory.
 */
inline QwordMemory qword_ptr(Register base_register, std::int32_t offset)
{
    return QwordMemory(base_register, offset);
}
} // namespace jit
