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
        case OpCode::PUSH_LITERAL:
        {
            stack.push_back(chunk.literals[ip->operand]);

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

        case OpCode::CREATE_GLOBAL:
        {
            Value value = stack.back();
            stack.pop_back();

            uint32_t index = ip->operand;

            if (index >= globals.size())
                globals.resize(index + 1);

            globals[index] = value;

            ++ip;
            break;
        }

        case OpCode::READ_GLOBAL:
        {
            uint32_t index = ip->operand;

            stack.push_back(globals[index]);

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
