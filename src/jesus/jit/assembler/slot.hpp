#pragma once

namespace jit
{

enum class Kind
{
    Int,
    Double
};

/**
 * @brief JIT slot (used at compile time)
 *
 * Used by the JIT compiler to determine how values must be
 * loaded, converted, and processed.
 *
 * It represents the jit-compiler's view of a stack slot,
 * not the actual runtime memory where the value is stored.
 */
struct Slot
{
    Kind kind;

    /** @brief True when the value is known during compilation. */
    bool isConstant = false;

    /** @brief Used when kind is Kind::Int. */
    int intValue = 0;

    /** Used when kind is Kind::Double */
    double doubleValue = 0.0;

    static Slot Int(int v)
    {
        Slot slot{Kind::Int};
        slot.isConstant = true;
        slot.intValue = v;
        slot.doubleValue = static_cast<double>(v);
        return slot;
    }

    static Slot Double(double v)
    {
        Slot slot{Kind::Double};
        slot.isConstant = true;
        slot.doubleValue = v;
        return slot;
    }

    double asDouble() const
    {
        return kind == Kind::Int ? static_cast<double>(intValue) : doubleValue;
    }

    bool isInt() const
    {
        return kind == Kind::Int;
    }
};

} // namespace jit
