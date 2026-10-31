#include "assembler.hpp"

#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

namespace jit
{

// --------------------------------------------------------------------------
// JIT runtime helper functions
//
// The generated assembly code cannot directly create our C++ Value objects.
// Instead, it passes the global variable's index and its raw value
// (integer or double) to these functions.
//
// These functions create the corresponding Value and store it in
// JitContext.globals.
//
// CREATE_GLOBAL and WRITE_GLOBAL use the same storage functions.
// The vector grows automatically when a new global variable is added.
//
// According to System V:
//      RDI/EDI: ctx    (first arg)
//      RSI:     index  (second arg)
//      RDX/EDX: value  (third arg)
// --------------------------------------------------------------------------
extern "C" void JitRuntimeStoreGlobalInt(JitContext *ctx, std::uint32_t index, int value)
{
    if (index >= ctx->globals.size())
        ctx->globals.resize(index + 1);

    ctx->globals[index] = Value(value);
}

extern "C" void JitRuntimeStoreGlobalDouble(JitContext *ctx, std::uint32_t index, double value)
{
    if (index >= ctx->globals.size())
        ctx->globals.resize(index + 1);

    ctx->globals[index] = Value(value);
}

extern "C" int JitRuntimeReadGlobalInt(JitContext *ctx, std::uint32_t index)
{
    return ctx->globals[index].intValue;
}

extern "C" double JitRuntimeReadGlobalDouble(JitContext *ctx, std::uint32_t index)
{
    return ctx->globals[index].number;
}

void Assembler::recordGlobalVarType(std::uint32_t index, JitValueType valueType)
{
    if (index >= globalVariableTypes.size())
        globalVariableTypes.resize(index + 1);

    globalVariableTypes[index] = valueType;
}

void Assembler::loadJitContextPointerIntoRDI()
{
    // --------------------------------------------------------------
    // System V: RDI/EDI contains the first argument of a function.
    //
    // Retrieve the JitContext pointer saved in the JIT runtime stack
    // and place it in RDI, the first argument register.
    // --------------------------------------------------------------
    mov(RDI, qword_ptr(RSP, jitContextOffset));
}

bool Assembler::emitStoreGlobal(const Instruction &instruction)
{
    if (jitStack.empty())
        return false;

    const Slot value = jitStack.back();
    const MemoryOffset valueOffset = static_cast<MemoryOffset>((jitStack.size() - 1) * JIT_SLOT_SIZE_BYTES);
    const std::uint32_t index = instruction.operand;

    // -----------------------------------------------------
    // Load the args of the function JitRuntimeStoreGlobal*
    //      RDI/EDI: first argument  (JitContext)
    //      RSI/ESI: second argument (index)
    //      RDX/EDX: third argument  (value)
    //
    //      XMM0:    third argument  (value)
    // -----------------------------------------------------
    // Pass the `context` value as the first argument (RDI)
    // -----------------------------------------------------
    loadJitContextPointerIntoRDI();

    // -----------------------------------------------------
    // Pass the `index` value as the second argument (RSI)
    // --------------------------------------------------
    mov(RSI, Immediate(index));

    if (value.isInt())
    {
        // -------------------------------------------------------
        // Pass the integer `value` as the third argument (EDX)
        // and put in RAX the address of the function to be called
        // -------------------------------------------------------
        mov(EDX, dword_ptr(RSP, valueOffset));
        mov(RAX, Immediate(&JitRuntimeStoreGlobalInt));
    }
    else
    {
        // -------------------------------------------------------------------
        // Pass the double `value` as the first floating-point argument (XMM0)
        // and put in RAX the address of the function to be called
        // -------------------------------------------------------------------
        movsd(XMM0, qword_ptr(RSP, valueOffset));
        mov(RAX, Immediate(&JitRuntimeStoreGlobalDouble));
    }

    // -------------------------------------------------------------
    // Call the function whose address is in RAX to store the global
    // -------------------------------------------------------------
    call(RAX);

    recordGlobalVarType(index, value.valueType);
    jitStack.pop_back();
    return true;
}

bool Assembler::emitReadGlobal(const Instruction &instruction)
{
    const std::uint32_t index = instruction.operand;
    if (index >= globalVariableTypes.size())
        return false;

    const JitValueType valueType = globalVariableTypes[index];
    const MemoryOffset destOffset = static_cast<MemoryOffset>(jitStack.size() * JIT_SLOT_SIZE_BYTES);

    // -----------------------------------------------------
    // Pass the `context` value as the first argument (RDI)
    // -----------------------------------------------------
    loadJitContextPointerIntoRDI();

    // -----------------------------------------------------
    // Pass the `index` value as the second argument (RSI)
    // --------------------------------------------------
    mov(RSI, Immediate(index));

    if (valueType == JitValueType::Int)
    {
        // -------------------------
        // Read the global int value
        // -------------------------
        mov(RAX, Immediate(&JitRuntimeReadGlobalInt));
        call(RAX);

        // ----------------------
        // Return value is in EAX
        // ----------------------
        mov(dword_ptr(RSP, destOffset), EAX);
        pushSlot(Slot{JitValueType::Int});
    }
    else
    {
        // ----------------------------
        // Read the global double value
        // ----------------------------
        mov(RAX, Immediate(&JitRuntimeReadGlobalDouble));
        call(RAX);

        // ------------------------
        // Return value is in XMM0
        // ------------------------
        movsd(qword_ptr(RSP, destOffset), XMM0);
        pushSlot(Slot{JitValueType::Double});
    }

    return true;
}

} // namespace jit
