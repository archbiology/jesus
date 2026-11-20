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
// JIT runtime helper for 'repeat while'.
//
// The generated assembly cannot safely perform a generic Value addition in
// registers, so it calls back into C++ for:
//
//      local[destination] = local[source] + literal
//
// packedOperand layout (same as ADD_LOCAL_LITERAL_STORE):
//      bits 0-7    source local slot
//      bits 8-23   literal index
//      bits 24-31  destination local slot
// --------------------------------------------------------------------------
extern "C" void JitRuntimeUpdateLoopVariable(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t packedOperand, const Value *literal)
{
    const std::uint32_t sourceSlot = extractLower8Bits(packedOperand);
    const std::uint32_t destinationSlot = extractUpper8Bits(packedOperand);

    // ------------
    // "i = i + 1"
    // ------------
    localAt(ctx, baseSlot, destinationSlot) = localAt(ctx, baseSlot, sourceSlot) + *literal;
}

bool Assembler::emitRepeatWhile(const Instruction &instruction, std::uint32_t jumpTarget)
{
    if (ir == nullptr || jumpTarget >= ir->instructions.size())
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

    // ------------------------------------------------
    // Pass the arguments to JitRuntimeUpdateLoopVariable.
    //
    // System V x86-64 argument registers:
    //      RDI/EDI: ctx             (first argument)
    //      RSI/ESI: baseSlot        (second argument)
    //      RDX/EDX: packedOperand   (third argument)
    //      RCX/ECX: literal pointer (fourth argument)
    //      RAX: address of the C++ helper to call
    // ------------------------------------------------
    // Load 'JitContext *ctx' into RDI
    // -------------------------------
    loadJitContextPointerIntoRDI();

    // ------------------------
    // Load `baseSlot` into RSI
    // ------------------------
    mov(RSI, dword_ptr(RSP, jitMethodsOffset));

    // -----------------------------
    // Load `packedOperand` into EDX
    // -----------------------------
    mov(EDX, Immediate(instruction.operand));

    // ------------------------------
    // Load `Value *literal` into RCX
    // ------------------------------
    mov(RCX, Immediate(reinterpret_cast<std::int64_t>(&literal)));

    // ---------------------------
    // Call JitRuntimeUpdateLoopVariable
    // ---------------------------
    mov(RAX, Immediate(&JitRuntimeUpdateLoopVariable));
    call(RAX);

    // ---------------------------------------
    // Go to the beginning of the repeat-while
    // ---------------------------------------
    jump(jumpTarget);
    return true;
}

bool Assembler::emitRepeatNTimes(const Instruction &instruction)
{
    if (jitCompilationStack.empty() || !jitCompilationStack.back().isInt())
        return false;

    return emitStoreGlobal(instruction);
}

} // namespace jit
