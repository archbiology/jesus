#include "bytecode_inspector.hpp"
#include "ast/stmt/inspect_stmt.hpp"
#include "interpreter/interpreter.hpp"
#include "vm/compiler.hpp"

void BytecodeInspector::inspect(Interpreter &jesus, const InspectStmt &)
{
    Compiler compiler;
    Chunk chunk = compiler.compile(jesus.persistedAST);

    std::cout << "Bytecode:\n";

    for (size_t i = 0; i < chunk.instructions.size(); ++i)
    {
        auto const &instr = chunk.instructions[i];

        const auto &value = chunk.constants[instr.operand];

        std::cout << i << ": ";

        switch (instr.opcode)
        {
        case OpCode::LOAD_CONST:
        case OpCode::JUMP:
        case OpCode::JUMP_IF_FALSE:

            std::cout << opcodeToString(instr.opcode) << " const[" << instr.operand << "] -> " << value.toString() << "\n";
            break;

        default:
            std::cout << opcodeToString(instr.opcode) << "\n";
            break;
        }
    }
}
