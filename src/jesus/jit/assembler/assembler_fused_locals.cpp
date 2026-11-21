#include "assembler.hpp"

#include <bit>
#include <cstring>
#include <limits>

#include "jit/jit_context.hpp"
#include "spirit/value.hpp"
#include "vm/opcode.hpp"

namespace jit
{
namespace
{

// Each field uses the same packing as its VM handler. Keep unused fields
// empty so validation only checks the operands the instruction actually reads.
struct FusedLocalOperands
{
    static constexpr std::uint32_t unused = std::numeric_limits<std::uint32_t>::max();
    std::uint32_t first = unused;
    std::uint32_t second = unused;
    std::uint32_t third = unused;
    std::uint32_t destination = unused;
    std::uint32_t literal = unused;
};

FusedLocalOperands decodeFusedLocalOperands(const Instruction &instruction)
{
    const auto packed = instruction.operand;
    const auto lowerHalf = extractLower16Bits(packed);
    const auto upperHalf = extractUpper16Bits(packed);
    const auto firstByte = extractLower8Bits(packed);
    const auto secondByte = extractLower8Bits(extractMiddle16Bits(packed));
    const auto thirdByte = extractLower8Bits(upperHalf);

    switch (instruction.opcode)
    {
    case OpCode::MULTIPLY_LOCAL_LITERAL: // say x * 3.0
    case OpCode::MULTIPLY_LITERAL_LOCAL: // say 2.0 * x
        return {.first = lowerHalf, .literal = upperHalf};

    case OpCode::DIVIDE_LOCAL_RIGHT: // say x * 3.0 / width, with x * 3.0 on the stack
        return {.first = lowerHalf};

    case OpCode::MULTIPLY_LOCAL_LOCAL_STORE: // create x2: number = x * x
        return {.first = firstByte, .second = secondByte, .destination = upperHalf};

    case OpCode::MULTIPLY_ADD_LOCAL_RIGHT_STORE: // y = 2.0 * x * y + cy, with 2.0 * x on the stack
        return {.first = firstByte, .second = secondByte, .destination = thirdByte};

    case OpCode::SUBTRACT_ADD_LOCAL_LOCAL_LOCAL_STORE: // x = x2 - y2 + cx
        return {.first = firstByte, .second = secondByte, .third = thirdByte, .destination = extractUpper8Bits(packed)};

    case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL: // if x2 + y2 > 4.0:
        return {.first = firstByte, .second = secondByte, .literal = upperHalf};

    default:
        return {};
    }
}

bool usesOperandFromStack(OpCode opcode)
{
    return opcode == OpCode::DIVIDE_LOCAL_RIGHT || opcode == OpCode::MULTIPLY_ADD_LOCAL_RIGHT_STORE;
}

JitValueType getArithmeticResultType(JitValueType left, JitValueType right)
{
    if (left == JitValueType::ValueIndex || right == JitValueType::ValueIndex)
        return JitValueType::ValueIndex;

    return left == JitValueType::Double || right == JitValueType::Double ? JitValueType::Double : JitValueType::Int;
}

Value readStackOperand(const JitContext &context, const void *address, JitValueType type)
{
    if (type == JitValueType::Double)
    {
        double number;
        std::memcpy(&number, address, sizeof(number));
        return Value(number);
    }

    // -------------------------------------------------------------
    // Not double, so it's either an integer or a globals ValueIndex
    // -------------------------------------------------------------
    std::uint32_t bits;
    std::memcpy(&bits, address, sizeof(bits));
    return type == JitValueType::Int ? Value(std::bit_cast<int>(bits)) : context.globals[bits];
}

void writeStackResult(JitContext &context, void *address, JitValueType type, Value result)
{
    if (type == JitValueType::Double)
        std::memcpy(address, &result.number, sizeof(result.number));

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

Value multiply(const Value &left, const Value &right)
{
    if (left.IS_INT && right.IS_INT)
        return Value(std::bit_cast<int>(std::uint32_t(left.intValue) * std::uint32_t(right.intValue)));

    return left * right;
}

Value multiplyThenAdd(const Value &left, const Value &right, const Value &valueToAdd)
{
    if (left.IS_NUMBER && right.IS_NUMBER && valueToAdd.IS_NUMBER)
    {
        // ------------------------------------------
        // One value is double. Example: 2 * 3 + 1.5
        // ------------------------------------------
        if (left.IS_DOUBLE || right.IS_DOUBLE || valueToAdd.IS_DOUBLE)
            return Value(left.number * right.number + valueToAdd.number);

        // ---------------------------
        // Integer operands: 6 * 5 + 3
        // ---------------------------
        return Value(
            std::bit_cast<int>(
                std::uint32_t(left.intValue) * std::uint32_t(right.intValue) + std::uint32_t(valueToAdd.intValue)));
    }

    // ---------------------------------------------------------------
    // At least one operand is not a number (example:  "ha" * 2 + "!")
    // ---------------------------------------------------------------
    return multiply(left, right) + valueToAdd;
}

Value subtractThenAdd(const Value &left, const Value &right, const Value &valueToAdd)
{
    if (left.IS_NUMBER && right.IS_NUMBER && valueToAdd.IS_NUMBER)
    {
        if (left.IS_DOUBLE || right.IS_DOUBLE || valueToAdd.IS_DOUBLE)
            return Value(left.number - right.number + valueToAdd.number);

        // ---------
        // Integers
        // ---------
        return Value(
            std::bit_cast<int>(
                std::uint32_t(left.intValue) - std::uint32_t(right.intValue) + std::uint32_t(valueToAdd.intValue)));
    }

    // --------------------------------------------------------------
    // At least one operand is not a number. Ex.: "hello" - "l" + "!"
    // --------------------------------------------------------------
    return left - right + valueToAdd;
}

// Like the existing JIT helpers for calls and local stores, this operates on
// the frame's boxed Values. In particular, it can read incoming arguments
// such as cx and cy without assuming they were stored by earlier JIT code.

/**
 * @brief Executes a fused operation using the original Value objects stored
 *        in the JIT runtime stack.
 *
 * Fused operations combine multiple operations into a single instruction.
 * This function executes those operations using the existing Value objects
 * instead of generating separate machine instructions for each operation.
 *
 * @param context     Runtime context containing local variables and exceptions.
 * @param baseSlot    First runtime stack slot belonging to the current frame.
 * @param instruction Fused instruction to execute.
 * @param literal     Literal value used by operations that require one.
 * @param stackSlot   Address of the assembly stack slot used by the operation.
 * @param stackType   Type of the value currently stored in that stack slot.
 *
 * @return 0 if the operation is unsupported or an exception occurred.
 *         1 if the operation completed successfully.
 *         2 if the conditional operation determined that the condition is true.
 *
 * Exceptions are caught and saved in context->jitException. They are rethrown
 * after the generated code returns, because exceptions cannot safely unwind
 * through JIT-generated stack frames.
 */
int executeFusedLocalOperation(
    JitContext *context,
    std::uint32_t baseSlot,
    const Instruction *instruction,
    const Value *literal,
    void *stackSlot,
    JitValueType stackType) noexcept
{
    try
    {
        const auto operands = decodeFusedLocalOperands(*instruction);
        auto local = [&](std::uint32_t index) -> Value & { return context->jitRuntimeStack[baseSlot + index]; };
        const Value &first = local(operands.first);

        switch (instruction->opcode)
        {
        case OpCode::MULTIPLY_LOCAL_LITERAL:
            writeStackResult(*context, stackSlot, stackType, multiply(first, *literal));
            break;

        case OpCode::MULTIPLY_LITERAL_LOCAL:
            writeStackResult(*context, stackSlot, stackType, multiply(*literal, first));
            break;

        case OpCode::DIVIDE_LOCAL_RIGHT:
            // Value division always returns a double and reports a zero divisor.
            writeStackResult(
                *context, stackSlot, JitValueType::Double, readStackOperand(*context, stackSlot, stackType) / first);
            break;

        case OpCode::MULTIPLY_LOCAL_LOCAL_STORE:
            local(operands.destination) = multiply(first, local(operands.second));
            break;

        case OpCode::MULTIPLY_ADD_LOCAL_RIGHT_STORE:
            // Evaluate the full right-hand side before writing the destination,
            // which can alias a source local (as y does in Mandelbrot).
            local(operands.destination) =
                multiplyThenAdd(readStackOperand(*context, stackSlot, stackType), first, local(operands.second));
            break;

        case OpCode::SUBTRACT_ADD_LOCAL_LOCAL_LOCAL_STORE:
            local(operands.destination) = subtractThenAdd(first, local(operands.second), local(operands.third));
            break;

        case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL:
        {
            const Value &second = local(operands.second);
            const Value sum =
                first.IS_INT && second.IS_INT
                    ? Value(std::bit_cast<int>(std::uint32_t(first.intValue) + std::uint32_t(second.intValue)))
                    : first + second;
            const bool greater = sum.IS_NUMBER && literal->IS_NUMBER ? sum.number > literal->number : sum > *literal;

            // ---------------------------------------------------------------------
            // 1 if the operation completed successfully.
            // 2 if the conditional operation determined that the condition is true.
            // ---------------------------------------------------------------------
            return greater ? 2 : 1;
        }
        default:
            // --------------------------------------------------------
            // Return 0 into EAX because we don't support the operation
            // --------------------------------------------------------
            return 0;
        }

        return 1;
    }
    catch (...)
    {
        context->jitException = std::current_exception();
        // --------------------------------------
        // Return 0 into EAX because of exception
        // --------------------------------------
        return 0;
    }
}

} // namespace

bool Assembler::isFusedLocalOperation(OpCode opcode)
{
    return decodeFusedLocalOperands(Instruction{opcode, 0}).first != FusedLocalOperands::unused;
}

std::vector<bool> Assembler::findFusedLocalsToReadAsValues(const IR &ir)
{
    bool hasJump = false;
    for (const Instruction &instruction : ir.instructions)
    {
        if (instruction.opcode == OpCode::JUMP || instruction.opcode == OpCode::JUMP_IF_FALSE)
        {
            hasJump = true;
            break;
        }
    }
    if (!hasJump)
        return {};

    std::vector<bool> fusedLocalsToReadAsValues(ir.localCount);
    for (const Instruction &instruction : ir.instructions)
    {
        const auto destination = decodeFusedLocalOperands(instruction).destination;
        // ---------------------------------------------------------------
        // destination != FusedLocalOperands::unused (unued = UINT32_MAX)?
        // ---------------------------------------------------------------
        if (destination < ir.localCount)
            fusedLocalsToReadAsValues[destination] = true;
    }
    return fusedLocalsToReadAsValues;
}

JitValueType Assembler::localReadType(
    std::uint32_t slot, const std::vector<JitValueType> &types, const std::vector<bool> &boxedLocals)
{
    if (slot >= types.size() || (slot < boxedLocals.size() && boxedLocals[slot]))
        return JitValueType::ValueIndex;

    return types[slot];
}

bool Assembler::validateFusedLocalOperation(
    const IR &ir,
    std::size_t index,
    std::vector<Slot> &stack,
    std::vector<JitValueType> &localTypes,
    const std::vector<bool> &boxedLocals)
{
    const Instruction &instruction = ir.instructions[index];
    const auto operands = decodeFusedLocalOperands(instruction);
    auto validLocal = [&](std::uint32_t slot) { return slot == FusedLocalOperands::unused || slot < ir.localCount; };
    if (!validLocal(operands.first) || !validLocal(operands.second) || !validLocal(operands.third) ||
        !validLocal(operands.destination))
        return false;

    JitValueType literalType = JitValueType::ValueIndex;
    if (operands.literal != FusedLocalOperands::unused)
    {
        if (operands.literal >= ir.literals.size())
            return false;

        const Value &literal = ir.literals[operands.literal];
        if (literal.IS_INT)
            literalType = JitValueType::Int;
        else if (literal.IS_DOUBLE)
            literalType = JitValueType::Double;
    }

    // ---------------------
    // Example: if a + b > 2
    // ---------------------
    if (instruction.opcode == OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL)
        return index + 1 < ir.instructions.size() && ir.instructions[index + 1].operand < ir.instructions.size();

    if (usesOperandFromStack(instruction.opcode) && stack.empty())
        return false;

    auto localType = [&](std::uint32_t slot) { return localReadType(slot, localTypes, boxedLocals); };
    const JitValueType first = localType(operands.first);
    if (instruction.opcode == OpCode::DIVIDE_LOCAL_RIGHT)
    {
        stack.back() = Slot{JitValueType::Double};
        return true;
    }

    if (operands.destination == FusedLocalOperands::unused)
    {
        // ------------------------------------------------------------------------
        // A successful multiplication by a double literal always returns a double,
        // even when the local's type is only known at runtime.
        // ------------------------------------------------------------------------
        const auto result =
            literalType == JitValueType::Double ? JitValueType::Double : getArithmeticResultType(first, literalType);
        stack.push_back(Slot{result});
        return true;
    }

    auto result = getArithmeticResultType(first, localType(operands.second));
    if (instruction.opcode == OpCode::MULTIPLY_ADD_LOCAL_RIGHT_STORE)
    {
        result = getArithmeticResultType(result, stack.back().valueType);
        stack.pop_back();
    }
    else if (instruction.opcode == OpCode::SUBTRACT_ADD_LOCAL_LOCAL_LOCAL_STORE)
        result = getArithmeticResultType(result, localType(operands.third));

    if (operands.destination >= localTypes.size())
        localTypes.resize(operands.destination + 1, JitValueType::ValueIndex);

    localTypes[operands.destination] = result;
    return true;
}

bool Assembler::emitExecuteFusedLocalOperation(std::size_t index)
{
    const Instruction &instruction = ir->instructions[index];
    const auto operands = decodeFusedLocalOperands(instruction);
    const bool readsStack = usesOperandFromStack(instruction.opcode);
    const std::size_t stackSize = jitCompilationStack.size();
    if (readsStack && stackSize == 0)
        return false;

    auto stackType = readsStack ? jitCompilationStack.back().valueType : JitValueType::ValueIndex;
    if (!validateFusedLocalOperation(*ir, index, jitCompilationStack, localVariableTypes, fusedLocalsToReadAsValues))
        return false;

    const bool pushesResult = jitCompilationStack.size() > stackSize;
    if (pushesResult)
        stackType = jitCompilationStack.back().valueType;

    const Value *literal = operands.literal == FusedLocalOperands::unused ? nullptr : &ir->literals[operands.literal];

    // ----------------------------------------------
    // Load the params for executeFusedLocalOperation
    // ----------------------------------------------

    loadJitContextPointerIntoRDI();

    // --------------------------------------
    // Load `std::uint32_t baseSlot` into RSI
    // --------------------------------------
    mov(RSI, dword_ptr(RSP, jitMethodsOffset));

    // ----------------------------------------------
    // Load `const Instruction *instruction` into RDX
    // ----------------------------------------------
    mov(RDX, Immediate(reinterpret_cast<std::uintptr_t>(&instruction)));

    // ------------------------------------
    // Load `const Value *literal` into RCX
    // ------------------------------------
    mov(RCX, Immediate(reinterpret_cast<std::uintptr_t>(literal)));

    // ----------------------------------------------------------------
    // Load `void *stackSlot` (the stack slot to read or write) into R8
    // ----------------------------------------------------------------
    if (readsStack || pushesResult)
    {
        const std::size_t slotIndex = readsStack ? stackSize - 1 : stackSize;
        lea(R8, qword_ptr(RSP, static_cast<MemoryOffset>(slotIndex * JIT_SLOT_SIZE_BYTES)));
    }
    else
        mov(R8, Immediate(0));

    // -----------------------------------------------------------------------
    // Load `JitValueType stackType` (integer, a double, or an index) into R9
    // -----------------------------------------------------------------------
    mov(R9, Immediate(static_cast<std::uint32_t>(stackType)));

    // ----------------------------------------
    // Call executeFusedLocalOperation
    // After the call, EAX contains the status:
    //   zero means we have an exception.
    // ----------------------------------------
    mov(RAX, Immediate(&executeFusedLocalOperation));
    call(RAX);
    jumpIfZero(EAX, static_cast<std::uint32_t>(ir->instructions.size()));

    if (instruction.opcode == OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL)
    {
        // ----------------------------------------------------------------------------------
        // executeFusedLocalOperation returns 1 for a failed comparison and 2 when it passes.
        // Turning 1 into zero lets jumpIfZero implement the `otherwise` branch.
        // ----------------------------------------------------------------------------------
        sub(EAX, Immediate(1));
        jumpIfZero(EAX, ir->instructions[index + 1].operand);
    }

    return true;
}

} // namespace jit
