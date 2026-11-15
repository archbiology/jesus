#include "assembler.hpp"

#include <cstring>
#include <iostream>
#include <vector>

#include "spirit/value.hpp"
#include "vm/opcode.hpp"

namespace jit
{

void Assembler::releaseCompilationData()
{
    jitCompilationStack.clear();
    jitCompilationStack.shrink_to_fit();
    globalVariableTypes.clear();
    globalVariableTypes.shrink_to_fit();
    localVariableTypes.clear();
    localVariableTypes.shrink_to_fit();
    ir = nullptr;
    chunksMap = nullptr;
}

/**
 * @brief Compiles validated IR instructions into assembly.
 */
bool Assembler::compileToMachineCode(const IR &input)
{
    jitCompilationStack.clear();
    globalVariableTypes.clear();
    localVariableTypes.clear();
    ir = &input;

    bool skipNextInstruction = false;

    for (std::size_t index = 0; index < ir->instructions.size(); ++index)
    {
        markJumpTarget(static_cast<std::uint32_t>(index));

        if (skipNextInstruction)
        {
            skipNextInstruction = false;
            continue;
        }

        const Instruction &instruction = ir->instructions[index];
        const OpCode opcode = instruction.opcode;

        if (opcode == OpCode::PUSH_LITERAL)
        {
            if (!emitPushLiteral(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::CREATE_GLOBAL || opcode == OpCode::WRITE_GLOBAL)
        {
            if (!emitStoreGlobal(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::READ_GLOBAL)
        {
            if (!emitReadGlobal(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::WRITE_LOCAL)
        {
            if (!emitStoreLocal(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::READ_LOCAL)
        {
            if (!emitReadLocal(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::CREATE_INSTANCE)
        {
            if (!emitCreateInstance())
                return false;

            continue;
        }

        if (opcode == OpCode::CALL)
        {
            if (!emitCall(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::JUMP)
        {
            if (!emitJump(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::JUMP_IF_FALSE)
        {
            if (!emitJumpIfFalse(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::INIT_REPEAT_COUNT)
        {
            if (!emitInitRepeatCount(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::ADD_LOCAL_LITERAL_STORE)
        {
            if (!emitAddLocalLiteralStore(instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::ADD_LOCAL_LITERAL_STORE_JUMP)
        {
            // ------------------------------------------------------------
            // [index]     ADD_LOCAL_LITERAL_STORE_JUMP
            // [index + 1] JUMP <loopStart>   (target holder, never executed)
            // ------------------------------------------------------------
            if (index + 1 >= ir->instructions.size())
                return false;

            const std::uint32_t jumpTarget = ir->instructions[index + 1].operand;
            if (!emitAddLocalLiteralStoreJump(instruction, jumpTarget))
                return false;

            skipNextInstruction = true;
            continue;
        }

        if (isComparisonBranch(opcode))
        {
            // -------------------------------------
            // [index]     JUMP_IF_*
            // [index + 1] TARGET 50
            // [index + 2] NEXT_INSTRUCTION
            // -------------------------------------
            if (index + 1 >= ir->instructions.size())
                return false;

            const std::uint32_t jumpTarget = ir->instructions[index + 1].operand;
            if (!emitComparisonBranch(instruction, jumpTarget))
                return false;

            // ----------------------------------------------------
            // This example code:
            //
            //     create age = 15
            //
            //     if age < 18:
            //       say "You are a minor"
            //     else:
            //       say "You are an adult"
            //
            //     say "Have a nice day"
            //
            // Is conceptually like this:
            //       compare age, 18
            //
            //       jump_if_not_less  <-- We are at this point.
            //       label_5           <-- label_5 is NOT an instruction but the operand of 'jump_if_not_less label_5'
            //                             meaning (jump to label_5 if not less than 18) that is why we have skipNextInstruction
            //
            //       say "You are a minor"
            //       jump label_7
            //
            //   label_5:
            //      say "You are an adult"
            //
            //   label_7:
            //      say "Have a nice day"
            //
            // "If age is NOT less than 18, jump to instruction 5."
            // ----------------------------------------------------
            skipNextInstruction = true;
            continue;
        }

        if (isValueComparison(opcode))
        {
            if (!emitValueComparison(opcode))
                return false;

            continue;
        }

        if (isArithmetic(opcode))
        {
            if (!emitBinaryArithmeticOperation(opcode))
                return false;

            continue;
        }

        OpCode operation{};
        if (isFusedOpForRightLiteral(opcode, operation))
        {
            if (!emitFusedLiteralRight(operation, instruction))
                return false;

            continue;
        }

        if (opcode == OpCode::PRINT)
        {
            if (!emitPrint())
                return false;

            continue;
        }

        if (opcode == OpCode::EXIT)
            return ir->localCount == 0;

        if (opcode == OpCode::RETURN)
        {
            if (ir->localCount == 0 || !emitReturn())
                return false;

            // A conditional (if) may lead to this RETURN while other instructions
            // still remain in the method. Skip them because the method has returned.

            // A conditional return must skip the remaining instructions in the method.
            //
            // Example:
            //   label_49:
            //       if condition:
            //           return
            //           jump label_50  // Skip the remaining instructions.
            //
            //       say "still running"
            //   label_50:              // End of the method.
            //
            // TODO: allow 'return' only inside methods to avoid this 'unnecessary' jump
            // -------------------------------------------------------------------------
            jump(static_cast<std::uint32_t>(ir->instructions.size()));
            continue;
        }

        return false;
    }

    // --------------------------------------------------------
    // Mark the end of the program/method as a jump destination
    // --------------------------------------------------------
    markJumpTarget(static_cast<std::uint32_t>(ir->instructions.size()));
    return true;
}

} // namespace jit
