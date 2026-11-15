#include "assembler.hpp"

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

} // namespace

// --------------------------------------------------------------------------
// JIT runtime helper for the fused loop tail.
//
// The generated assembly cannot safely perform a generic Value addition in
// registers, so it calls back into C++ for:
//
//      local[destination] = local[source] + literal
//
// System V x86-64 argument registers:
//      RDI/EDI: ctx             (first argument)
//      RSI/ESI: baseSlot        (second argument)
//      RDX/EDX: packedOperand   (third argument)
//      RCX/ECX: literal pointer (fourth argument)
//
// packedOperand layout (same as ADD_LOCAL_LITERAL_STORE):
//      bits 0-7    source local slot
//      bits 8-23   literal index
//      bits 24-31  destination local slot
// --------------------------------------------------------------------------
extern "C" void JitRuntimeAddLocalLiteralStore(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t packedOperand, const Value *literal)
{
    const std::uint32_t sourceSlot = packedOperand & 0xFF;
    const std::uint32_t destinationSlot = (packedOperand >> 24) & 0xFF;

    // Read both operands before assigning, so source and destination may be
    // the same slot (the common `i = i + 1` case).
    localAt(ctx, baseSlot, destinationSlot) = localAt(ctx, baseSlot, sourceSlot) + *literal;
}

bool Assembler::emitAddLocalLiteralStore(const Instruction &instruction)
{
    if (ir == nullptr)
        return false;

    const std::uint32_t sourceSlot = instruction.operand & 0xFF;
    const std::uint32_t literalIndex = (instruction.operand >> 8) & 0xFFFF;
    const std::uint32_t destinationSlot = (instruction.operand >> 24) & 0xFF;

    if (ir->localCount == 0 || sourceSlot >= ir->localCount || destinationSlot >= ir->localCount ||
        literalIndex >= ir->literals.size())
    {
        return false;
    }

    const Value &literal = ir->literals[literalIndex];

    // ------------------------------------------------------------------
    // Pass the arguments to JitRuntimeAddLocalLiteralStore.
    //
    // System V x86-64:
    //      RDI: ctx
    //      RSI: baseSlot (the current method frame's first local slot)
    //      RDX: packed source/literal/destination operand
    //      RCX: address of the literal Value
    //      RAX: address of the C++ helper to call
    // ------------------------------------------------------------------

    // First argument (RDI): JitContext* used to reach jitRuntimeStack.
    loadJitContextPointerIntoRDI();

    // Second argument (RSI): base slot of the current method's locals.
    mov(RSI, dword_ptr(RSP, jitMethodsOffset));

    // Third argument (RDX): source slot, literal index and destination slot.
    mov(EDX, Immediate(instruction.operand));

    // Fourth argument (RCX): pointer to the literal Value held by the chunk.
    mov(RCX, Immediate(reinterpret_cast<std::int64_t>(&literal)));

    // Call the helper through RAX.
    mov(RAX, Immediate(&JitRuntimeAddLocalLiteralStore));
    call(RAX);

    // Keep local type information accurate for instructions that read this
    // local after the loop. Numeric locals keep their type when the literal is
    // numeric; anything else becomes a generic Value.
    const JitValueType sourceType =
        sourceSlot < localVariableTypes.size() ? localVariableTypes[sourceSlot] : JitValueType::ValueIndex;

    JitValueType resultType = JitValueType::ValueIndex;
    if ((sourceType == JitValueType::Int || sourceType == JitValueType::Double) && literal.IS_INT)
        resultType = sourceType;
    else if ((sourceType == JitValueType::Int || sourceType == JitValueType::Double) && literal.IS_DOUBLE)
        resultType = JitValueType::Double;

    recordLocalVarType(destinationSlot, resultType);

    return true;
}

bool Assembler::emitAddLocalLiteralStoreJump(const Instruction &instruction, std::uint32_t target)
{
    if (ir == nullptr || target >= ir->instructions.size())
        return false;

    if (!emitAddLocalLiteralStore(instruction))
        return false;

    // Fused back-edge: jump to the loop condition without dispatching the
    // target-holder JUMP that follows this instruction in the IR.
    jump(target);
    return true;
}

} // namespace jit
