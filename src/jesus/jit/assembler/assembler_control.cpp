#include "assembler.hpp"

#include <cstdint>

#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

namespace jit
{

namespace
{

enum class ComparisonOperandKind
{
    LocalWithLocal,
    LocalWithLiteral,
    LiteralWithLocal,
    LocalWithStack,
    LiteralWithStack,
};

struct ComparisonInfo
{
    ComparisonOperandKind operands;
    JitCondition condition;
};

bool getComparisonInfo(OpCode opcode, ComparisonInfo &comparision)
{
    struct Family
    {
        OpCode firstOpcode;
        OpCode lastOpcode;
        ComparisonOperandKind operands;
    };

    // ---------------------------------------------------------------------
    // In opcode.hpp we have 5 groups of 'compare-and-jump' opcodes
    //
    // To compare a local variable with another local variable, the opcodes go
    // from JUMP_IF_NOT_EQUAL_LOCAL_LOCAL to JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL
    // in opcode.hpp, so,
    //    firstOpcode is JUMP_IF_NOT_EQUAL_LOCAL_LOCAL, and
    //    lastOpcode  is JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL
    //
    // All opcodes between these two boundaries have the same ComparisonOperandKind.
    // This lets us identify the operands without listing every opcode individually.
    //
    // Why do we have those compare-and-jump opcoes? For performance.
    //
    // How do they improve performance?
    //
    //  Consider:
    //    if age < 18:
    //       say "You are a minor"
    //
    //     Without a compare-and-jump opcode, the compiler would need to:
    //        1. Compare age with 18.
    //        2. Create a boolean value (1 or 0) from that comparison.
    //        3. Store that boolean value.
    //        4. Read the boolean value.
    //        5. Jump if the value is false.
    //
    //     With a compare-and-jump opcode, it can generate:
    //        1. Compare age with 18.
    //        2. Jump directly if the comparison is false.
    //
    //     In this example, the generated code reduce work needed at runtime
    // ---------------------------------------------------------------------
    static constexpr Family families[] = {
        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LOCAL,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL,
         ComparisonOperandKind::LocalWithLocal},

        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LITERAL,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL,
         ComparisonOperandKind::LocalWithLiteral},

        {OpCode::JUMP_IF_NOT_EQUAL_LITERAL_LOCAL,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL,
         ComparisonOperandKind::LiteralWithLocal},

        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_RIGHT,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT,
         ComparisonOperandKind::LocalWithStack},

        {OpCode::JUMP_IF_NOT_EQUAL_LITERAL_RIGHT,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT,
         ComparisonOperandKind::LiteralWithStack},
    };

    // --------------------------------------------------------------
    // OpCodeNumber is uint8_t, because Opcode was declared this way:
    //      enum class OpCode : uint8_t {...}
    // --------------------------------------------------------------
    using OpCodeNumber = std::underlying_type_t<OpCode>;
    const OpCodeNumber opcodeNumber = static_cast<OpCodeNumber>(opcode);

    for (const Family &family : families)
    {
        const auto firstOpcode = static_cast<OpCodeNumber>(family.firstOpcode);
        const auto lastOpcode = static_cast<OpCodeNumber>(family.lastOpcode);

        if (opcodeNumber < firstOpcode || opcodeNumber > lastOpcode)
            continue;

        // ----------------------------------------------------------------------
        // The offset tells us which comparison condition this opcode represents.
        //
        // Each family has the same six conditions, in the same order as JitCondition:
        //
        //   0 = Equal
        //   1 = NotEqual
        //   2 = Less
        //   3 = LessEqual
        //   4 = Greater
        //   5 = GreaterEqual
        // ----------------------------------------------------------------------
        const auto offset = opcodeNumber - firstOpcode;
        if (offset >= 6)
            return false;

        comparision.operands = family.operands;
        comparision.condition = static_cast<JitCondition>(offset);
        return true;
    }

    return false;
}

