#include "assembler.hpp"
#include "spirit/value.hpp"

#include <cstring>

namespace jit
{

void Assembler::emitPushLiteral(const Instruction &instruction)
{
    const Value &literal = ir->literals[instruction.operand];
    const MemoryOffset offset = static_cast<MemoryOffset>(jitStack.size() * JIT_SLOT_SIZE_BYTES);

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
    }
    else
    {
        std::uint64_t bits = 0;
        static_assert(sizeof(bits) == sizeof(literal.number));
        // ------------------------------------------------------------
        // Why memcpy instead of `bits = literal.number`?
        //   Because number is double, but bits is int64.
        //   If we assign it directly, C++ casts 3.14 to 3, losing info
        // ------------------------------------------------------------
        std::memcpy(&bits, &literal.number, sizeof(bits));

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
        pushSlot(Slot::Double(literal.number));
    }
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
void Assembler::loadNumberFromMemoryToSimd(const Slot &slot, MemoryOffset offset, const SimdRegister &simdRegister)
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

void Assembler::loadNumberToSimd(
    const Slot &slot, const bool isRightOperandInMemory, std::size_t memoryOffset, const SimdRegister &simdRegister)
{
    if (isRightOperandInMemory)
    {
        loadNumberFromMemoryToSimd(slot, memoryOffset, simdRegister);
        return;
    }

    if (slot.isInt())
    {
        mov(EAX, Immediate(slot.intValue));
        cvtsi2sd(simdRegister, EAX);
        return;
    }

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
