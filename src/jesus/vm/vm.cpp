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
        case OpCode::MODULO:
        case OpCode::EQUAL:
        case OpCode::NOT_EQUAL:
        case OpCode::LESS:
        case OpCode::LESS_EQUAL:
        case OpCode::GREATER:
        case OpCode::GREATER_EQUAL:
        case OpCode::OR:
        case OpCode::AND:
        case OpCode::XOR:
        {
            Value right = stack.back();
            stack.pop_back();
            Value left = stack.back();
            stack.pop_back();

            switch (ip->opcode)
            {
            // ----------
            // Arithmetic
            // ----------
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
            case OpCode::MODULO:
                stack.push_back(left % right);
                break;
            // -----------------------
            // Relational / Comparison
            // -----------------------
            case OpCode::EQUAL:
                stack.push_back(Value(left == right));
                break;
            case OpCode::NOT_EQUAL:
                stack.push_back(Value(left != right));
                break;
            case OpCode::LESS:
                stack.push_back(Value(left < right));
                break;
            case OpCode::LESS_EQUAL:
                stack.push_back(Value(left <= right));
                break;
            case OpCode::GREATER:
                stack.push_back(Value(left > right));
                break;
            case OpCode::GREATER_EQUAL:
                stack.push_back(Value(left >= right));
                break;
            // -----
            // Logic
            // -----
            case OpCode::OR:
                stack.push_back(left.AS_BOOLEAN ? left : right);
                break;
            case OpCode::AND:
                stack.push_back(left.AS_BOOLEAN ? right : left);
                break;

            case OpCode::XOR:
                if (left.IS_BOOLEAN && right.IS_BOOLEAN)
                {
                    // Logical XOR
                    stack.push_back(Value(left.AS_BOOLEAN != right.AS_BOOLEAN));
                    break;
                }

                if (left.IS_NUMBER && right.IS_NUMBER)
                {
                    // Bitwise XOR
                    stack.push_back(Value(left.toInt() ^ right.toInt()));
                    break;
                }

                stack.push_back(Value::formless()); // fallback if types mismatch
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

        case OpCode::WRITE_GLOBAL:
        {
            uint32_t index = ip->operand;
            Value value = stack.back();

            stack.pop_back();
            globals[index] = value;

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
