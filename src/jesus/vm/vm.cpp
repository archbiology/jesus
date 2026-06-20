#include "vm.hpp"
#include "opcode.hpp"
#include <sstream>

void VM::run(const Chunk &chunk)
{
    std::cout << "[VM]\n";
    auto begin = chunk.instructions.data();
    auto ip = begin;

    while (true)
    {
        switch (ip->opcode)
        {
        case OpCode::LOAD_CONST:
        {
            stack.push_back(chunk.constants[ip->operand]);

            ++ip;
            break;
        }

        case OpCode::PRINT:
        {
            std::cout << stack.back().toString() << std::endl;
            stack.pop_back();

            ++ip;
            break;
        }

        case OpCode::ADD:
        case OpCode::SUBTRACT:
        case OpCode::MULTIPLY:
        case OpCode::DIVIDE:
        {
            Value right = stack.back();
            stack.pop_back();
            Value left = stack.back();
            stack.pop_back();

            switch (ip->opcode)
            {
            case OpCode::ADD:
                stack.push_back(left + right);
                break;
            case OpCode::SUBTRACT:
                stack.push_back(left - right);
                break;
            case OpCode::MULTIPLY:
                stack.push_back(left * right);
                break;
            case OpCode::DIVIDE:
                stack.push_back(left / right);
                break;
            default:
                break;
            }

            ++ip;
            break;
        }

        case OpCode::RETURN:
        {
            return;
        }

        default:
        {
            auto offset = ip - begin;
            std::stringstream error;
            error << "VM Error\n\n"
                  << "  Instruction (offset): " << offset << "\n"
                  << "  Opcode: " << opcodeToString(ip->opcode) << "\n"
                  << "  Operand: " << ip->operand << "\n\n"
                  << "  Reason:\n"
                  << "    Opcode not implemented.";

            throw std::runtime_error(error.str());
        }
        }
    }
}
