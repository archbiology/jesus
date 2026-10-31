#include "assembler.hpp"

#include <cstring>
#include <iostream>
#include <vector>

#include "spirit/value.hpp"
#include "vm/opcode.hpp"

namespace jit
{

/**
 * @brief Compiles validated IR instructions into assembly.
 */
bool Assembler::compileToMachineCode(const IR &input)
{
    jitStack.clear();
    ir = &input;

    for (const Instruction &instruction : ir->instructions)
    {
        const OpCode opcode = instruction.opcode;

        if (opcode == OpCode::PUSH_LITERAL)
        {
            emitPushLiteral(instruction);
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

        if (opcode == OpCode::RETURN)
            return true;

        return false;
    }

    return true;
}

} // namespace jit
