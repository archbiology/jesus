#pragma once

namespace jit
{

enum class JitValueType
{
    Int,
    Double,

    /**
     * @brief An index into `JitContext::globals`.
     *
     * Classes, instances and formless values use this kind.
    * The JIT stack stores a 32-bit index instead of a pointer,
    * avoiding pointer invalidation when the vector grows.
     */
    ValueIndex
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
    JitValueType valueType;

    /** @brief True when the value is known during compilation. */
    bool isConstant = false;

    /** @brief Used when valueType is int. */
    int intValue = 0;

    /** Used when valueType is double */
    double doubleValue = 0.0;

    static Slot Int(int v)
    {
        Slot slot{JitValueType::Int};
        slot.isConstant = true;
        slot.intValue = v;
        slot.doubleValue = static_cast<double>(v);
        return slot;
    }

    static Slot Double(double v)
    {
        Slot slot{JitValueType::Double};
        slot.isConstant = true;
        slot.doubleValue = v;
        return slot;
    }

    static Slot ValueIndex()
    {
        return Slot{JitValueType::ValueIndex};
    }

    double asDouble() const
    {
        return valueType == JitValueType::Int ? static_cast<double>(intValue) : doubleValue;
    }

    bool isInt() const
    {
        return valueType == JitValueType::Int;
    }

    bool isDouble() const
    {
        return valueType == JitValueType::Double;
    }

    bool isNumeric() const
    {
        return valueType == JitValueType::Int || valueType == JitValueType::Double;
    }

    bool isValueIndex() const
    {
        return valueType == JitValueType::ValueIndex;
    }
};

} // namespace jit
