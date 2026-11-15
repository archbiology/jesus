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

} // namespace jit
