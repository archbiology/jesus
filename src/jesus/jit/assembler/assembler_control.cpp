#include "assembler.hpp"

#include <cstdint>

#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

namespace jit
{

namespace
{

Value &localAt(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot)
{
    return ctx->jitRuntimeStack[static_cast<std::size_t>(baseSlot) + slot];
}

bool compareValues(const Value &left, const Value &right, JitCondition condition)
{
    if (left.IS_NUMBER && right.IS_NUMBER)
    {
        switch (condition)
        {
        case JitCondition::Equal:
            return left.number == right.number;
        case JitCondition::NotEqual:
            return left.number != right.number;
        case JitCondition::Less:
            return left.number < right.number;
        case JitCondition::LessEqual:
            return left.number <= right.number;
        case JitCondition::Greater:
            return left.number > right.number;
        case JitCondition::GreaterEqual:
            return left.number >= right.number;
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

enum class BranchOperandKind
{
    LocalLocal,
    LocalLiteral,
    LiteralLocal,
    LocalRight,
    LiteralRight,
};

struct BranchInfo
{
    BranchOperandKind kind;
    JitCondition condition;
};

bool getBranchInfo(OpCode opcode, BranchInfo &info)
{
    using OpCodeValue = std::underlying_type_t<OpCode>;

    struct Family
    {
        OpCode first;
        OpCode last;
        BranchOperandKind kind;
    };

    static constexpr Family families[] = {
        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LOCAL,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL,
         BranchOperandKind::LocalLocal},
        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LITERAL,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL,
         BranchOperandKind::LocalLiteral},
        {OpCode::JUMP_IF_NOT_EQUAL_LITERAL_LOCAL,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL,
         BranchOperandKind::LiteralLocal},
        {OpCode::JUMP_IF_NOT_EQUAL_LOCAL_RIGHT,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT,
         BranchOperandKind::LocalRight},
        {OpCode::JUMP_IF_NOT_EQUAL_LITERAL_RIGHT,
         OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT,
         BranchOperandKind::LiteralRight},
    };

    const auto value = static_cast<OpCodeValue>(opcode);

    for (const Family &family : families)
    {
        const auto first = static_cast<OpCodeValue>(family.first);
        const auto last = static_cast<OpCodeValue>(family.last);

        if (value < first || value > last)
            continue;

        const auto offset = value - first;
        if (offset >= 6)
            return false;

        info.kind = family.kind;
        info.condition = static_cast<JitCondition>(offset);
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
        slot = Slot::Double(literal.number);
        return true;
    }

    return false;
}

} // namespace

// --------------------------------------------------------------------------
// JIT runtime helpers used by comparison branches and fused loop increments.
//
// Comparisons mirror the VM's CMP_IS_TRUE: numeric operands compare through
// `number`, while non-numeric operands use Value's comparison operators.
// Packs always keep local slots in the low 16 bits; when a condition is packed
// with a slot it lives in the high 16 bits.
// --------------------------------------------------------------------------
extern "C" int JitRuntimeCompareLocalLocal(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t packedSlots, std::uint32_t condition)
{
    const std::uint32_t leftSlot = packedSlots & 0xFFFF;
    const std::uint32_t rightSlot = packedSlots >> 16;
    return compareValues(
        localAt(ctx, baseSlot, leftSlot), localAt(ctx, baseSlot, rightSlot), static_cast<JitCondition>(condition));
}

extern "C" int JitRuntimeCompareLocalNumber(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t packedSlotAndCondition, double right)
{
    const std::uint32_t slot = packedSlotAndCondition & 0xFFFF;
    const auto condition = static_cast<JitCondition>(packedSlotAndCondition >> 16);
    return compareValues(localAt(ctx, baseSlot, slot), Value(right), condition);
}

extern "C" int JitRuntimeCompareNumberLocal(
    JitContext *ctx, double left, std::uint32_t baseSlot, std::uint32_t packedSlotAndCondition)
{
    const std::uint32_t slot = packedSlotAndCondition & 0xFFFF;
    const auto condition = static_cast<JitCondition>(packedSlotAndCondition >> 16);
    return compareValues(Value(left), localAt(ctx, baseSlot, slot), condition);
}

extern "C" void JitRuntimeAddLocalLiteralStore(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t packedSlots, const Value *literal)
{
    const std::uint32_t sourceSlot = packedSlots & 0xFF;
    const std::uint32_t destinationSlot = (packedSlots >> 8) & 0xFF;
    localAt(ctx, baseSlot, destinationSlot) = localAt(ctx, baseSlot, sourceSlot) + *literal;
}

extern "C" int JitRuntimeValueTruthy(JitContext *ctx, std::uint32_t index)
{
    if (index >= ctx->globals.size())
        return 0;

    return ctx->globals[index].truthy() ? 1 : 0;
}

bool Assembler::isComparisonBranch(OpCode opcode)
{
    BranchInfo info{};
    return getBranchInfo(opcode, info);
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
        mov(EAX, dword_ptr(RSP, offset));
        jumpIfZero(EAX, instruction.operand);
        return true;
    }

    if (condition.isDouble())
    {
        movsd(XMM0, qword_ptr(RSP, offset));
        loadNumberToSimd(Slot::Double(0.0), false, 0, XMM1);
        compareDoubles(XMM0, XMM1);
        setComparisonResult(JitCondition::NotEqual, EAX);
        jumpIfZero(EAX, instruction.operand);
        return true;
    }