bool literalAsSlot(const Value &literal, Slot &slot)
{
    if (literal.IS_INT)
    {
        slot = Slot::Int(literal.intValue);
        return true;
    }

    if (literal.IS_DOUBLE)
    {
        slot = Slot::Double(literal.doubleValue);
        return true;
    }

    return false;
}

} // namespace

// --------------------------------------------------------------------------
// JIT runtime helpers used by conditional branches.
//
// Comparisons mirror the VM's CMP_IS_TRUE: numeric operands compare through
// `number`, while non-numeric operands use Value's comparison operators.
// Packs always keep local slots in the low 16 bits;
// when a condition is packed with a slot it lives in the high 16 bits.
// --------------------------------------------------------------------------
bool jitRuntimeCompareValues(const Value &left, const Value &right, JitCondition condition)
{
    if (left.IS_NUMBER && right.IS_NUMBER)
    {
        switch (condition)
        {
        case JitCondition::Equal:
            return left.doubleValue == right.doubleValue;
        case JitCondition::NotEqual:
            return left.doubleValue != right.doubleValue;
        case JitCondition::Less:
            return left.doubleValue < right.doubleValue;
        case JitCondition::LessEqual:
            return left.doubleValue <= right.doubleValue;
        case JitCondition::Greater:
            return left.doubleValue > right.doubleValue;
        case JitCondition::GreaterEqual:
            return left.doubleValue >= right.doubleValue;
        }
    }

    switch (condition)
    {
    case JitCondition::Equal:
        return left == right;
    case JitCondition::NotEqual:
        return left != right;
    case JitCondition::Less:
        return left < right;
    case JitCondition::LessEqual:
        return left <= right;
    case JitCondition::Greater:
        return left > right;
    case JitCondition::GreaterEqual:
        return left >= right;
    }

    return false;
}

Value &localAt(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot)
{
    return ctx->jitRuntimeStack[static_cast<std::size_t>(baseSlot) + slot];
}

extern "C" int JitRuntimeCompareLocalWithLocal(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t packedSlots, std::uint32_t condition)
{
    const std::uint32_t leftSlot = extractLower16Bits(packedSlots);
    const std::uint32_t rightSlot = extractUpper16Bits(packedSlots);

    return jitRuntimeCompareValues(
        localAt(ctx, baseSlot, leftSlot), localAt(ctx, baseSlot, rightSlot), static_cast<JitCondition>(condition));
}

extern "C" int JitRuntimeCompareLocalWithNumber(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t packedSlotAndCondition, double right)
{
    const std::uint32_t slot = extractLower16Bits(packedSlotAndCondition);
    const auto condition = static_cast<JitCondition>(extractUpper16Bits(packedSlotAndCondition));

    return jitRuntimeCompareValues(localAt(ctx, baseSlot, slot), Value(right), condition);
}

extern "C" int JitRuntimeCompareNumberWithLocal(
    JitContext *ctx, double left, std::uint32_t baseSlot, std::uint32_t packedSlotAndCondition)
{
    const std::uint32_t slot = extractLower16Bits(packedSlotAndCondition);
    const auto condition = static_cast<JitCondition>(extractUpper16Bits(packedSlotAndCondition));

    return jitRuntimeCompareValues(Value(left), localAt(ctx, baseSlot, slot), condition);
}

extern "C" int JitRuntimeCheckTruthy(JitContext *ctx, std::uint32_t globalsIndex)
{
    if (globalsIndex >= ctx->globals.size())
        return 0;

    return ctx->globals[globalsIndex].truthy() ? 1 : 0;
}

bool Assembler::isComparisonBranch(OpCode opcode)
{
    ComparisonInfo branch{};
    return getComparisonInfo(opcode, branch);
}

bool Assembler::emitJump(const Instruction &instruction)
{
    if (ir == nullptr || instruction.operand >= ir->instructions.size())
        return false;

    jump(instruction.operand);
    return true;
}

