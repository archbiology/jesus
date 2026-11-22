#pragma once

#include <memory>

#include <asmjit/x86.h>

#include "jit/assembler/assembler.hpp"

namespace jit
{

/**
 * @brief x86-64 code generator built on top of asmjit.
 *
 * It receives platform-independent IR and emits native x86-64 instructions.
 * No x86-specific type leaks into the IR, the parser or the JIT engine.
 */
class X86Assembler : public Assembler
{
public:
    const char *name() const override
    {
        return "x86-64";
    }

    std::unique_ptr<ExecutableCode> assemble(const IR &ir) override;

    void releaseCompilationData() override;

private:
    std::unique_ptr<asmjit::CodeHolder> code;
    std::unique_ptr<asmjit::x86::Assembler> assembler;
    std::vector<asmjit::Label> jumpLabels;

    void ensureJumpLabelExistsUpTo(std::uint32_t index);

protected:
    // ---------
    // Move data
    // ---------
    void mov(Register destination, Immediate value) override;
    void mov(Register destination, DwordMemory source) override;
    void mov(Register destination, QwordMemory source) override;
    void mov(DwordMemory destination, Register source) override;
    void mov(DwordMemory destination, Immediate value) override;
    void mov(QwordMemory destination, Register source) override;
    void lea(Register destination, QwordMemory source) override;

    /**
     * @brief Move data from general-purpose to floating-point registers.
     */
    void movq(SimdRegister destination, Register source) override;

    // ---------------------------------
    // Move scalar floating-point values
    // ---------------------------------
    void movsd(SimdRegister destination, QwordMemory source) override;
    void movsd(QwordMemory destination, SimdRegister source) override;

    // -------------------------------------------------------------------
    // Convert a signed integer to a double-precision floating-point value
    // -------------------------------------------------------------------
    void cvtsi2sd(SimdRegister destination, Register source) override;
    void cvtsi2sd(SimdRegister destination, DwordMemory source) override;

    // ------------------
    // Integer arithmetic
    // ------------------
    void add(Register destination, Register source) override;
    void add(Register destination, Immediate value) override;
    void sub(Register destination, Register source) override;
    void sub(Register destination, Immediate value) override;
    void imul(Register destination, Register source) override;

    // -------------------------
    // Floating-point arithmetic
    // -------------------------
    void addsd(SimdRegister destination, SimdRegister source) override;
    void subsd(SimdRegister destination, SimdRegister source) override;
    void mulsd(SimdRegister destination, SimdRegister source) override;
    void divsd(SimdRegister destination, SimdRegister source) override;

    // --------------------------
    // Function calls and returns
    // --------------------------
    void call(Register target) override;
    void ret() override;

    // -----------------
    // Control flow
    // -----------------
    void markJumpTarget(std::uint32_t index) override;
    void jump(std::uint32_t target) override;
    void jumpIfZero(Register condition, std::uint32_t target) override;
    void jumpIfMaskClear(Register base, MemoryOffset offset, std::uint16_t mask, std::uint32_t target) override;
    void jumpIfNotGreater(SimdRegister left, SimdRegister right, std::uint32_t target) override;
    void compareDoubles(SimdRegister left, SimdRegister right) override;
    void setComparisonResult(JitCondition condition, Register destination) override;
    void compareInts(Register left, Register right) override;
    void setIntComparisonResult(JitCondition condition, Register destination) override;
};

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
    case Register::R8:
        return asmjit::x86::r8;
    case Register::R9:
        return asmjit::x86::r9;
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

} // namespace jit
