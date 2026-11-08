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
    ir = &input;

    for (const Instruction &instruction : ir->instructions)
    {
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
            return ir->localCount > 0 && emitReturn();

        return false;
    }

    return true;
}

} // namespace jit