bool Assembler::emitJumpIfFalse(const Instruction &instruction)
{
    if (ir == nullptr || instruction.operand >= ir->instructions.size() || jitCompilationStack.empty())
        return false;

    const Slot condition = jitCompilationStack.back();
    jitCompilationStack.pop_back();

    const MemoryOffset offset = static_cast<MemoryOffset>(jitCompilationStack.size() * JIT_SLOT_SIZE_BYTES);

    if (condition.isInt())
    {
        // --------------------------------------------
        // Read the integer value from the stack.
        // Zero means false; any other value means true.
        // jump if zero means the condition was false,
        // then we 'jump to else/otherwise'
        // --------------------------------------------
        mov(EAX, dword_ptr(RSP, offset));
        jumpIfZero(EAX, instruction.operand);
        return true;
    }

    if (condition.isDouble())
    {
        // -----------------------------------------
        // Read the double from the stack into XMM0.
        // -----------------------------------------
        movsd(XMM0, qword_ptr(RSP, offset));

        // ------------------------------------------
        // Load 0.0 into XMM1 to compare it with XMM0
        // ------------------------------------------
        constexpr bool isRightOperandInMemory = false;
        constexpr std::size_t memoryOffset = 0;
        loadNumberToSimd(Slot::Double(0.0), isRightOperandInMemory, memoryOffset, XMM1);

        // -------------------------------------------------------------------
        // Compare if both numbers are equal (meaning the double value is 0.0)
        // -------------------------------------------------------------------
        compareDoubles(XMM0, XMM1);
        setComparisonResult(JitCondition::NotEqual, EAX);

        // ------------------
        // Jump if it is zero
        // ------------------
        jumpIfZero(EAX, instruction.operand);
        return true;
    }

    if (condition.isStackIndex())
    {
        // ---------------------------------------------
        // Load the first param of JitRuntimeCheckTruthy
        // ---------------------------------------------
        loadJitContextPointerIntoRDI();

        // --------------------------------------------------------------------------
        // Load the second param of JitRuntimeCheckTruthy(..., globalsIndex) into RSI
        // --------------------------------------------------------------------------
        mov(RSI, dword_ptr(RSP, offset));

        // --------------------------
        // call JitRuntimeCheckTruthy
        // --------------------------
        mov(RAX, Immediate(&JitRuntimeCheckTruthy));
        call(RAX);

        jumpIfZero(EAX, instruction.operand);
        return true;
    }

    return false;
}

