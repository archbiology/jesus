#include "x86_assembler.hpp"
#include <asmjit/x86.h>

namespace jit
{
void X86Assembler::compareDoubles(SimdRegister left, SimdRegister right)
{
    assembler->comisd(toAsmJit(left), toAsmJit(right));
}

void X86Assembler::setComparisonResult(JitCondition condition, Register destination)
{
    using namespace asmjit::x86;

    if (destination != Register::EAX)
        throw std::invalid_argument("setComparisonResult expects EAX as its destination");

    //-------------------------------------------
    // The registers
    // -------------
    // ┌────────────────────────────────────────┐
    // │                  RAX                   │ Full 64 bits
    // └────────────────────────────────────────┘
    //                       │        EAX       │ Lower 32 bits of RAX
    //                       └──────────────────┘
    //                               │    AX    │ Lower 16 bits of RAX
    //                               └──────────┘
    //                                     │ AL │ Lower 8 bits of RAX
    //                                     └────┘
    // ┌────────────────────────────────────────┐
    // │                  RCX                   │ Full 64 bits
    // └────────────────────────────────────────┘
    //                       │        ECX       │ Lower 32 bits of RCX
    //                       └──────────────────┘
    //                               │    CX    │ Lower 16 bits of RCX
    //                               └──────────┘
    //                                     │ CL │ Lower 8 bits of RCX
    //                                     └────┘
    //-------------------------------------------
    // The flags:
    // ----------
    // |     Flag         |                 Meaning                   |
    // |------------------|-------------------------------------------|
    // | ZF (Zero Flag)   | The numbers are equal.                    |
    // | CF (Carry Flag)  | The first number is less than the second. |
    // | PF (Parity Flag) | The comparison is unordered.              |
    //-----------------------------------------------------------------
    //
    //
    // Before this method is called, we had a comparison similar to the following:
    //
    //  Check if two numbers are equal
    //      ucomisd xmm0, xmm1
    //
    // It stored the comparison result in CPU flags (ZF, PF and CF):
    //      ----------------------------------
    //      | Comparison      | ZF | PF | CF |
    //      |-----------------|----|----|----|
    //      | First = Second  | 1  | 0  | 0  |
    //      | First < Second  | 0  | 0  | 1  |
    //      | First > Second  | 0  | 0  | 0  |
    //      | Unordered (NaN) | 1  | 1  | 1  |
    //      ----------------------------------
    switch (condition)
    {
    case JitCondition::Equal:
        // ------------------------------------
        // 1. Was the comparison valid? (SETNP)
        // ---------------------------------------------------------------------
        // setnp al -> set AL to 1 if the comparison produced a valid result.
        //
        //      SET = set a register to 1 or 0, depending on a condition.
        //      P = Parity Flag (PF -> at least one operand was NaN).
        //      NP = Not Parity (PF is zero -> both were valid numbers).
        //
        // So: Is the Parity Flag equal to zero?
        //      Yes → put 1 in AL (valid numbers; no NaN).
        //      No  → put 0 in AL (invalid comparison; at least one NaN)
        // ---------------------------------------------------------------------
        assembler->set(CondCode::kNP, al);
        // ------------------------------------------------------------------------
        // Copy the answer from AL into EAX, converting it to a full 32-bit number.
        // We do this because the next operations work with 32-bit registers.
        // ------------------------------------------------------------------------
        assembler->movzx(eax, al);

        // ------------------------------------------------
        // 2. Were the numbers equal? (SETE)
        //
        // sete cl -> set CL to 1 if both numbers are EQUAL
        // Then, copy the value to the 32-bit register.
        // ------------------------------------------------
        assembler->set(CondCode::kEqual, cl);
        assembler->movzx(ecx, cl);

        // -------------------------------------------------------
        // 3. Were BOTH conditions true? (AND)
        //
        // (was the comparison valid) AND (were the numbers equal)
        // -------------------------------------------------------
        assembler->and_(eax, ecx);
        break;

    case JitCondition::NotEqual:
        // -----------------------------------------------------------
        // 1. Check if the comparison produced an INvalid result (NaN)
        // -----------------------------------------------------------
        assembler->set(CondCode::kP, al);
        assembler->movzx(eax, al);

        // -----------------------------------
        // 2. Check if the numbers are different.
        // -----------------------------------
        assembler->set(CondCode::kNotEqual, cl);
        assembler->movzx(ecx, cl);

        // -------------------
        // 3. NaN OR different
        // -------------------
        assembler->or_(eax, ecx);
        break;

    case JitCondition::Less:
        // -----------------------------
        // 1. Was the comparison valid?
        // -----------------------------
        assembler->set(CondCode::kNP, al);
        assembler->movzx(eax, al);

        // -------------------------------
        // 2. Check if the first < second
        // -------------------------------
        assembler->set(CondCode::kUnsignedLT, cl);
        assembler->movzx(ecx, cl);

        // ----------------------------------------
        // 3. Valid comparison AND (first < second)
        // ----------------------------------------
        assembler->and_(eax, ecx);
        break;

    case JitCondition::LessEqual:
        // -----------------------------
        // 1. Was the comparison valid?
        // -----------------------------
        assembler->set(CondCode::kNP, al);
        assembler->movzx(eax, al);

        // --------------------------------
        // 2. Check if the first <= second
        // --------------------------------
        assembler->set(CondCode::kUnsignedLE, cl);
        assembler->movzx(ecx, cl);

        // -----------------------------------------
        // 3. Valid comparison AND (first <= second)
        // -----------------------------------------
        assembler->and_(eax, ecx);
        break;

    case JitCondition::Greater:
        // -----------------------------
        // 1. Was the comparison valid?
        // -----------------------------
        assembler->set(CondCode::kNP, al);
        assembler->movzx(eax, al);

        // -------------------------------
        // 2. Check if the first > second
        // -------------------------------
        assembler->set(CondCode::kUnsignedGT, cl);
        assembler->movzx(ecx, cl);

        // ----------------------------------------
        // 3. Valid comparison AND (first > second)
        // ----------------------------------------
        assembler->and_(eax, ecx);
        break;

    case JitCondition::GreaterEqual:
        // -----------------------------
        // 1. Was the comparison valid?
        // -----------------------------
        assembler->set(CondCode::kNP, al);
        assembler->movzx(eax, al);

        // -------------------------------
        // 2. Check if the first >= second
        // -------------------------------
        assembler->set(CondCode::kUnsignedGE, cl);
        assembler->movzx(ecx, cl);

        // -----------------------------------------
        // 3. Valid comparison AND (first >= second)
        // -----------------------------------------
        assembler->and_(eax, ecx);
        break;
    }
}

void X86Assembler::compareInts(Register left, Register right)
{
    assembler->cmp(toAsmJit(left), toAsmJit(right));
}

void X86Assembler::setIntComparisonResult(JitCondition condition, Register destination)
{
    using namespace asmjit::x86;

    if (destination != Register::EAX)
        throw std::invalid_argument("setIntComparisonResult expects EAX as its destination");

    // -----------------------------------------------------------------
    // `cmp` has already compared the two integers and set the CPU flags.
    //
    // Unlike floating-point comparisons, integer comparisons use signed
    // conditions (Less, LessEqual, Greater and GreaterEqual).
    //
    // Let's SET 1 or 0 into AL, depending on the comparison condition.
    // -----------------------------------------------------------------
    switch (condition)
    {
    case JitCondition::Equal:
        assembler->set(CondCode::kEqual, al);
        break;
    case JitCondition::NotEqual:
        assembler->set(CondCode::kNotEqual, al);
        break;
    case JitCondition::Less:
        assembler->set(CondCode::kSignedLT, al);
        break;
    case JitCondition::LessEqual:
        assembler->set(CondCode::kSignedLE, al);
        break;
    case JitCondition::Greater:
        assembler->set(CondCode::kSignedGT, al);
        break;
    case JitCondition::GreaterEqual:
        assembler->set(CondCode::kSignedGE, al);
        break;
    }

    // ---------------------------------------------------------------
    // SET only writes the lower 8 bits (AL).
    // Let's convert the 0/1 byte result into a 32-bit JIT stack slot.
    //
    //     AL  =                            00000001
    //     EAX = 00000000 00000000 00000000 00000001
    // ---------------------------------------------------------------
    assembler->movzx(eax, al);
}

} // namespace jit
