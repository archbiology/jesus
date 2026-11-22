#include "assembler.hpp"

#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

namespace jit
{

namespace
{

Value &localAt(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot)
{
    // The engine and method dispatcher size the frame before entering JIT code.
    return ctx->jitRuntimeStack[static_cast<std::size_t>(baseSlot) + slot];
}

// Derive offsets from the actual Value layout without requiring standard layout.
template <typename Field>
MemoryOffset localFieldOffset(std::uint32_t slot, Field Value::*field)
{
    const Value value;
    const auto address = reinterpret_cast<std::uintptr_t>(&(value.*field));
    const auto base = reinterpret_cast<std::uintptr_t>(&value);
    return static_cast<MemoryOffset>(slot * sizeof(Value) + (address - base));
}

} // namespace

// --------------------------------------------------------------------------
// JIT runtime helper functions
//
// The generated assembly code cannot directly create our C++ Value objects.
// Instead, it passes the base slot, slot index, and raw value
// (integer or double) to these functions.
//
// These functions create the corresponding Value and store it in
// JitContext.jitRuntimeStack.
//
// According to System V:
//      RDI/EDI: ctx      (first arg)
//      RSI/ESI: baseSlot (second arg)
//      RDX/EDX: slot     (third arg)
//      RCX/ECX: value    (fourth arg)
// --------------------------------------------------------------------------
extern "C" void JitRuntimeStoreLocalInt(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot, int value)
{
    localAt(ctx, baseSlot, slot) = Value(value);
}

extern "C" void JitRuntimeUpdateLocalDouble(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot, double value)
{
    localAt(ctx, baseSlot, slot) = Value(value);
}

extern "C" void JitRuntimeUpdateLocalValue(
    JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot, std::uint32_t globalIndex)
{
    localAt(ctx, baseSlot, slot) = ctx->globals[globalIndex];
}

extern "C" int JitRuntimeReadLocalInt(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot)
{
    return localAt(ctx, baseSlot, slot).intValue;
}

extern "C" double JitRuntimeReadLocalDouble(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot)
{
    return localAt(ctx, baseSlot, slot).doubleValue;
}

extern "C" std::uint32_t JitRuntimeCopyLocalValue(JitContext *ctx, std::uint32_t baseSlot, std::uint32_t slot)
{
    ctx->globals.push_back(localAt(ctx, baseSlot, slot));
    return static_cast<std::uint32_t>(ctx->globals.size() - 1);
}

// RDX addresses the current boxed frame until the next runtime call.
void Assembler::loadFrameLocals()
{
    loadJitContextPointerIntoRDI();
    mov(RDX, qword_ptr(RDI, offsetof(JitContext, frameLocals)));
}

void Assembler::guardLocalType(std::uint32_t slot, std::uint16_t mask, std::uint32_t fallback)
{
    jumpIfMaskClear(RDX, localFieldOffset(slot, &Value::__flags), mask, fallback);
}

QwordMemory Assembler::localDouble(std::uint32_t slot)
{
    return qword_ptr(RDX, localFieldOffset(slot, &Value::doubleValue));
}

DwordMemory Assembler::localInt(std::uint32_t slot)
{
    return dword_ptr(RDX, localFieldOffset(slot, &Value::intValue));
}

void Assembler::recordLocalVarType(std::uint32_t index, JitValueType valueType)
{
    if (index >= localVariableTypes.size())
        localVariableTypes.resize(index + 1, JitValueType::StackIndex);

    localVariableTypes[index] = valueType;
}

bool Assembler::emitStoreLocal(const Instruction &instruction)
{
    if (ir == nullptr || jitCompilationStack.empty() || instruction.operand >= ir->localCount)
        return false;

    const Slot value = jitCompilationStack.back();
    const MemoryOffset valueOffset = static_cast<MemoryOffset>((jitCompilationStack.size() - 1) * JIT_SLOT_SIZE_BYTES);
    const std::uint32_t index = instruction.operand;

    // -----------------------------------------------------
    // Load the args of the function JitRuntimeStoreLocal*
    //      RDI/EDI: first argument  (JitContext)
    //      RSI/ESI: second argument (baseSlot)
    //      RDX/EDX: third argument  (slot)
    //      RCX/ECX: fourth argument (value)
    //
    //      XMM0:    first double    (value)
    // -----------------------------------------------------
    // Pass the `context` value as the first argument (RDI)
    // -----------------------------------------------------
    loadJitContextPointerIntoRDI();

    // -----------------------------------------------------
    // Pass the `baseSlot` value as the second argument (RSI)
    // -----------------------------------------------------
    mov(RSI, dword_ptr(RSP, jitMethodsOffset));

    // -----------------------------------------------------------
    // Pass the `slot` (`index`) value as the third argument (EDX)
    // -----------------------------------------------------------
    mov(EDX, Immediate(index));

    if (value.isInt())
    {
        // -------------------------------------------------------
        // Pass the integer `value` as the fourth argument (ECX)
        // and put in RAX the address of the function to be called
        // -------------------------------------------------------
        mov(ECX, dword_ptr(RSP, valueOffset));
        mov(RAX, Immediate(&JitRuntimeStoreLocalInt));
    }
    else if (value.isDouble())
    {
        // -------------------------------------------------------------------
        // Pass the double `value` as the first floating-point argument (XMM0)
        // and put in RAX the address of the function to be called
        // -------------------------------------------------------------------
        movsd(XMM0, qword_ptr(RSP, valueOffset));
        mov(RAX, Immediate(&JitRuntimeUpdateLocalDouble));
    }
    else if (value.isStackIndex())
    {
        // -------------------------------------------------
        // Pass the Value index as the fourth argument (ECX)
        // -------------------------------------------------
        mov(ECX, dword_ptr(RSP, valueOffset));
        mov(RAX, Immediate(&JitRuntimeUpdateLocalValue));
    }
    else
    {
        return false;
    }

    // -------------------------------------------------------------
    // Call the function whose address is in RAX to store the local
    // -------------------------------------------------------------
    call(RAX);

    recordLocalVarType(index, value.valueType);
    jitCompilationStack.pop_back();
    return true;
}

bool Assembler::emitReadLocal(const Instruction &instruction)
{
    if (ir == nullptr || instruction.operand >= ir->localCount)
        return false;

    const std::uint32_t index = instruction.operand;
    const JitValueType valueType = readVarType(index, localVariableTypes, localsToReadFromStack);
    const MemoryOffset destOffset = static_cast<MemoryOffset>(jitCompilationStack.size() * JIT_SLOT_SIZE_BYTES);

    // -----------------------------------------------------
    // Pass the `context` value as the first argument (RDI)
    // -----------------------------------------------------
    loadJitContextPointerIntoRDI();

    // -----------------------------------------------------
    // Pass the `baseSlot` value as the second argument (RSI)
    // -----------------------------------------------------
    mov(RSI, dword_ptr(RSP, jitMethodsOffset));

    // -----------------------------------------------------------
    // Pass the `slot` (`index`) value as the third argument (EDX)
    // -----------------------------------------------------------
    mov(EDX, Immediate(index));

    if (valueType == JitValueType::Int)
    {
        // -------------------------
        // Read the local int value
        // -------------------------
        mov(RAX, Immediate(&JitRuntimeReadLocalInt));
        call(RAX);

        // ----------------------
        // Return value is in EAX
        // ----------------------
        mov(dword_ptr(RSP, destOffset), EAX);
        pushSlot(Slot{JitValueType::Int});
    }
    else if (valueType == JitValueType::Double)
    {
        // ----------------------------
        // Read the local double value
        // ----------------------------
        mov(RAX, Immediate(&JitRuntimeReadLocalDouble));
        call(RAX);

        // ------------------------
        // Return value is in XMM0
        // ------------------------
        movsd(qword_ptr(RSP, destOffset), XMM0);
        pushSlot(Slot{JitValueType::Double});
    }
    else
    {
        // ------------------------------------------------
        // Read the local Value and return its copied index
        // ------------------------------------------------
        mov(RAX, Immediate(&JitRuntimeCopyLocalValue));
        call(RAX);

        // --------------------------------
        // Return value is a Value index
        // --------------------------------
        mov(dword_ptr(RSP, destOffset), EAX);
        pushSlot(Slot::StackIndex());
    }

    return true;
}

} // namespace jit