bool Assembler::emitComparisonBranch(const Instruction &instruction, std::uint32_t jumpTarget)
{
    if (ir == nullptr || jumpTarget >= ir->instructions.size())
        return false;

    ComparisonInfo comparison{};
    if (!getComparisonInfo(instruction.opcode, comparison))
        return false;

    auto loadLiteral = [&](std::uint32_t literalIndex, const SimdRegister &destination) -> bool
    {
        if (literalIndex >= ir->literals.size())
            return false;

        Slot slot{JitValueType::Double};
        if (!literalAsSlot(ir->literals[literalIndex], slot))
            return false;

        loadNumberToSimd(slot, false, 0, destination);
        return true;
    };

    const std::uint32_t leftIndexOrLiteral = extractLower16Bits(instruction.operand);
    const std::uint32_t rightIndex = extractUpper16Bits(instruction.operand);
    const auto condition = static_cast<std::uint32_t>(comparison.condition);

    // Runtime comparison helpers that take a slot expect the condition in the
    // upper 16 bits. The IR operand uses those bits for the other operand, so
    // rebuild the runtime packing explicitly.
    const std::uint32_t packedSlotAndCondition = pack16Bits(leftIndexOrLiteral, condition);

    switch (comparison.operands)
    {
    case ComparisonOperandKind::LocalWithLocal:
    {
        if (leftIndexOrLiteral >= ir->localCount || rightIndex >= ir->localCount)
            return false;

        // -------------------------------
        // 1. Load the JitContext into RDI
        // -------------------------------
        loadJitContextPointerIntoRDI();

        // -----------------------------
        // 2. Load the baseSlot into RSI
        // -----------------------------
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));

        // --------------------------------------------------------
        // 3. Load the packedSlots (leftIndex, rightIndex) into EDX
        // --------------------------------------------------------
        mov(EDX, Immediate(instruction.operand));

        // ------------------------------
        // 4. Load JitCondition into ECX
        // ------------------------------
        mov(ECX, Immediate(condition));

        // ---------------------------------------------------
        // Now call the function whose params we loaded [1..4]
        // ---------------------------------------------------
        mov(RAX, Immediate(&JitRuntimeCompareLocalWithLocal));
        call(RAX);

        jumpIfZero(EAX, jumpTarget);
        return true;
    }

    case ComparisonOperandKind::LocalWithLiteral:
    {
        // ---------------------------------------------------------------------------------
        // Load the literal double into XMM0 (4th param of JitRuntimeCompareLocalWithNumber)
        // ---------------------------------------------------------------------------------
        if (leftIndexOrLiteral >= ir->localCount || !loadLiteral(rightIndex, XMM0))
            return false;

        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));
        // -------------------------------------------
        // 3. Load the packedSlotAndCondition into EDX
        // -------------------------------------------
        mov(EDX, Immediate(packedSlotAndCondition));

        mov(RAX, Immediate(&JitRuntimeCompareLocalWithNumber));
        call(RAX);

        jumpIfZero(EAX, jumpTarget);
        return true;
    }

    case ComparisonOperandKind::LiteralWithLocal:
    {
        // ---------------------------------------------------------------------------------
        // Load the literal double into XMM0 (2nd param of JitRuntimeCompareNumberWithLocal)
        // ---------------------------------------------------------------------------------
        if (!loadLiteral(rightIndex, XMM0) || leftIndexOrLiteral >= ir->localCount)
            return false;

        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));

        // ---------------------------------------
        // 3. Load packedSlotAndCondition into EDX
        // ---------------------------------------
        mov(EDX, Immediate(packedSlotAndCondition));

        mov(RAX, Immediate(&JitRuntimeCompareNumberWithLocal));
        call(RAX);

        jumpIfZero(EAX, jumpTarget);
        return true;
    }

    case ComparisonOperandKind::LocalWithStack:
    {
        if (jitCompilationStack.empty() || leftIndexOrLiteral >= ir->localCount)
            return false;

        const Slot leftValue = jitCompilationStack.back();
        if (!leftValue.isNumeric())
            return false;

        const MemoryOffset leftOffset =
            static_cast<MemoryOffset>((jitCompilationStack.size() - 1) * JIT_SLOT_SIZE_BYTES);

        loadNumberFromMemoryToSimd(leftValue, leftOffset, XMM0);

        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));
        // ---------------------------------------
        // 3. Load packedSlotAndCondition into EDX
        // ---------------------------------------
        mov(EDX, Immediate(packedSlotAndCondition));

        mov(RAX, Immediate(&JitRuntimeCompareNumberWithLocal));
        call(RAX);

        jitCompilationStack.pop_back();
        jumpIfZero(EAX, jumpTarget);
        return true;
    }

    case ComparisonOperandKind::LiteralWithStack:
    {
        // ------------------------------------------
        // Check that there is a literal in the stack
        // ------------------------------------------
        if (jitCompilationStack.empty())
            return false;

        const Slot leftValue = jitCompilationStack.back();
        if (!leftValue.isNumeric())
            return false;

        const MemoryOffset leftOffset =
            static_cast<MemoryOffset>((jitCompilationStack.size() - 1) * JIT_SLOT_SIZE_BYTES);

        // --------------------------------------------
        // Load the left value from the stack into XMM0
        // --------------------------------------------
        loadNumberFromMemoryToSimd(leftValue, leftOffset, XMM0);

        // --------------------------------
        // Load the literal value into XMM1
        // --------------------------------
        if (!loadLiteral(leftIndexOrLiteral, XMM1))
            return false;

        compareDoubles(XMM0, XMM1);
        setComparisonResult(comparison.condition, EAX);

        jitCompilationStack.pop_back();
        jumpIfZero(EAX, jumpTarget);
        return true;
    }
    }

    return false;
}

} // namespace jit
