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

    case OpCode::READ_LOCAL:
        return "READ_LOCAL";

    case OpCode::WRITE_LOCAL:
        return "WRITE_LOCAL";

    case OpCode::CREATE_INSTANCE:
        return "CREATE_INSTANCE";

    case OpCode::READ_ATTR:
        return "READ_ATTR";

    case OpCode::WRITE_ATTR:
        return "WRITE_ATTR";

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

    case OpCode::ADD_LOCAL_LOCAL:
        return "ADD_LOCAL_LOCAL";

    case OpCode::SUBTRACT_LOCAL_LOCAL:
        return "SUBTRACT_LOCAL_LOCAL";

    case OpCode::MULTIPLY_LOCAL_LOCAL:
        return "MULTIPLY_LOCAL_LOCAL";

    case OpCode::DIVIDE_LOCAL_LOCAL:
        return "DIVIDE_LOCAL_LOCAL";

    case OpCode::MODULO_LOCAL_LOCAL:
        return "MODULO_LOCAL_LOCAL";

    case OpCode::EQUAL_LOCAL_LOCAL:
        return "EQUAL_LOCAL_LOCAL";

    case OpCode::NOT_EQUAL_LOCAL_LOCAL:
        return "NOT_EQUAL_LOCAL_LOCAL";

    case OpCode::LESS_LOCAL_LOCAL:
        return "LESS_LOCAL_LOCAL";

    case OpCode::LESS_EQUAL_LOCAL_LOCAL:
        return "LESS_EQUAL_LOCAL_LOCAL";

    case OpCode::GREATER_LOCAL_LOCAL:
        return "GREATER_LOCAL_LOCAL";

    case OpCode::GREATER_EQUAL_LOCAL_LOCAL:
        return "GREATER_EQUAL_LOCAL_LOCAL";

    case OpCode::ADD_LOCAL_LOCAL_STORE:
        return "ADD_LOCAL_LOCAL_STORE";

    case OpCode::SUBTRACT_LOCAL_LOCAL_STORE:
        return "SUBTRACT_LOCAL_LOCAL_STORE";

    case OpCode::MULTIPLY_LOCAL_LOCAL_STORE:
        return "MULTIPLY_LOCAL_LOCAL_STORE";

    case OpCode::DIVIDE_LOCAL_LOCAL_STORE:
        return "DIVIDE_LOCAL_LOCAL_STORE";

    case OpCode::MODULO_LOCAL_LOCAL_STORE:
        return "MODULO_LOCAL_LOCAL_STORE";

    case OpCode::ADD_LOCAL_LITERAL:
        return "ADD_LOCAL_LITERAL";

    case OpCode::SUBTRACT_LOCAL_LITERAL:
        return "SUBTRACT_LOCAL_LITERAL";

    case OpCode::MULTIPLY_LOCAL_LITERAL:
        return "MULTIPLY_LOCAL_LITERAL";

    case OpCode::DIVIDE_LOCAL_LITERAL:
        return "DIVIDE_LOCAL_LITERAL";

    case OpCode::MODULO_LOCAL_LITERAL:
        return "MODULO_LOCAL_LITERAL";

    case OpCode::EQUAL_LOCAL_LITERAL:
        return "EQUAL_LOCAL_LITERAL";

    case OpCode::NOT_EQUAL_LOCAL_LITERAL:
        return "NOT_EQUAL_LOCAL_LITERAL";

    case OpCode::LESS_LOCAL_LITERAL:
        return "LESS_LOCAL_LITERAL";

    case OpCode::LESS_EQUAL_LOCAL_LITERAL:
        return "LESS_EQUAL_LOCAL_LITERAL";

    case OpCode::GREATER_LOCAL_LITERAL:
        return "GREATER_LOCAL_LITERAL";

    case OpCode::GREATER_EQUAL_LOCAL_LITERAL:
        return "GREATER_EQUAL_LOCAL_LITERAL";

    case OpCode::ADD_LITERAL_LOCAL:
        return "ADD_LITERAL_LOCAL";

    case OpCode::SUBTRACT_LITERAL_LOCAL:
        return "SUBTRACT_LITERAL_LOCAL";

    case OpCode::MULTIPLY_LITERAL_LOCAL:
        return "MULTIPLY_LITERAL_LOCAL";

    case OpCode::DIVIDE_LITERAL_LOCAL:
        return "DIVIDE_LITERAL_LOCAL";

    case OpCode::MODULO_LITERAL_LOCAL:
        return "MODULO_LITERAL_LOCAL";

    case OpCode::EQUAL_LITERAL_LOCAL:
        return "EQUAL_LITERAL_LOCAL";

    case OpCode::NOT_EQUAL_LITERAL_LOCAL:
        return "NOT_EQUAL_LITERAL_LOCAL";

    case OpCode::LESS_LITERAL_LOCAL:
        return "LESS_LITERAL_LOCAL";

    case OpCode::LESS_EQUAL_LITERAL_LOCAL:
        return "LESS_EQUAL_LITERAL_LOCAL";

    case OpCode::GREATER_LITERAL_LOCAL:
        return "GREATER_LITERAL_LOCAL";

    case OpCode::GREATER_EQUAL_LITERAL_LOCAL:
        return "GREATER_EQUAL_LITERAL_LOCAL";

    case OpCode::ADD_LOCAL_LITERAL_STORE:
        return "ADD_LOCAL_LITERAL_STORE";

    case OpCode::SUBTRACT_LOCAL_LITERAL_STORE:
        return "SUBTRACT_LOCAL_LITERAL_STORE";

    case OpCode::MULTIPLY_LOCAL_LITERAL_STORE:
        return "MULTIPLY_LOCAL_LITERAL_STORE";

    case OpCode::DIVIDE_LOCAL_LITERAL_STORE:
        return "DIVIDE_LOCAL_LITERAL_STORE";

    case OpCode::MODULO_LOCAL_LITERAL_STORE:
        return "MODULO_LOCAL_LITERAL_STORE";

    case OpCode::ADD_LOCAL_RIGHT:
        return "ADD_LOCAL_RIGHT";

    case OpCode::SUBTRACT_LOCAL_RIGHT:
        return "SUBTRACT_LOCAL_RIGHT";

    case OpCode::MULTIPLY_LOCAL_RIGHT:
        return "MULTIPLY_LOCAL_RIGHT";

    case OpCode::DIVIDE_LOCAL_RIGHT:
        return "DIVIDE_LOCAL_RIGHT";

    case OpCode::MODULO_LOCAL_RIGHT:
        return "MODULO_LOCAL_RIGHT";

    case OpCode::EQUAL_LOCAL_RIGHT:
        return "EQUAL_LOCAL_RIGHT";

    case OpCode::NOT_EQUAL_LOCAL_RIGHT:
        return "NOT_EQUAL_LOCAL_RIGHT";

    case OpCode::LESS_LOCAL_RIGHT:
        return "LESS_LOCAL_RIGHT";

    case OpCode::LESS_EQUAL_LOCAL_RIGHT:
        return "LESS_EQUAL_LOCAL_RIGHT";

    case OpCode::GREATER_LOCAL_RIGHT:
        return "GREATER_LOCAL_RIGHT";

    case OpCode::GREATER_EQUAL_LOCAL_RIGHT:
        return "GREATER_EQUAL_LOCAL_RIGHT";

    case OpCode::ADD_LITERAL_RIGHT:
        return "ADD_LITERAL_RIGHT";

    case OpCode::SUBTRACT_LITERAL_RIGHT:
        return "SUBTRACT_LITERAL_RIGHT";

    case OpCode::MULTIPLY_LITERAL_RIGHT:
        return "MULTIPLY_LITERAL_RIGHT";

    case OpCode::DIVIDE_LITERAL_RIGHT:
        return "DIVIDE_LITERAL_RIGHT";

    case OpCode::MODULO_LITERAL_RIGHT:
        return "MODULO_LITERAL_RIGHT";

    case OpCode::EQUAL_LITERAL_RIGHT:
        return "EQUAL_LITERAL_RIGHT";

    case OpCode::NOT_EQUAL_LITERAL_RIGHT:
        return "NOT_EQUAL_LITERAL_RIGHT";

    case OpCode::LESS_LITERAL_RIGHT:
        return "LESS_LITERAL_RIGHT";

    case OpCode::LESS_EQUAL_LITERAL_RIGHT:
        return "LESS_EQUAL_LITERAL_RIGHT";

    case OpCode::GREATER_LITERAL_RIGHT:
        return "GREATER_LITERAL_RIGHT";

    case OpCode::GREATER_EQUAL_LITERAL_RIGHT:
        return "GREATER_EQUAL_LITERAL_RIGHT";

    case OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LOCAL:
        return "JUMP_IF_NOT_EQUAL_LOCAL_LOCAL";
    case OpCode::JUMP_IF_NOT_NOT_EQUAL_LOCAL_LOCAL:
        return "JUMP_IF_NOT_NOT_EQUAL_LOCAL_LOCAL";
    case OpCode::JUMP_IF_NOT_LESS_LOCAL_LOCAL:
        return "JUMP_IF_NOT_LESS_LOCAL_LOCAL";
    case OpCode::JUMP_IF_NOT_LESS_EQUAL_LOCAL_LOCAL:
        return "JUMP_IF_NOT_LESS_EQUAL_LOCAL_LOCAL";
    case OpCode::JUMP_IF_NOT_GREATER_LOCAL_LOCAL:
        return "JUMP_IF_NOT_GREATER_LOCAL_LOCAL";
    case OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL:
        return "JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL";

    case OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LITERAL:
        return "JUMP_IF_NOT_EQUAL_LOCAL_LITERAL";
    case OpCode::JUMP_IF_NOT_NOT_EQUAL_LOCAL_LITERAL:
        return "JUMP_IF_NOT_NOT_EQUAL_LOCAL_LITERAL";
    case OpCode::JUMP_IF_NOT_LESS_LOCAL_LITERAL:
        return "JUMP_IF_NOT_LESS_LOCAL_LITERAL";
    case OpCode::JUMP_IF_NOT_LESS_EQUAL_LOCAL_LITERAL:
        return "JUMP_IF_NOT_LESS_EQUAL_LOCAL_LITERAL";
    case OpCode::JUMP_IF_NOT_GREATER_LOCAL_LITERAL:
        return "JUMP_IF_NOT_GREATER_LOCAL_LITERAL";
    case OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL:
        return "JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL";

    case OpCode::JUMP_IF_NOT_EQUAL_LITERAL_LOCAL:
        return "JUMP_IF_NOT_EQUAL_LITERAL_LOCAL";
    case OpCode::JUMP_IF_NOT_NOT_EQUAL_LITERAL_LOCAL:
        return "JUMP_IF_NOT_NOT_EQUAL_LITERAL_LOCAL";
    case OpCode::JUMP_IF_NOT_LESS_LITERAL_LOCAL:
        return "JUMP_IF_NOT_LESS_LITERAL_LOCAL";
    case OpCode::JUMP_IF_NOT_LESS_EQUAL_LITERAL_LOCAL:
        return "JUMP_IF_NOT_LESS_EQUAL_LITERAL_LOCAL";
    case OpCode::JUMP_IF_NOT_GREATER_LITERAL_LOCAL:
        return "JUMP_IF_NOT_GREATER_LITERAL_LOCAL";
    case OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL:
        return "JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL";

    case OpCode::JUMP_IF_NOT_EQUAL_LOCAL_RIGHT:
        return "JUMP_IF_NOT_EQUAL_LOCAL_RIGHT";
    case OpCode::JUMP_IF_NOT_NOT_EQUAL_LOCAL_RIGHT:
        return "JUMP_IF_NOT_NOT_EQUAL_LOCAL_RIGHT";
    case OpCode::JUMP_IF_NOT_LESS_LOCAL_RIGHT:
        return "JUMP_IF_NOT_LESS_LOCAL_RIGHT";
    case OpCode::JUMP_IF_NOT_LESS_EQUAL_LOCAL_RIGHT:
        return "JUMP_IF_NOT_LESS_EQUAL_LOCAL_RIGHT";
    case OpCode::JUMP_IF_NOT_GREATER_LOCAL_RIGHT:
        return "JUMP_IF_NOT_GREATER_LOCAL_RIGHT";
    case OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT:
        return "JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT";

    case OpCode::JUMP_IF_NOT_EQUAL_LITERAL_RIGHT:
        return "JUMP_IF_NOT_EQUAL_LITERAL_RIGHT";
    case OpCode::JUMP_IF_NOT_NOT_EQUAL_LITERAL_RIGHT:
        return "JUMP_IF_NOT_NOT_EQUAL_LITERAL_RIGHT";
    case OpCode::JUMP_IF_NOT_LESS_LITERAL_RIGHT:
        return "JUMP_IF_NOT_LESS_LITERAL_RIGHT";
    case OpCode::JUMP_IF_NOT_LESS_EQUAL_LITERAL_RIGHT:
        return "JUMP_IF_NOT_LESS_EQUAL_LITERAL_RIGHT";
    case OpCode::JUMP_IF_NOT_GREATER_LITERAL_RIGHT:
        return "JUMP_IF_NOT_GREATER_LITERAL_RIGHT";
    case OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT:
        return "JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT";

    case OpCode::ADD_LOCAL_RIGHT_STORE:
        return "ADD_LOCAL_RIGHT_STORE";
    case OpCode::SUBTRACT_LOCAL_RIGHT_STORE:
        return "SUBTRACT_LOCAL_RIGHT_STORE";
    case OpCode::MULTIPLY_LOCAL_RIGHT_STORE:
        return "MULTIPLY_LOCAL_RIGHT_STORE";
    case OpCode::DIVIDE_LOCAL_RIGHT_STORE:
        return "DIVIDE_LOCAL_RIGHT_STORE";
    case OpCode::MODULO_LOCAL_RIGHT_STORE:
        return "MODULO_LOCAL_RIGHT_STORE";

    case OpCode::ADD_LITERAL_RIGHT_STORE:
        return "ADD_LITERAL_RIGHT_STORE";
    case OpCode::SUBTRACT_LITERAL_RIGHT_STORE:
        return "SUBTRACT_LITERAL_RIGHT_STORE";
    case OpCode::MULTIPLY_LITERAL_RIGHT_STORE:
        return "MULTIPLY_LITERAL_RIGHT_STORE";
    case OpCode::DIVIDE_LITERAL_RIGHT_STORE:
        return "DIVIDE_LITERAL_RIGHT_STORE";
    case OpCode::MODULO_LITERAL_RIGHT_STORE:
        return "MODULO_LITERAL_RIGHT_STORE";

    case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_EQUAL_LITERAL:
        return "JUMP_IF_NOT_ADD_LOCAL_LOCAL_EQUAL_LITERAL";
    case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_NOT_EQUAL_LITERAL:
        return "JUMP_IF_NOT_ADD_LOCAL_LOCAL_NOT_EQUAL_LITERAL";
    case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_LITERAL:
        return "JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_LITERAL";
    case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_EQUAL_LITERAL:
        return "JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_EQUAL_LITERAL";
    case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL:
        return "JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL";
    case OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_EQUAL_LITERAL:
        return "JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_EQUAL_LITERAL";

    case OpCode::MULTIPLY_ADD_LOCAL_RIGHT_STORE:
        return "MULTIPLY_ADD_LOCAL_RIGHT_STORE";

    case OpCode::ADD_LOCAL_LITERAL_STORE_JUMP:
        return "ADD_LOCAL_LITERAL_STORE_JUMP";

    default:
        return "UNKNOWN_OPCODE";
    }
}
