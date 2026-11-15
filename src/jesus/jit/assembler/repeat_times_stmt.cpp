#include "assembler.hpp"

#include "jit/jit_context.hpp"

namespace jit
{

// Implemented in assembler_globals.cpp. Counted loops store their remaining
// count in JitContext::globals, just like an ordinary global variable.
extern "C" void JitRuntimeStoreGlobalInt(JitContext *ctx, std::uint32_t index, int value);

bool Assembler::emitInitRepeatCount(const Instruction &instruction)
{
    if (ir == nullptr || jitCompilationStack.empty())
        return false;

    // The VM rejects non-int counts when INIT_REPEAT_COUNT runs. Returning
    // false here leaves that error to the VM fallback instead of emitting a
    // JIT call that would have to throw across the assembly boundary.
    const Slot count = jitCompilationStack.back();
    if (!count.isInt())
        return false;

    const std::uint32_t index = instruction.operand;
    const MemoryOffset countOffset =
        static_cast<MemoryOffset>((jitCompilationStack.size() - 1) * JIT_SLOT_SIZE_BYTES);

    // ------------------------------------------------------------------
    // Pass the arguments to JitRuntimeStoreGlobalInt.
    //
    // System V x86-64:
    //      RDI: ctx    (first argument)
    //      RSI: index  (second argument)
    //      RDX: count  (third argument)
    //      RAX: address of the C++ helper to call
    // ------------------------------------------------------------------

    // First argument (RDI): JitContext* used to reach JitContext::globals.
    loadJitContextPointerIntoRDI();

    // Second argument (RSI): the compiler-generated remaining-count slot.
    mov(RSI, Immediate(index));

    // Third argument (EDX): the int count held in the JIT value stack.
    mov(EDX, dword_ptr(RSP, countOffset));

    // Call the helper through RAX.
    mov(RAX, Immediate(&JitRuntimeStoreGlobalInt));
    call(RAX);

    // Subsequent READ_GLOBAL instructions expect the counted-loop slot to be
    // an int, so keep the assembler's global type information in sync.
    recordGlobalVarType(index, JitValueType::Int);

    // INIT_REPEAT_COUNT consumes the count value; it leaves nothing behind.
    jitCompilationStack.pop_back();
    return true;
}

} // namespace jit
