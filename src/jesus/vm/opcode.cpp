#include "opcode.hpp"

std::string opcodeToString(OpCode opcode)
{
    switch (opcode)
    {
    case OpCode::PUSH_LITERAL:
        return "PUSH_LITERAL";

    case OpCode::CREATE_GLOBAL:
        return "CREATE_GLOBAL";

    case OpCode::READ_GLOBAL:
        return "READ_GLOBAL";

    case OpCode::WRITE_GLOBAL:
        return "WRITE_GLOBAL";

    case OpCode::ADD:
        return "ADD";

    case OpCode::SUBTRACT:
        return "SUBTRACT";

    case OpCode::MULTIPLY:
        return "MULTIPLY";

    case OpCode::DIVIDE:
        return "DIVIDE";

    case OpCode::MODULO:
        return "MODULO";

    case OpCode::EQUAL:
        return "EQUAL";

    case OpCode::NOT_EQUAL:
        return "NOT_EQUAL";

    case OpCode::LESS:
        return "LESS";

    case OpCode::LESS_EQUAL:
        return "LESS_EQUAL";

    case OpCode::GREATER:
        return "GREATER";

    case OpCode::GREATER_EQUAL:
        return "GREATER_EQUAL";

    case OpCode::OR:
        return "OR";

    case OpCode::AND:
        return "AND";

    case OpCode::XOR:
        return "XOR";

    case OpCode::PRINT:
        return "PRINT";

    case OpCode::JUMP:
        return "JUMP";

    case OpCode::JUMP_IF_FALSE:
        return "JUMP_IF_FALSE";

    case OpCode::CALL:
        return "CALL";

    case OpCode::RETURN:
        return "RETURN";

    default:
        return "UNKNOWN_OPCODE";
    }
}
