#include "x86_assembler.hpp"
#include <asmjit/x86.h>

namespace jit
{
// --------------
// Control flow
// --------------
void X86Assembler::ensureJumpLabelExistsUpTo(std::uint32_t index)
{
    while (jumpLabels.size() <= index)
        jumpLabels.push_back(assembler->new_label());
}

void X86Assembler::markJumpTarget(std::uint32_t index)
{
    ensureJumpLabelExistsUpTo(index);

    // ---------------------------------------------------------
    // Tell AsmJit that the current machine-code position is the
    // destination of jumps that reference this label.
    // ---------------------------------------------------------
    assembler->bind(jumpLabels[index]);
}

void X86Assembler::jump(std::uint32_t label)
{
    ensureJumpLabelExistsUpTo(label);
    assembler->jmp(jumpLabels[label]);
}

void X86Assembler::jumpIfZero(Register condition, std::uint32_t target)
{
    const asmjit::x86::Gp reg = toAsmJit(condition);

    ensureJumpLabelExistsUpTo(target);

    // ------------------------------------------------------------------
    // TEST compares the register with itself without changing its value.
    // It sets the CPU's zero flag if the register contains zero.
    // ------------------------------------------------------------------
    assembler->test(reg, reg);

    // -----------------------------------------------
    // Jump to the label only if the zero flag is set.
    // -----------------------------------------------
    assembler->j(asmjit::x86::CondCode::kZero, jumpLabels[target]);
}

void X86Assembler::jumpIfTypeIsNot(
    Register localsAddress, MemoryOffset typeFlagsOffset, std::uint16_t requiredTypeFlag, std::uint32_t jumpTarget)
{
    ensureJumpLabelExistsUpTo(jumpTarget);
    // -----------------------------------------------------------------------
    // Value flags can overlap. F_NUMBER is set for both integers and doubles.
    // TEST performs a bitwise AND between the Value flags and requiredTypeFlag.
    // It does not store the result; it only updates the CPU flags.
    // -----------------------------------------------------------------------
    assembler->test(asmjit::x86::word_ptr(toAsmJit(localsAddress), typeFlagsOffset), requiredTypeFlag);

    // --------------------------------------------------------
    // JZ jumps when the TEST result is zero.
    // This means none of the required type flag bits were set.
    // --------------------------------------------------------
    assembler->jz(jumpLabels[jumpTarget]);
}

/** @brief It jumps when the first operand is less than or equal to the second operand. */
void X86Assembler::jumpIfNotGreater(SimdRegister left, SimdRegister right, std::uint32_t jumpTarget)
{
    ensureJumpLabelExistsUpTo(jumpTarget);

    // -----------------------------------------------------------
    // UCOMISD compares two double-precision floating-point values.
    // It sets the CPU flags according to the comparison:
    //   left > right: CF=0, ZF=0, PF=0
    //   left < right: CF=1, ZF=0, PF=0
    //   left = right: CF=0, ZF=1, PF=0
    //   unordered:    CF=1, ZF=1, PF=1 (either operand is NaN)
    //
    // CF (Carry Flag): Set when the first operand is less than the second, or when NaN.
    // ZF (Zero Flag): Set when the operands are equal, or when NaN.
    // PF (Parity Flag): Set when the comparison is unordered (either operand is NaN).
    // -----------------------------------------------------------
    assembler->ucomisd(toAsmJit(left), toAsmJit(right));

    // -------------------------------------------------------------
    // JBE jumps when CF=1 or ZF=1.
    // Therefore, it jumps when left is less than or equal to right.
    // It also jumps when either operand is NaN.
    // -------------------------------------------------------------
    assembler->jbe(jumpLabels[jumpTarget]);
}

} // namespace jit
