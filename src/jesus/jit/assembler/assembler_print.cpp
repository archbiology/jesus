#include "assembler.hpp"
#include "spirit/value.hpp"

namespace jit
{
// --------------------------------------------------------------------------
// Runtime helpers called from JIT machine code.
//
// They live on the C++ side and reproduce exactly the VM's PRINT formatting
// (`Value::toString()`), so `--jit` and `--vm` produce identical output.
// --------------------------------------------------------------------------
extern "C" void JitRuntimePrintInt(int value)
{
    std::cout << Value(value).toString() << std::endl;
}

extern "C" void JitRuntimePrintDouble(double value)
{
    std::cout << Value(value).toString() << std::endl;
}

extern "C" void JitRuntimePrintValue(JitContext *ctx, std::uint32_t index)
{
    std::cout << ctx->globals[index].toString() << std::endl;
}

bool Assembler::emitPrint()
{
    if (jitStack.empty())
        return false;

    const Slot value = jitStack.back();
    jitStack.pop_back();

    const MemoryOffset offset = static_cast<MemoryOffset>(jitStack.size() * JIT_SLOT_SIZE_BYTES);

    if (value.isInt())
    {
        // ----------------------------------------------------
        // Move the 32-bit integer value from memory into EDI.
        //
        // Why EDI and not another register?
        //    The 'System V calling convention' specifies that the
        //    first integer argument is passed in EDI
        //    EDI = the lower 32 bits of 64-bit RDI.
        //
        //    The C++ compiler follows this same rule when
        //    generating the machine code for our print function.
        //    Therefore, the function reads its int argument from
        //    EDI when we call it.
        //
        //    For a 64-bit integer argument, the register would
        //    be RDI instead.
        // ----------------------------------------------------
        mov(EDI, dword_ptr(RSP, offset));

        // ---------------------------------------------
        // Load the address of our print function into RAX
        // ---------------------------------------------
        mov(RAX, Immediate(&JitRuntimePrintInt));
    }
    else if (value.isDouble())
    {
        // -------------------------------------------------------------
        // Move the 64-bit double value from memory into XMM0.
        // XMM0 = the 64 lower bits from SIMD
        //
        //  Why XMM0 and not another register?
        //    The 'System V calling convention' specifies that the
        //    first floating-point argument is passed in XMM0.
        //
        //    Although XMM0 is 128 bits wide, a double uses only
        //    its lower 64 bits.
        //
        //    The C++ compiler follows this same rule when
        //    generating the machine code for our print function.
        //    Therefore, the function reads its double argument
        //    from XMM0 when we call it.
        // -------------------------------------------------------------
        movsd(XMM0, qword_ptr(RSP, offset));

        // ---------------------------------------------
        // Load the address of our print function into RAX
        // ---------------------------------------------
        mov(RAX, Immediate(&JitRuntimePrintDouble));
    }
    else
    {
        // ---------------------------------------------------------
        // Then it is a ValueIndex (an index to an object)
        //
        // Fist, load JitContext for `JitRuntimePrintValue` into RDI
        // ---------------------------------------------------------
        loadJitContextPointerIntoRDI();

        // --------------------------------------
        // Load the second arg (`index`) into RSI
        // --------------------------------------
        mov(RSI, dword_ptr(RSP, offset));

        // ---------------------------------------------
        // Load the address of our print function to RAX
        // ---------------------------------------------
        mov(RAX, Immediate(&JitRuntimePrintValue));
    }

    // -----------------------------------------------
    // Call the print function whose address in in RAX
    // -----------------------------------------------
    call(RAX);

    return true;
}

} // namespace jit
