#include <iomanip>
#include "bytecode_inspector.hpp"
#include "ast/stmt/inspect_stmt.hpp"
#include "interpreter/interpreter.hpp"
#include "vm/compiler.hpp"
#include "utils/terminal/color.hpp"
#include "utils/terminal/terminal.hpp"

std::string style(std::string text, const char *color, bool bold = false)
{
    std::string out;

    if (bold)
        out += terminal::color::bold;

    out += color;
    out += text;
    out += terminal::color::reset;

    return out;
}

std::string maybeBold(std::string text)
{
    if (terminal::supportsColor())
        return style(text, "", true);

    return text;
}

std::string maybeRed(std::string text)
{
    if (terminal::supportsColor())
        return style(text, terminal::color::red, true);

    return text;
}

std::string maybeGreen(std::string text)
{
    if (terminal::supportsColor())
        return style(text, terminal::color::green, true);

    return text;
}

void BytecodeInspector::inspect(Interpreter &jesus, const InspectStmt &)
{
    Compiler compiler;
    Chunk chunk = compiler.compile(jesus.persistedAST);

    std::cout << "Bytecode:\n";

    for (size_t i = 0; i < chunk.instructions.size(); ++i)
    {
        auto const &instr = chunk.instructions[i];

        auto raw_opcode = opcodeToString(instr.opcode);
        std::ostringstream oss;
        oss << std::right << std::setw(14) << raw_opcode;
        std::string padded_opcode = oss.str();

        std::string opcode_str = padded_opcode;

        switch (instr.opcode)
        {
        case OpCode::JUMP:
            opcode_str = maybeGreen(padded_opcode);
            break;
        case OpCode::JUMP_IF_FALSE:
            opcode_str = maybeRed(padded_opcode);
            break;
        case OpCode::RETURN:
            opcode_str = maybeBold(padded_opcode);
            break;

        default:
            break;
        }

        std::cout << std::setw(4) << i << ": " << opcode_str;

        switch (instr.opcode)
        {
        case OpCode::PUSH_LITERAL:
        {
            const auto &value = chunk.literals[instr.operand];
            std::cout << " const[" << instr.operand << "] = " << value.toString();
            break;
        }

        case OpCode::READ_GLOBAL:
        case OpCode::WRITE_GLOBAL:
        case OpCode::CREATE_GLOBAL:
        {
            std::cout << " vars[" << instr.operand << "]";
            break;
        }

        case OpCode::JUMP:
        case OpCode::JUMP_IF_FALSE:
        {
            std::cout << " -> " << instr.operand;
            break;
        }

        default:
            break;
        }

        std::cout << "\n";
    }
}
