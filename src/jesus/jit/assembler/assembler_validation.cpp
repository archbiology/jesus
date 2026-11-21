#include "assembler.hpp"

#include <algorithm>

#include "interpreter/runtime/method.hpp"
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
namespace
{

int comparisonBranchFamily(OpCode opcode)
{
    using OpCodeValue = std::underlying_type_t<OpCode>;

    struct Family
    {
        OpCode first;
        OpCode last;
    };

    static constexpr Family families[] = {
        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LOCAL, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL},
        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LITERAL, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL},
        {OpCode::JUMP_IF_NOT_EQUAL_LITERAL_LOCAL, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL},
        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_RIGHT, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT},
        {OpCode::JUMP_IF_NOT_EQUAL_LITERAL_RIGHT, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT},
    };

    const auto value = static_cast<OpCodeValue>(opcode);

    for (int index = 0; index < 5; ++index)
    {
        const auto first = static_cast<OpCodeValue>(families[index].first);
        const auto last = static_cast<OpCodeValue>(families[index].last);

        if (value >= first && value <= last)
            return index;
    }

    return -1;
}

} // namespace

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
    std::vector<JitValueType> tempLocalVariableTypes;
    const auto tmpFusedLocalsToReadAsValues = findFusedLocalsToReadAsValues(ir);
    maxSlots = 0;

    auto push = [&](const Slot &slot)
    {
        tempStack.push_back(slot);
        if (tempStack.size() > maxSlots)
            maxSlots = tempStack.size();
    };

    for (std::size_t instructionIndex = 0; instructionIndex < ir.instructions.size(); ++instructionIndex)
    {
        const Instruction &instruction = ir.instructions[instructionIndex];
        const OpCode opcode = instruction.opcode;

        if (opcode == OpCode::PUSH_LITERAL)
        {
            if (instruction.operand >= ir.literals.size())
                return false;

            const Value &literal = ir.literals[instruction.operand];
            if (literal.IS_INT)
                push(Slot::Int(literal.intValue));
            else if (literal.IS_DOUBLE)
                push(Slot::Double(literal.number));
            else if (literal.IS_CLASS || literal.IS_FORMLESS)
                push(Slot::ValueIndex());
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

        if (opcode == OpCode::READ_LOCAL)
        {
            if (ir.localCount == 0 || instruction.operand >= ir.localCount)
                return false;

            const std::uint32_t index = instruction.operand;
            const JitValueType valueType = localReadType(index, tempLocalVariableTypes, tmpFusedLocalsToReadAsValues);
            push(Slot{valueType});
            continue;
        }

        if (opcode == OpCode::WRITE_LOCAL)
        {
            if (ir.localCount == 0 || instruction.operand >= ir.localCount || tempStack.empty())
                return false;

            const std::uint32_t index = instruction.operand;
            if (index >= tempLocalVariableTypes.size())
                tempLocalVariableTypes.resize(index + 1, JitValueType::ValueIndex);

            tempLocalVariableTypes[index] = tempStack.back().valueType;
            tempStack.pop_back();
            continue;
        }

        if (opcode == OpCode::CREATE_INSTANCE)
        {
            if (tempStack.empty() || !tempStack.back().isValueIndex())
                return false;

            tempStack.back() = Slot::ValueIndex();
            continue;
        }

        if (opcode == OpCode::CALL)
        {
            if (instruction.operand >= ir.literals.size())
                return false;

            const Value &literal = ir.literals[instruction.operand];
            if (!literal.IS_METHOD)
                return false;

            const auto method = std::dynamic_pointer_cast<Method>(literal.asMethod());
            if (!method)
                return false;

            std::uint32_t paramsCount = method->params ? method->params->paramsCount : 0;
            paramsCount++; // Why? We always have $self instance as first arg.
            if (tempStack.size() < paramsCount)
                return false;

            for (std::uint32_t index = 0; index < paramsCount; ++index)
                tempStack.pop_back();

            push(Slot::ValueIndex());
            continue;
        }

        if (opcode == OpCode::JUMP)
        {
            if (instruction.operand >= ir.instructions.size())
                return false;

            continue;
        }

        if (opcode == OpCode::JUMP_IF_FALSE)
        {
            if (tempStack.empty() || instruction.operand >= ir.instructions.size())
                return false;

            tempStack.pop_back();
            continue;
        }

        if (opcode == OpCode::INIT_REPEAT_COUNT)
        {
            // ---------------
            // repeat N times
            // ---------------
            if (tempStack.empty() || !tempStack.back().isInt())
                return false;

            tempStack.pop_back();

            const std::uint32_t index = instruction.operand;
            if (index >= tempGlobalVariableTypes.size())
                tempGlobalVariableTypes.resize(index + 1);

            tempGlobalVariableTypes[index] = JitValueType::Int;
            continue;
        }

        if (opcode == OpCode::ADD_LOCAL_LITERAL_STORE_JUMP || opcode == OpCode::ADD_LOCAL_LITERAL_STORE)
        {
            // ------------------------------------------------------------
            // Fused loop tail: local[dest] = local[source] + literal
            //
            // Operand layout (same as ADD_LOCAL_LITERAL_STORE):
            //   bits 0-7    source local slot
            //   bits 8-23   literal index
            //   bits 24-31  destination local slot
            // ------------------------------------------------------------
            if (opcode == OpCode::ADD_LOCAL_LITERAL_STORE_JUMP)
            {
                if (instructionIndex + 1 >= ir.instructions.size())
                    return false;

                const Instruction &targetHolder = ir.instructions[instructionIndex + 1];
                if (targetHolder.opcode != OpCode::JUMP || targetHolder.operand >= ir.instructions.size())
                    return false;
            }

            if (ir.localCount == 0)
                return false;

            const std::uint32_t sourceSlot = extractLower8Bits(instruction.operand);
            const std::uint32_t literalIndex = extractMiddle16Bits(instruction.operand);
            const std::uint32_t destinationSlot = extractUpper8Bits(instruction.operand);

            if (sourceSlot >= ir.localCount || destinationSlot >= ir.localCount || literalIndex >= ir.literals.size())
                return false;

            // The fused operation only writes a local;
            // it does not push or pop an operand-stack value.
            continue;
        }

        if (isFusedLocalOperation(opcode))
        {
            if (!validateFusedLocalOperation(
                    ir, instructionIndex, tempStack, tempLocalVariableTypes, tmpFusedLocalsToReadAsValues))
                return false;

            maxSlots = std::max(maxSlots, tempStack.size());

            // ----------------------------------------------------------------
            // This fused instruction also uses the next instruction to store
            // its jump destination. Skip it because it is not an instruction.
            //
            //   Instruction N:     JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL
            //   Instruction N + 1: Jump destination
            //   Instruction N + 2: Next instruction
            //
            // Used in a loop like this:
            //   repeat while i <= limit:
            //     say i
            //     i = i + 1
            //   amen
            // ----------------------------------------------------------------
            if (opcode == OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL)
                ++instructionIndex;

            continue;
        }

        if (isComparisonBranch(opcode))
        {
            if (instructionIndex + 1 >= ir.instructions.size())
                return false;

            const std::uint32_t target = ir.instructions[instructionIndex + 1].operand;
            if (target >= ir.instructions.size())
                return false;

            const std::uint32_t left = extractLower16Bits(instruction.operand);
            const std::uint32_t right = extractUpper16Bits(instruction.operand);
            const int family = comparisonBranchFamily(opcode);

            auto validLocal = [&](std::uint32_t slot) { return ir.localCount != 0 && slot < ir.localCount; };

            auto validNumericLiteral = [&](std::uint32_t literalIndex)
            {
                if (literalIndex >= ir.literals.size())
                    return false;

                const Value &literal = ir.literals[literalIndex];
                return literal.IS_INT || literal.IS_DOUBLE;
            };

            switch (family)
            {
            case 0: // localAndLocal
                if (!validLocal(left) || !validLocal(right))
                    return false;
                break;
            case 1: // localAndLiteral
                if (!validLocal(left) || !validNumericLiteral(right))
                    return false;
                break;
            case 2: // literalAndLocal
                if (!validNumericLiteral(right) || !validLocal(left))
                    return false;
                break;
            case 3: // localAndStack
            case 4: // literalAndStack
                if (tempStack.empty() || !tempStack.back().isNumeric())
                    return false;
                tempStack.pop_back();

                if (family == 3) // localAndStack
                {
                    if (!validLocal(left))
                        return false;
                }
                else if (!validNumericLiteral(left))
                {
                    return false;
                }
                break;
            default:
                return false;
            }

            continue;
        }

        // -----------------------------------------------------------------
        // Numeric comparisons consume two values and produce an int boolean
        // (0 or 1), which JUMP_IF_FALSE can then consume.
        // -----------------------------------------------------------------
        if (isComparison(opcode))
        {
            if (tempStack.size() < 2)
                return false;

            const Slot right = tempStack.back();
            tempStack.pop_back();
            const Slot left = tempStack.back();
            tempStack.pop_back();

            if (!left.isNumeric() || !right.isNumeric())
                return false;

            push(Slot{JitValueType::Int});
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

            if (!left.isNumeric() || !right.isNumeric())
                return false;

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

            if (!right.isNumeric())
                return false;

            if (isDivisionByZero(base_operation, right))
                return false;

            // --------------------------------------------
            // Add the slot size (int/double) to the stack
            // --------------------------------------------
            const Slot left = tempStack.back();
            if (!left.isNumeric())
                return false;

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

        if (opcode == OpCode::EXIT)
        {
            // EXIT only ends the top-level program chunk.
            if (ir.localCount != 0)
                return false;

            // Anything after EXIT is unreachable and is ignored here as well.
            return true;
        }

        if (opcode == OpCode::RETURN)
        {
            // RETURN only returns from a method and hands its top stack value
            // back to the caller.
            if (ir.localCount == 0 || tempStack.empty())
                return false;

            // RETURN consumes one value.
            tempStack.pop_back();
            continue;
        }

        // Any other opcode: the JIT engine will run this chunk on the VM.
        return false;
    }

    return true;
}

} // namespace jit
