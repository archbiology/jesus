#include "x86_assembler.hpp"

#include <cstdint>
#include <stdexcept>

#include <asmjit/x86.h>

namespace jit
{

std::unique_ptr<ExecutableCode> X86Assembler::assemble(const IR &ir)
{
    std::size_t maxSlots = 0;
    if (!validateSupportedJitOperations(ir, maxSlots))
        return nullptr;

    // ----------------------------------------------------------
    // Destroy the previous code holder before creating a new one.
    // ----------------------------------------------------------
    assembler.reset();
    code.reset();

    auto runtime = std::make_shared<asmjit::JitRuntime>();

    code = std::make_unique<asmjit::CodeHolder>();
    if (code->init(runtime->environment()) != asmjit::Error::kOk)
        return nullptr;

    assembler = std::make_unique<asmjit::x86::Assembler>(code.get());

    // --------------------------------------------------------------------
    // Reserve C++ stack memory for the JIT's temporary values plus one
    // extra slot that holds the JitContext pointer passed in RDI.
    // Each JIT-compiler stack slot occupies 8 bytes (64 bits).
    // Example:
    //    RSP
    //     |
    //     +---------------------------+
    //     | Value slot 0 (8 bytes)    | Offset 0
    //     +---------------------------+
    //     | Value slot 1 (8 bytes)    | Offset 8
    //     +---------------------------+
    //     | Value slot 2 (8 bytes)    | Offset 16
    //     +---------------------------+
    //     | Value slot 3 (8 bytes)    | Offset 24
    //     +---------------------------+
    //     | JitContext* (8 bytes)     | Offset 32  <-- jitContextOffset
    //     +---------------------------+
    // --------------------------------------------------------------------
    jitContextOffset = static_cast<MemoryOffset>(maxSlots * JIT_SLOT_SIZE_BYTES);
    const std::size_t usedBytes = (maxSlots + 1) * JIT_SLOT_SIZE_BYTES;

    // ---------------------------------------------------------------
    // Before calling our function, the CPU put its 8-byte (64 bits)
    // return address to the top of the stack
    // Imagine this function:
    //      void functionA() {
    //        functionB();
    //        say("I'm back in function A!");
    //      }
    //
    // Imagine the instructions in memory:
    //
    //      Memory address     Instruction
    //      ----------------------------------------
    //      1000               Call functionB
    //      1005               Say "I'm back in function A!"
    //      1010               ...
    //                         ...
    //      2000               void functionB () {
    //      2005                 Do something
    //      2010               Return }
    //
    // When the CPU executes call functionB at address 1000, it pushes 1005 onto the stack.
    // Why 1005?
    //    Because that's where execution must continue after functionB() finishes.
    //    The CPU then jumps to address 2000.
    //    When functionB() executes ret, the CPU retrieves 1005 from the stack
    //    and continues execution there.
    // ---------------------------------------------------------------
    constexpr std::size_t X86_64_RETURN_ADDRESS_SIZE_BYTES = 8;
    const std::size_t frameSize = ((usedBytes + 15) / 16) * 16 + X86_64_RETURN_ADDRESS_SIZE_BYTES;

    // -----------------------------------------------------------------------
    // Step 1: Reserve space for the JIT runtime values.
    //
    // At runtime, move the stack pointer to the beginning of the memory area
    // reserved for the JIT runtime (the JIT runtime stack), because when
    // the JIT runtime is called, RSP is "at the end of the stack".
    //
    // The x86-64 stack grows toward LOWER memory addresses.
    //
    // Imagine a stack with 90 bytes, numbered from 0 to 89:
    //
    // Higher addresses
    //       89  <-- RSP starts here
    //       88
    //       87
    //       ...
    //        2
    //        1
    //        0
    // Lower addresses
    //
    // If a function needs 2 bytes, RSP moves from address 89 to address 87.
    // It does NOT move from address 0 to address 2.
    //
    // This is a convention adopted by the x86 architecture. The stack grows
    // downward, so we use SUB to reserve space and ADD to release it.
    //
    // Before SUB:
    //       89  <-- RSP
    //       88
    //       87
    //
    // After SUB RSP, 2:
    //       89
    //       88
    //       87  <-- RSP
    //
    // This subtraction is not executed while compiling the JIT code.
    // It becomes an assembly instruction that executes when the generated
    // assembly function is called with code->execute() by the JitEngine.
    // -----------------------------------------------------------------------
    sub(RSP, Immediate(static_cast<std::int64_t>(frameSize)));

    // --------------------------------------------------------
    // Store the address contained in RDI at the memory
    // location identified by RSP + jitContextOffset.
    //
    // System V: RDI contains the first function argument.
    // When the following code is called by the JitEngine:
    //
    //      JitContext context;
    //      code->execute(context);
    //
    // RDI contains the address of JitContext*.
    //
    // We save the JitContext address received in RDI
    // into the JIT runtime stack for later use, because
    // other function calls may change RDI.
    // ---------------------------------------------------
    mov(qword_ptr(RSP, jitContextOffset), RDI);

    // -----------------------------------------------
    // Step 2: generate the assembly for the user code
    // -----------------------------------------------
    if (!compileToMachineCode(ir))
        return nullptr;

    // -----------------------------------------------
    // Step 3: return the RSP to the original position
    // -----------------------------------------------
    add(RSP, Immediate(static_cast<std::int64_t>(frameSize)));

    // -------------------------------------------------------
    // Emit the RETURN code that ends the JIT-runtime function
    // -------------------------------------------------------
    ret();

    if (assembler->finalize() != asmjit::Error::kOk)
        return nullptr;

    ExecutableCode::Entrypoint entrypoint = nullptr;
    if (runtime->add(&entrypoint, code.get()) != asmjit::Error::kOk || entrypoint == nullptr)
        return nullptr;

    return std::make_unique<ExecutableCode>(entrypoint, std::move(runtime));
}

static asmjit::x86::Gp toAsmJit(Register reg)
{
    using namespace asmjit::x86;

    switch (reg)
    {
    case Register::RAX:
        return rax;
    case Register::EAX:
        return eax;
    case Register::RBX:
        return rbx;
    case Register::RCX:
        return rcx;
    case Register::ECX:
        return ecx;
    case Register::RDX:
        return rdx;
    case Register::EDX:
        return edx;
    case Register::RSI:
        return rsi;
    case Register::RDI:
        return rdi;
    case Register::EDI:
        return edi;
    case Register::RBP:
        return rbp;
    case Register::RSP:
        return rsp;

    default:
        throw std::invalid_argument("Register is not a general-purpose x86_64 register");
    }
}

static asmjit::x86::Vec toAsmJit(SimdRegister reg)
{
    using namespace asmjit::x86;

    switch (reg)
    {
    case SimdRegister::XMM0:
        return xmm0;
    case SimdRegister::XMM1:
        return xmm1;

    default:
        throw std::invalid_argument("Expected an XMM register");
    }
}

// ---------
// Move data
// ---------
void X86Assembler::mov(Register destination, Immediate value)
{
    assembler->mov(toAsmJit(destination), asmjit::imm(static_cast<std::int64_t>(value)));
}

void X86Assembler::mov(Register destination, DwordMemory source)
{
    assembler->mov(toAsmJit(destination), asmjit::x86::dword_ptr(toAsmJit(source.base_register), source.offset));
}

void X86Assembler::mov(Register destination, QwordMemory source)
{
    assembler->mov(toAsmJit(destination), asmjit::x86::qword_ptr(toAsmJit(source.base_register), source.offset));
}

void X86Assembler::mov(DwordMemory destination, Register source)
{
    assembler->mov(asmjit::x86::dword_ptr(toAsmJit(destination.base_register), destination.offset), toAsmJit(source));
}

void X86Assembler::mov(DwordMemory destination, Immediate value)
{
    assembler->mov(
        asmjit::x86::dword_ptr(toAsmJit(destination.base_register), destination.offset),
        asmjit::imm(static_cast<std::int64_t>(value)));
}

void X86Assembler::mov(QwordMemory destination, Register source)
{
    assembler->mov(asmjit::x86::qword_ptr(toAsmJit(destination.base_register), destination.offset), toAsmJit(source));
}

/**
 * @brief Move data between general-purpose and floating-point registers.
 */
void X86Assembler::movq(SimdRegister destination, Register source)
{
    assembler->movq(toAsmJit(destination), toAsmJit(source));
}

// ---------------------------------
// Move scalar floating-point values
// ---------------------------------
void X86Assembler::movsd(SimdRegister destination, QwordMemory source)
{
    assembler->movsd(toAsmJit(destination), asmjit::x86::qword_ptr(toAsmJit(source.base_register), source.offset));
}

void X86Assembler::movsd(QwordMemory destination, SimdRegister source)
{
    assembler->movsd(asmjit::x86::qword_ptr(toAsmJit(destination.base_register), destination.offset), toAsmJit(source));
}

// -------------------------------------------------------------------
// Convert a signed integer to a double-precision floating-point value
// -------------------------------------------------------------------
void X86Assembler::cvtsi2sd(SimdRegister destination, Register source)
{
    assembler->cvtsi2sd(toAsmJit(destination), toAsmJit(source));
}

void X86Assembler::cvtsi2sd(SimdRegister destination, DwordMemory source)
{
    assembler->cvtsi2sd(toAsmJit(destination), asmjit::x86::dword_ptr(toAsmJit(source.base_register), source.offset));
}

// ------------------
// Integer arithmetic
// ------------------
void X86Assembler::add(Register destination, Register source)
{
    assembler->add(toAsmJit(destination), toAsmJit(source));
}

void X86Assembler::add(Register destination, Immediate value)
{
    assembler->add(toAsmJit(destination), asmjit::imm(static_cast<std::int64_t>(value)));
}

void X86Assembler::sub(Register destination, Register source)
{
    assembler->sub(toAsmJit(destination), toAsmJit(source));
}

void X86Assembler::sub(Register destination, Immediate value)
{
    assembler->sub(toAsmJit(destination), asmjit::imm(static_cast<std::int64_t>(value)));
}

void X86Assembler::imul(Register destination, Register source)
{
    assembler->imul(toAsmJit(destination), toAsmJit(source));
}

// -------------------------
// Floating-point arithmetic
// -------------------------
void X86Assembler::addsd(SimdRegister destination, SimdRegister source)
{
    assembler->addsd(toAsmJit(destination), toAsmJit(source));
}

void X86Assembler::subsd(SimdRegister destination, SimdRegister source)
{
    assembler->subsd(toAsmJit(destination), toAsmJit(source));
}

void X86Assembler::mulsd(SimdRegister destination, SimdRegister source)
{
    assembler->mulsd(toAsmJit(destination), toAsmJit(source));
}

void X86Assembler::divsd(SimdRegister destination, SimdRegister source)
{
    assembler->divsd(toAsmJit(destination), toAsmJit(source));
}

// --------------------------
// Function calls and returns
// --------------------------
void X86Assembler::call(Register target)
{
    assembler->call(toAsmJit(target));
}

void X86Assembler::ret()
{
    assembler->ret();
}

} // namespace jit
