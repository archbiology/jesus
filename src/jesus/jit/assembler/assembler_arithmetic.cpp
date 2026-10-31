#include "assembler.hpp"
#include "spirit/value.hpp"
#include "vm/opcode.hpp"

namespace jit
{

bool Assembler::isArithmetic(OpCode opcode)
{
    switch (opcode)
    {
    case OpCode::ADD:
    case OpCode::SUBTRACT:
    case OpCode::MULTIPLY:
    case OpCode::DIVIDE:
        return true;
    default:
        return false;
    }
}

/**
 * @brief Arithmetic that consumes the top of the stack and a literal.
 *
 * These are the forms the compiler emits for literal arithmetic such as
 * `say 2 + 3` (PUSH_LITERAL followed by ADD_LITERAL_RIGHT), so supporting
 * them is what makes literal arithmetic genuinely native.
 *
 * On success, @p operation receives the equivalent plain OpCode.
 */
bool Assembler::isFusedOpForRightLiteral(OpCode opcode, OpCode &operation)
{
    switch (opcode)
    {
    case OpCode::ADD_LITERAL_RIGHT:
        operation = OpCode::ADD;
        return true;
    case OpCode::SUBTRACT_LITERAL_RIGHT:
        operation = OpCode::SUBTRACT;
        return true;
    case OpCode::MULTIPLY_LITERAL_RIGHT:
        operation = OpCode::MULTIPLY;
        return true;
    case OpCode::DIVIDE_LITERAL_RIGHT:
        operation = OpCode::DIVIDE;
        return true;
    default:
        return false;
    }
}

/**
 * @brief Emits one arithmetic operation and reports its result slot.
 *
 * @param left           static kind/value of the left operand
 * @param leftOffset     JIT stack-frame offset where the left operand lives
 * @param right          static kind/value of the right operand
 * @param isRightOperandInMemory false when the right operand is a literal;
 *                       otherwise its stack offset is @p rightOffset
 */
bool Assembler::emitArithmeticOperation(
    OpCode opcode,
    const Slot &left,
    MemoryOffset leftOffset,
    const Slot &right,
    const bool isRightOperandInMemory,
    MemoryOffset rightOffset,
    Slot &result)
{
    const bool bothInt = left.isInt() && right.isInt();
    const bool resultIsDouble = opcode == OpCode::DIVIDE || !bothInt;

    if (!resultIsDouble)
    {
        // --------------------------------------------------
        // Move the left 32-bit int from memory to 32-bit EAX
        // --------------------------------------------------
        mov(EAX, dword_ptr(RSP, leftOffset));

        // ---------------------------------------------------------------------
        // Move the right 32-bit int from memory to 32-bit ECX
        //
        // Using ECX instead of EBX because under the Linux x86-64 System V ABI,
        // EBX is set by the function that made the call. If the current
        // function changes it, it must restore its original value before
        // returning to the function that made the call.
        // EBX must survive AFTER function calls.
        // ---------------------------------------------------------------------
        if (isRightOperandInMemory)
            mov(ECX, dword_ptr(RSP, rightOffset));
        else
            mov(ECX, Immediate(right.intValue));

        switch (opcode)
        {
        case OpCode::ADD:
            add(EAX, ECX);
            break;
        case OpCode::SUBTRACT:
            sub(EAX, ECX);
            break;
        case OpCode::MULTIPLY:
            imul(EAX, ECX);
            break;
        default:
            return false;
        }

        // -------------------------------------------------------
        // (runtime) Update the left memory offset with the result
        // -------------------------------------------------------
        mov(dword_ptr(RSP, leftOffset), EAX);

        // -------------------------------------
        // set the result type for the assembler
        // -------------------------------------
        result = Slot{JitValueType::Int};

        return true;
    }

    // -----------------------------------------------------------------
    // Move the values to SIMD registers.
    // If a double has 64 bits, why using 128-bit SIMD registers and
    // not RAX and RCX that are 64 bits?
    //     Because floating-point operations only work on SIMD registers
    //     There is no 'add xmm0, 1.5' operation avaliable in assembly.
    // -----------------------------------------------------------------
    loadNumberFromMemoryToSimd(left, leftOffset, XMM0);
    loadNumberToSimd(right, isRightOperandInMemory, rightOffset, XMM1);

    switch (opcode)
    {
    case OpCode::ADD:
        addsd(XMM0, XMM1);
        break;
    case OpCode::SUBTRACT:
        subsd(XMM0, XMM1);
        break;
    case OpCode::MULTIPLY:
        mulsd(XMM0, XMM1);
        break;
    case OpCode::DIVIDE:
        divsd(XMM0, XMM1);
        break;
    default:
        return false;
    }

    // -------------------------------------------------------
    // (runtime) Update the left offset memory with the result
    // -------------------------------------------------------
    movsd(qword_ptr(RSP, leftOffset), XMM0);

    // -------------------------------------
    // set the result type for the assembler
    // -------------------------------------
    result = Slot{JitValueType::Double};

    return true;
}

/**
 * @brief Generates machine instructions for an arithmetic operation.
 *
 * Takes the left and right operands from the JIT's internal stack,
 * generates instructions to calculate the result, and pushes the result
 * back onto the stack.
 *
 * Returns false if the operation cannot be generated.
 *
 * Assume: say 10 + 20
 *
 * JIT stack before addition:
    ┌───────────────┐
    │ 20 (right)    │ ← top of stack
    ├───────────────┤
    │ 10 (left)     │
    └───────────────┘

    The method removes both values from its internal stack,
    generates machine code to calculate 10 + 20, and records the result.

    Conceptually:

    JIT stack before:
        20  ← right operand
        10  ← left operand

          ↓ Addition

    JIT stack after:
        30  ← result

    The method doesn't execute the addition immediately.
    It generates machine instructions that will perform the addition when
    the JIT-compiled program runs.
 */
bool Assembler::emitBinaryArithmeticOperation(OpCode opcode)
{
    if (jitStack.size() < 2)
        return false;

    const Slot right = jitStack.back();
    jitStack.pop_back();
    const Slot left = jitStack.back();
    jitStack.pop_back();

    // ---------------------------------------------------------------------------------
    // Above we removed two values from the compiler's internal stack model,
    // but the corresponding values are still stored in the memory that JIT runtime uses.
    // ---------------------------------------------------------------------------------
    const MemoryOffset leftOffset = static_cast<MemoryOffset>(jitStack.size() * JIT_SLOT_SIZE_BYTES);
    const MemoryOffset rightOffset = leftOffset + JIT_SLOT_SIZE_BYTES;
    constexpr bool isRightOperandInMemory = true;

    Slot result{JitValueType::Int};
    if (!emitArithmeticOperation(opcode, left, leftOffset, right, isRightOperandInMemory, rightOffset, result))
        return false;

    pushSlot(result);
    return true;
}

bool Assembler::emitFusedLiteralRight(OpCode base, const Instruction &instruction)
{
    if (jitStack.empty())
        return false;

    const std::uint32_t literalIndex = extractLower16Bits(instruction.operand);
    const Value &literal = ir->literals[literalIndex];

    Slot right{JitValueType::Int};
    if (literal.IS_INT)
        right = Slot::Int(literal.intValue);
    else
        right = Slot::Double(literal.number);

    // The left operand is already on top of the stack; the result replaces
    // it, exactly like the VM's *_LITERAL_RIGHT handlers.
    const Slot left = jitStack.back();
    const std::size_t leftOffset = (jitStack.size() - 1) * JIT_SLOT_SIZE_BYTES;
    constexpr bool isRightOperandInMemory = false;

    Slot result{JitValueType::Int};
    if (!emitArithmeticOperation(base, left, leftOffset, right, isRightOperandInMemory, /*rightOffset */ 0, result))
        return false;

    jitStack.back() = result;
    return true;
}

} // namespace jit
