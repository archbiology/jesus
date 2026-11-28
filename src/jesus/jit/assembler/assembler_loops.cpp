#include "assembler.hpp"

#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

namespace jit
{
/** @brief Emit: destination = source + literal
 *
 * Example: i = i + 1
 */
bool Assembler::emitAddLiteralToLocal(const Instruction &instruction)
{
    if (ir == nullptr)
        return false;

    const std::uint32_t sourceSlot = extractLower8Bits(instruction.operand);
    const std::uint32_t literalIndex = extractMiddle16Bits(instruction.operand);
    const std::uint32_t destinationSlot = extractUpper8Bits(instruction.operand);

    if (ir->localCount == 0 || sourceSlot >= ir->localCount || destinationSlot >= ir->localCount ||
        literalIndex >= ir->literals.size())
    {
        return false;
    }

    const Value &literal = ir->literals[literalIndex];
    const JitValueType destinationType = destinationSlot < localVariableTypes.size()
        ? localVariableTypes[destinationSlot] : JitValueType::StackIndex;
    const JitValueType sourceType = sourceSlot < localVariableTypes.size()
        ? localVariableTypes[sourceSlot] : JitValueType::StackIndex;

    if (sourceType == JitValueType::StackIndex || !literal.IS_NUMBER)
        return false;

    const bool bothOperandsAreIntegers = sourceType == JitValueType::Int && literal.IS_INT;
    const JitValueType resultType = bothOperandsAreIntegers ? JitValueType::Int : JitValueType::Double;
    if (destinationType != resultType)
        return false;

    emitLoadCurrentMethodBaseSlotAddressIntoRDX();

    if (bothOperandsAreIntegers)
    {
        // ---------------------------------------------
        // Load sourceSlot.intValue into EAX.
        // Add literal.intValue to EAX.
        // Write the result to destinationSlot.intValue.
        // Example: destination = source + literal
        // ---------------------------------------------
        mov(EAX, intValueOffset(RDX, sourceSlot));
        add(EAX, Immediate(literal.intValue));
        mov(intValueOffset(RDX, destinationSlot), EAX);

        // ------------------------------------------------------
        // Convert the integer result in EAX to double.
        // Write the double value to destinationSlot.doubleValue.
        // ------------------------------------------------------
        cvtsi2sd(XMM0, EAX);
        movsd(doubleValueOffset(RDX, destinationSlot), XMM0);
    }
    else
    {
        // ----------------------------------------------------
        // Convert the source .intValue to double if necessary
        // Otherwise, load .doubleValue directly into XMM0
        // ----------------------------------------------------
        if (sourceType == JitValueType::Int)
            cvtsi2sd(XMM0, intValueOffset(RDX, sourceSlot));
        else
            movsd(XMM0, doubleValueOffset(RDX, sourceSlot));

        // --------------------------------------------
        // Load the literal as a double into XMM1
        // Value already sets .doubleValue for integers
        // --------------------------------------------
        loadNumberToSimd(XMM1, Slot::Double(literal.doubleValue));

        // --------------------------------------
        // Add the operands and write the double
        // result to destinationSlot.doubleValue
        // --------------------------------------
        addsd(XMM0, XMM1);
        movsd(doubleValueOffset(RDX, destinationSlot), XMM0);
    }

    return true;
}

bool Assembler::emitRepeatNTimes(const Instruction &instruction)
{
    if (jitCompilationStack.empty() || !jitCompilationStack.back().isInt())
        return false;

    return emitStoreGlobal(instruction);
}

} // namespace jit
