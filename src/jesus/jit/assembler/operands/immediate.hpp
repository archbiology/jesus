#pragma once

#include <cstdint>

namespace jit
{

/**
 * @brief Represents an immediate integer operand in an assembly instruction.
 *
 * An immediate is a literal value encoded directly into an instruction,
 * rather than a value read from a register or memory.
 *
 * Example:
 *      add(RAX, Immediate(42)); // Adds 42 directly to RAX.
 */
struct Immediate
{
    std::int64_t value;

    constexpr Immediate(std::int64_t value) : value(value)
    {
    }

    /** To convert function pointers to int64 */
    template <typename NativeFunction, typename... Args>
    Immediate(NativeFunction (*function)(Args...))
        : value(static_cast<std::int64_t>(reinterpret_cast<std::uintptr_t>(function)))
    {
    }

    /**
     * @brief Allow implicit conversion to std::int64_t.
     *
     * Unlike a type alias, Immediate remains a distinct type,
     * preventing it from being confused with a regular integer.
     */
    constexpr operator std::int64_t() const
    {
        return value;
    }
};

} // namespace jit
