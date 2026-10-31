#include "assembler.hpp"
#include "spirit/value.hpp"
#include "vm/opcode.hpp"

namespace jit
{

/**
 * @brief Check if it is a division by zero.
 * FIXME: Validate this at compile time
 */
inline bool isDivisionByZero(const OpCode &opcode, const Slot &right)
{
    return opcode == OpCode::DIVIDE && right.isConstant && right.asDouble() == 0.0;
}

/**
 * @brief First pass: validate the whole chunk can be compiled by JIT.
 *
 * If the chunk contains operations not yet supported by the JIT,
 * the VM executes it instead.
 *
 * This pass checks operand types and calculates the maximum number of stack
 * slots needed to hold intermediate values during execution. The JIT uses
 * this count to reserve space on the current thread's C++ execution stack
 * for the generated machine code to store those values.
 */
bool Assembler::validateSupportedJitOperations(const IR &ir, std::size_t &maxSlots)
{
    std::vector<Slot> tempStack;
    std::vector<JitValueType> tempGlobalVariableTypes;
    maxSlots = 0;

    auto push = [&](const Slot &slot)
    {
        tempStack.push_back(slot);
        if (tempStack.size() > maxSlots)
            maxSlots = tempStack.size();
    };

    for (const Instruction &instruction : ir.instructions)
    {
        const OpCode opcode = instruction.opcode;

        // -----------------------------------------------------
        // If PUSH_LITERAL, check if the literal is a number
        // because we only support arithmethic operations for now
        // ------------------------------------------------------
        if (opcode == OpCode::PUSH_LITERAL)
        {
            if (instruction.operand >= ir.literals.size())
                return false;

            const Value &literal = ir.literals[instruction.operand];
            if (literal.IS_INT)
                push(Slot::Int(literal.intValue));
            else if (literal.IS_DOUBLE)
                push(Slot::Double(literal.number));
            else
                return false;

            continue;
        }

        if (opcode == OpCode::CREATE_GLOBAL || opcode == OpCode::WRITE_GLOBAL)
        {
            if (tempStack.empty())
                return false;

            const Slot value = tempStack.back();
            tempStack.pop_back();

            const std::uint32_t index = instruction.operand;
            if (index >= tempGlobalVariableTypes.size())
                tempGlobalVariableTypes.resize(index + 1);

            tempGlobalVariableTypes[index] = value.valueType;
            continue;
        }

        if (opcode == OpCode::READ_GLOBAL)
        {
            const std::uint32_t index = instruction.operand;
            if (index >= tempGlobalVariableTypes.size())
                return false;

            push(Slot{tempGlobalVariableTypes[index]});
            continue;
        }

        // ----------------------------------------------------------------
        // If isArithmetic, validate division by zero,
        // pop the operands and add the slot size (int/double) to the stack
        // ----------------------------------------------------------------
        if (isArithmetic(opcode))
        {
            if (tempStack.size() < 2)
                return false;

            // -------------------------
            // Validate division by zero
            // -------------------------
            const Slot right = tempStack.back();
            tempStack.pop_back();
            const Slot left = tempStack.back();
            tempStack.pop_back();
            if (isDivisionByZero(opcode, right))
                return false;

            // --------------------------------------------
            // Add the slot size (int/double) to the stack
            // --------------------------------------------
            const bool bothInt = left.isInt() && right.isInt();
            const bool resultIsDouble = opcode == OpCode::DIVIDE || !bothInt;
            push(Slot{resultIsDouble ? JitValueType::Double : JitValueType::Int});
            continue;
        }

        // -----------------------------------------------
        // If fused operation, validate division by zero,
        // and add the slot size (int/double) to the stack
        // -----------------------------------------------
        OpCode base_operation{};
        if (isFusedOpForRightLiteral(opcode, base_operation))
        {
            if (tempStack.empty())
                return false;

            const std::uint32_t literalIndex = extractLower16Bits(instruction.operand);
            if (literalIndex >= ir.literals.size())
                return false;

            // --------------------------------------------
            // Extract the literal (right) value of the IR
            // --------------------------------------------
            const Value &literal = ir.literals[literalIndex];
            Slot right{JitValueType::Int};
            if (literal.IS_INT)
                right = Slot::Int(literal.intValue);
            else if (literal.IS_DOUBLE)
                right = Slot::Double(literal.number);
            else
                return false;

            if (isDivisionByZero(base_operation, right))
                return false;

            // --------------------------------------------
            // Add the slot size (int/double) to the stack
            // --------------------------------------------
            const Slot left = tempStack.back();
            const bool bothInt = left.isInt() && right.isInt();
            const bool resultIsDouble = base_operation == OpCode::DIVIDE || !bothInt;
            tempStack.back() = Slot{resultIsDouble ? JitValueType::Double : JitValueType::Int};
            continue;
        }

        if (opcode == OpCode::PRINT)
        {
            if (tempStack.empty())
                return false;

            tempStack.pop_back();
            continue;
        }

        if (opcode == OpCode::RETURN)
        {
            // Top-level RETURN ends the program chunk. Anything after it is
            // unreachable for the VM and is ignored here as well.
            return true;
        }

        // Any other opcode: the JIT engine will run this chunk on the VM.
        return false;
    }

    return true;
}

} // namespace jit
