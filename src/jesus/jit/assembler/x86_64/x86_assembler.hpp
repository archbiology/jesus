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

private:
    std::unique_ptr<asmjit::CodeHolder> code;
    std::unique_ptr<asmjit::x86::Assembler> assembler;

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
};

} // namespace jit