    if (condition.isValueIndex())
    {
        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, offset));
        mov(RAX, Immediate(&JitRuntimeValueTruthy));
        call(RAX);
        jumpIfZero(EAX, instruction.operand);
        return true;
    }

    return false;
}

bool Assembler::emitComparisonBranch(const Instruction &instruction, std::uint32_t target)
{
    if (ir == nullptr || target >= ir->instructions.size())
        return false;

    BranchInfo info{};
    if (!getBranchInfo(instruction.opcode, info))
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

    const std::uint32_t left = instruction.operand & 0xFFFF;
    const std::uint32_t right = instruction.operand >> 16;
    const auto condition = static_cast<std::uint32_t>(info.condition);

    switch (info.kind)
    {
    case BranchOperandKind::LocalLocal:
    {
        if (left >= ir->localCount || right >= ir->localCount)
            return false;

        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));
        mov(EDX, Immediate(left | (right << 16)));
        mov(ECX, Immediate(condition));
        mov(RAX, Immediate(&JitRuntimeCompareLocalLocal));
        call(RAX);
        jumpIfZero(EAX, target);
        return true;
    }

    case BranchOperandKind::LocalLiteral:
    {
        if (left >= ir->localCount || !loadLiteral(right, XMM0))
            return false;

        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));
        mov(EDX, Immediate(left | (condition << 16)));
        mov(RAX, Immediate(&JitRuntimeCompareLocalNumber));
        call(RAX);
        jumpIfZero(EAX, target);
        return true;
    }

    case BranchOperandKind::LiteralLocal:
    {
        if (!loadLiteral(right, XMM0) || left >= ir->localCount)
            return false;

        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));
        mov(EDX, Immediate(left | (condition << 16)));
        mov(RAX, Immediate(&JitRuntimeCompareNumberLocal));
        call(RAX);
        jumpIfZero(EAX, target);
        return true;
    }

    case BranchOperandKind::LocalRight:
    {
        if (jitCompilationStack.empty() || left >= ir->localCount)
            return false;

        const Slot leftValue = jitCompilationStack.back();
        if (!leftValue.isNumeric())
            return false;

        const MemoryOffset leftOffset =
            static_cast<MemoryOffset>((jitCompilationStack.size() - 1) * JIT_SLOT_SIZE_BYTES);

        loadNumberFromMemoryToSimd(leftValue, leftOffset, XMM0);

        loadJitContextPointerIntoRDI();
        mov(RSI, dword_ptr(RSP, jitMethodsOffset));
        mov(EDX, Immediate(left | (condition << 16)));
        mov(RAX, Immediate(&JitRuntimeCompareNumberLocal));
        call(RAX);

        jitCompilationStack.pop_back();
        jumpIfZero(EAX, target);
        return true;
    }

    case BranchOperandKind::LiteralRight:
    {
        if (jitCompilationStack.empty())
            return false;

        const Slot leftValue = jitCompilationStack.back();
        if (!leftValue.isNumeric())
            return false;

        const MemoryOffset leftOffset =
            static_cast<MemoryOffset>((jitCompilationStack.size() - 1) * JIT_SLOT_SIZE_BYTES);

        loadNumberFromMemoryToSimd(leftValue, leftOffset, XMM0);
        if (!loadLiteral(left, XMM1))
            return false;

        compareDoubles(XMM0, XMM1);
        setComparisonResult(info.condition, EAX);

        jitCompilationStack.pop_back();
        jumpIfZero(EAX, target);
        return true;
    }
    }

    return false;
}

bool Assembler::emitAddLiteralToLocalAndJump(const Instruction &instruction, std::uint32_t target)
{
    if (ir == nullptr || target >= ir->instructions.size())
        return false;

    const std::uint32_t sourceSlot = instruction.operand & 0xFF;
    const std::uint32_t literalIndex = (instruction.operand >> 8) & 0xFFFF;
    const std::uint32_t destinationSlot = (instruction.operand >> 24) & 0xFF;

    if (sourceSlot >= ir->localCount || destinationSlot >= ir->localCount || literalIndex >= ir->literals.size())
    {
        return false;
    }

    loadJitContextPointerIntoRDI();
    mov(RSI, dword_ptr(RSP, jitMethodsOffset));
    mov(EDX, Immediate(sourceSlot | (destinationSlot << 8)));
    mov(RCX, Immediate(reinterpret_cast<std::int64_t>(&ir->literals[literalIndex])));
    mov(RAX, Immediate(&JitRuntimeAddLocalLiteralStore));
    call(RAX);

    const JitValueType sourceType =
        sourceSlot < localVariableTypes.size() ? localVariableTypes[sourceSlot] : JitValueType::ValueIndex;
    const Value &literal = ir->literals[literalIndex];

    JitValueType resultType = JitValueType::ValueIndex;
    if ((sourceType == JitValueType::Int || sourceType == JitValueType::Double) && literal.IS_INT)
        resultType = sourceType;
    else if ((sourceType == JitValueType::Int || sourceType == JitValueType::Double) && literal.IS_DOUBLE)
        resultType = JitValueType::Double;

    recordLocalVarType(destinationSlot, resultType);

    jump(target);
    return true;
}

} // namespace jit
