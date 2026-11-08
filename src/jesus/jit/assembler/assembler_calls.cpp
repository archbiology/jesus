#include "assembler.hpp"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <utility>

#include "ast/stmt/create_method_stmt.hpp"
#include "interpreter/runtime/method.hpp"
#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

namespace jit
{

// --------------------------------------------------------------------------
// CALL / RETURN runtime helpers
//
// The function that makes the call first pushes the instance and arguments
// from its native value slots onto JitContext::jitRuntimeStack.
// JitRuntimeCallMethod then performs the same frame setup as OP_CALL in the VM
// and invokes the native entry point of the function being called.
//
// On RETURN, the function being called replaces its frame with a single
// result Value. The function that made the call copies that result into
// globals and keeps its index in a native slot, so PRINT and the other value
// helpers can consume it without knowing whether the function returned
// a number or an object.
// --------------------------------------------------------------------------

extern "C" void JitRuntimePushInt(JitContext *ctx, int value)
{
    ctx->jitRuntimeStack.push_back(Value(value));
}

extern "C" void JitRuntimePushDouble(JitContext *ctx, double value)
{
    ctx->jitRuntimeStack.push_back(Value(value));
}

extern "C" void JitRuntimePushValue(JitContext *ctx, std::uint32_t valueIndex)
{
    ctx->jitRuntimeStack.push_back(ctx->globals[valueIndex]);
}

extern "C" void JitRuntimeCallMethod(JitContext *ctx, std::uint32_t targetIndex)
{
    if (targetIndex >= ctx->jitMethods.size())
        return;

    const JitMethod &method = ctx->jitMethods[targetIndex];
    if (method.entrypoint == nullptr || ctx->jitRuntimeStack.size() < method.paramsCount + 1)
        return;

    const std::size_t baseSlot = ctx->jitRuntimeStack.size() - (method.paramsCount + 1);
    ctx->jitRuntimeStack.resize(baseSlot + method.localCount);

    const std::size_t previousBase = ctx->currentMethodBaseSlot;
    ctx->currentMethodBaseSlot = baseSlot;

    // ------------------
    // Execute the method
    // ------------------
    method.entrypoint(ctx, static_cast<std::uint32_t>(baseSlot));

    ctx->currentMethodBaseSlot = previousBase;
}

extern "C" std::uint32_t JitRuntimeMoveReturnValueToGlobals(JitContext *ctx)
{
    if (ctx->jitRuntimeStack.empty())
        return 0;

    Value result = std::move(ctx->jitRuntimeStack.back());
    ctx->jitRuntimeStack.pop_back();

    ctx->globals.push_back(std::move(result));
    return static_cast<std::uint32_t>(ctx->globals.size() - 1);
}

static void putReturnValueOnStack(JitContext *ctx, std::uint32_t baseSlot, Value result)
{
    ctx->jitRuntimeStack.resize(baseSlot);
    ctx->jitRuntimeStack.push_back(std::move(result));
}

extern "C" void JitRuntimeReturnInt(JitContext *ctx, std::uint32_t baseSlot, int value)
{
    putReturnValueOnStack(ctx, baseSlot, Value(value));
}

extern "C" void JitRuntimeReturnDouble(JitContext *ctx, std::uint32_t baseSlot, double value)
{
    putReturnValueOnStack(ctx, baseSlot, Value(value));
}

extern "C" void JitRuntimeReturnValue(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t valueIndex)
{
    putReturnValueOnStack(ctx, baseSlot, ctx->globals[valueIndex]);
}

bool Assembler::emitCall(const Instruction &instruction)
{
    if (ir == nullptr || instruction.operand >= ir->literals.size())
        return false;

    const Value &literal = ir->literals[instruction.operand];
    if (!literal.IS_METHOD)
        return false;

    const auto method = std::dynamic_pointer_cast<Method>(literal.asMethod());
    if (!method || method->definition == nullptr || method->definition->compiledBody == nullptr)
        return false;

    if (chunksMap == nullptr)
        return false;

    const auto target = chunksMap->find(method->definition->compiledBody.get());
    if (target == chunksMap->end())
        return false;

    const std::uint32_t paramsCount = method->params ? method->params->paramsCount : 0;
    if (jitCompilationStack.size() < paramsCount + 1)
        return false;

    const std::size_t baseIndex = jitCompilationStack.size() - (paramsCount + 1);

    for (std::size_t index = baseIndex; index < jitCompilationStack.size(); ++index)
    {
        const Slot &slot = jitCompilationStack[index];
        const MemoryOffset offset = static_cast<MemoryOffset>(index * JIT_SLOT_SIZE_BYTES);

        loadJitContextPointerIntoRDI();

        if (slot.isInt())
        {
            mov(RSI, dword_ptr(RSP, offset));
            mov(RAX, Immediate(&JitRuntimePushInt));
        }
        else if (slot.isDouble())
        {
            movsd(XMM0, qword_ptr(RSP, offset));
            mov(RAX, Immediate(&JitRuntimePushDouble));
        }
        else if (slot.isValueIndex())
        {
            mov(RSI, dword_ptr(RSP, offset));
            mov(RAX, Immediate(&JitRuntimePushValue));
        }
        else
        {
            return false;
        }

        call(RAX);
    }

    jitCompilationStack.resize(baseIndex);

    loadJitContextPointerIntoRDI();
    mov(RSI, Immediate(target->second));
    mov(RAX, Immediate(&JitRuntimeCallMethod));
    call(RAX);

    loadJitContextPointerIntoRDI();
    mov(RAX, Immediate(&JitRuntimeMoveReturnValueToGlobals));
    call(RAX);

    const MemoryOffset resultOffset = static_cast<MemoryOffset>(baseIndex * JIT_SLOT_SIZE_BYTES);
    mov(dword_ptr(RSP, resultOffset), EAX);
    pushSlot(Slot::ValueIndex());
    return true;
}

bool Assembler::emitReturn()
{
    if (jitCompilationStack.empty())
        return false;

    const Slot result = jitCompilationStack.back();
    const MemoryOffset offset = static_cast<MemoryOffset>((jitCompilationStack.size() - 1) * JIT_SLOT_SIZE_BYTES);

    loadJitContextPointerIntoRDI();
    mov(RSI, dword_ptr(RSP, jitMethodsOffset));

    if (result.isInt())
    {
        mov(EDX, dword_ptr(RSP, offset));
        mov(RAX, Immediate(&JitRuntimeReturnInt));
    }
    else if (result.isDouble())
    {
        movsd(XMM0, qword_ptr(RSP, offset));
        mov(RAX, Immediate(&JitRuntimeReturnDouble));
    }
    else if (result.isValueIndex())
    {
        mov(EDX, dword_ptr(RSP, offset));
        mov(RAX, Immediate(&JitRuntimeReturnValue));
    }
    else
    {
        return false;
    }

    call(RAX);
    jitCompilationStack.pop_back();
    return true;
}

} // namespace jit
