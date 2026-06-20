#include "opcode.hpp"

std::string opcodeToString(OpCode opcode)
{
    switch (opcode)
    {
    case OpCode::LOAD_CONST:
        return "LOAD_CONST";

    case OpCode::LOAD_GLOBAL:
        return "LOAD_GLOBAL";

    case OpCode::STORE_GLOBAL:
        return "STORE_GLOBAL";

    case OpCode::ADD:
        return "ADD";

    case OpCode::SUBTRACT:
        return "SUBTRACT";

    case OpCode::MULTIPLY:
        return "MULTIPLY";

    case OpCode::DIVIDE:
        return "DIVIDE";

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
