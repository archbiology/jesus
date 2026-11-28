#include "assembler.hpp"
#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

#include <bit>

#include <cstring>

namespace jit
{

JitValueType Assembler::readVarType(
    std::uint32_t slot, const std::vector<JitValueType> &types, const std::vector<bool> &varsToReadAsValues)
{

    // -------------------------------------------------------------
    // If the slot has no known (int/double) type, return StackIndex
    // -------------------------------------------------------------
    if (slot >= types.size())
        return JitValueType::StackIndex;

    // ------------------------------------------
    // If the slot exists in varsToReadAsValues
    // and its value is true, return StackIndex.
    // ------------------------------------------
    if (slot < varsToReadAsValues.size() && varsToReadAsValues[slot])
        return JitValueType::StackIndex;

    // ----------------------------------------------------
    // Return its type from `types`. It's an int or double.
    // ----------------------------------------------------
    return types[slot];
}

Value readFromStack(const JitContext &context, const void *address, JitValueType type)
{
    if (type == JitValueType::Double)
    {
        double doubleValue;
        std::memcpy(&doubleValue, address, sizeof(doubleValue));
        return Value(doubleValue);
    }

    // -------------------------------------------------------------
    // Not double, so it's either an integer or a globals StackIndex
    // -------------------------------------------------------------
    std::uint32_t bits;
    std::memcpy(&bits, address, sizeof(bits));
    return type == JitValueType::Int ? Value(std::bit_cast<int>(bits)) : context.globals[bits];
}

void writeToStack(JitContext &context, void *address, JitValueType type, Value result)
{
    if (type == JitValueType::Double)
        std::memcpy(address, &result.doubleValue, sizeof(result.doubleValue));

    else if (type == JitValueType::Int)
        std::memcpy(address, &result.intValue, sizeof(result.intValue));

    else
    {
        // ------------------------------------------
        // Store the Value in context.globals
        // and write its index to the assembly stack.
        // ------------------------------------------
        const auto index = static_cast<std::uint32_t>(context.globals.size());
        context.globals.push_back(std::move(result));
        std::memcpy(address, &index, sizeof(index));
    }
}

// Implemented in assembler_objects.cpp
extern "C" std::uint32_t JitRuntimeAppendLiteral(JitContext *ctx, const Value *value);

bool Assembler::emitPushLiteral(const Instruction &instruction)
{
    const Value &literal = ir->literals[instruction.operand];
    const MemoryOffset offset = static_cast<MemoryOffset>(jitCompilationStack.size() * JIT_SLOT_SIZE_BYTES);

    if (literal.IS_INT)
    {
        // -----------------------------------------------
        // Store the integer in the JIT's stack frame.
        //
        // mov:         Copy a value into a destination.
        // dword_ptr:   The destination is a 32-bit (4-byte) location in memory.
        // RSP:         The CPU register holding the current stack pointer (an address).
        // offset:      The byte offset from RSP where the value will be stored.
        // Immediate:   The actual integer value to write
        // -----------------------------------------------
        mov(dword_ptr(RSP, offset), Immediate(literal.intValue));
        pushSlot(Slot::Int(literal.intValue));
        return true;
    }

    if (literal.IS_DOUBLE)
    {
        std::uint64_t bits = 0;
        static_assert(sizeof(bits) == sizeof(literal.doubleValue));
        // ------------------------------------------------------------
        // Why memcpy instead of `bits = literal.doubleValue`?
        //   Because number is double, but bits is int64.
        //   If we assign it directly, C++ casts 3.14 to 3, losing info
        // ------------------------------------------------------------
        std::memcpy(&bits, &literal.doubleValue, sizeof(bits));

        // ------------------------------------------
        // Move the double value (bits) to 64-bit RAX
        // ------------------------------------------
        mov(RAX, Immediate(bits));

        // ----------------------------------------------------------------
        // Copy from 64-bit RAX to 64-bit location in Memory
        // qword_ptr:   64 bits
        //
        // Why not copy Immediate(double) directly to memory
        // just like we did for int?
        //    Because x86-64 doesn't have a mov encoding that writes
        //    an arbitrary 64-bit immediate directly to memory.
        //    (32 bits can be copied directly into memory, but 64 bits, no)
        // ----------------------------------------------------------------
        mov(qword_ptr(RSP, offset), RAX);
        pushSlot(Slot::Double(literal.doubleValue));
        return true;
    }

    if (literal.IS_CLASS || literal.IS_FORMLESS)
    {
        // --------------------------------------------------------
        // Non-numeric values are copied into `JitContext::globals`
        // The stack slot stores the returned index.
        //
        // Lets load the JitRuntimeAppendLiteral's params
        // Start loading the `JitContext` in RDI (the first arg)
        // --------------------------------------------------------
        loadJitContextPointerIntoRDI();

        // ------------------------------------------------
        // Now, load the second argument (`value`) into RSI
        // ------------------------------------------------
        mov(RSI, Immediate(reinterpret_cast<std::int64_t>(&literal)));

        // ----------------------------------------------------------
        // Now put the function address in RAX, and call the function
        // ----------------------------------------------------------
        mov(RAX, Immediate(&JitRuntimeAppendLiteral));
        call(RAX);

        // ----------------------------------------------------
        // The function's return, the instance index, is at EAX
        // Let's take it from EAX and save it into the stack
        // ----------------------------------------------------
        mov(dword_ptr(RSP, offset), EAX);
        pushSlot(Slot::StackIndex());
        return true;
    }

    return false;
}

/**
 * @brief Loads a numeric value from the JIT stack frame into a 128-bit SIMD register.
 *
 * If the value is an integer, it converts it to double-precision before loading it.
 * If the value is already a double, it copies its 64-bit representation
 * directly into the 128-bit SIMD register.
 *
 * This allows arithmetic operations to work with integers and doubles
 * using the same double-precision SIMD instructions.
 */
void Assembler::loadNumberFromMemoryToSimd(const SimdRegister &simdRegister, const Slot &slot, MemoryOffset offset)
{
    if (slot.isInt())
        // ------------------------------------------------------------------
        // Convert the 32-bit int to 128-bit and move it to the SIMD register
        // ------------------------------------------------------------------
        cvtsi2sd(simdRegister, dword_ptr(RSP, offset));
    else
        // ----------------------------------------
        // The slot holds a double instead.
        // Move qword (64 bits) to the SIMD register
        // ----------------------------------------
        movsd(simdRegister, qword_ptr(RSP, offset));
}

void Assembler::loadNumberToSimd(const SimdRegister &simdRegister, const Slot &slot)
{
    if (slot.isInt())
    {
        mov(EAX, Immediate(slot.intValue));
        cvtsi2sd(simdRegister, EAX);
        return;
    }

    // It is a double
    // ------------------------------------------------------------
    // Why memcpy instead of `bits = slot.doubleValue`?
    //   Because doubleValue is double, but bits is int64.
    //   If we assign it directly, C++ casts 3.14 to 3, losing info
    // ------------------------------------------------------------
    std::uint64_t bits = 0;
    std::memcpy(&bits, &slot.doubleValue, sizeof(bits));

    // ------------------------------------------
    // Move the double value (bits) to 64-bit RAX
    // ------------------------------------------
    mov(RAX, Immediate(bits));

    // -----------------------------------------------------------
    // Copy from 64-bit RAX to 128-bit SIMD
    //
    // Why not copy Immediate(doubleValue) directly to memory
    // just like we did for int?
    //    Because x86-64 doesn't have a mov encoding that writes
    //    an arbitrary 64-bit immediate directly to SIMD register.
    // -----------------------------------------------------------
    movq(simdRegister, RAX);
}
} // namespace jit
