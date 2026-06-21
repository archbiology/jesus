#include "compiler.hpp"

Chunk Compiler::compile(const std::vector<std::unique_ptr<Stmt>> &statements)
{
    chunk.instructions.clear();
    chunk.literals.clear();

    for (const auto &stmt : statements)
    {
        compileStmt(*stmt);
    }

    emit(OpCode::RETURN);

    return chunk;
}

void Compiler::compileStmt(const Stmt &stmt)
{
    if (auto print = dynamic_cast<const PrintStmt *>(&stmt))
    {
        compilePrintStmt(*print);
        return;
    }

    if (auto create_var = dynamic_cast<const CreateVarStmt *>(&stmt))
    {
        compileCreateVarStmt(*create_var);
        return;
    }

    if (auto update_var = dynamic_cast<const UpdateVarStmt *>(&stmt))
    {
        compileUpdateVarStmt(*update_var);
        return;
    }

    if (auto repeat_while = dynamic_cast<const RepeatWhileStmt *>(&stmt))
    {
        compileRepeatWhileStmt(*repeat_while);
        return;
    }

    throw std::runtime_error("Statement not supported by VM yet: " + stmt.toString());
}

void Compiler::compilePrintStmt(const PrintStmt &stmt)
{
    compileExpr(*stmt.message);

    emit(OpCode::PRINT);
}

void Compiler::compileExpr(const Expr &expr)
{
    if (auto literal = dynamic_cast<const LiteralExpr *>(&expr))
    {
        compileLiteralExpr(*literal);
        return;
    }

    if (auto binary = dynamic_cast<const BinaryExpr *>(&expr))
    {
        compileBinaryExpr(*binary);
        return;
    }

    if (auto var = dynamic_cast<const VariableExpr *>(&expr))
    {
        compileVariableExpr(*var);
        return;
    }

    throw std::runtime_error("Expression not supported by VM yet: " + expr.toString());
}

void Compiler::compileBinaryExpr(const BinaryExpr &expr)
{
    compileExpr(*expr.left);
    compileExpr(*expr.right);

    switch (expr.op.type)
    {
    case TokenType::PLUS:
        emit(OpCode::ADD);
        break;
    case TokenType::MINUS:
        emit(OpCode::SUBTRACT);
        break;
    case TokenType::STAR:
        emit(OpCode::MULTIPLY);
        break;
    case TokenType::SLASH:
        emit(OpCode::DIVIDE);
        break;
    case TokenType::MOD:
        emit(OpCode::MODULO);
        break;

    case TokenType::IS:
        emit(OpCode::EQUAL);
        break;
    case TokenType::NOT_EQUAL:
        emit(OpCode::NOT_EQUAL);
        break;
    case TokenType::LESS:
        emit(OpCode::LESS);
        break;
    case TokenType::LESS_EQUAL:
        emit(OpCode::LESS_EQUAL);
        break;
    case TokenType::GREATER:
        emit(OpCode::GREATER);
        break;
    case TokenType::GREATER_EQUAL:
        emit(OpCode::GREATER_EQUAL);
        break;

    case TokenType::OR:
        emit(OpCode::OR);
        break;
    case TokenType::AND:
        emit(OpCode::AND);
        break;
    case TokenType::VERSUS:
        emit(OpCode::XOR);
        break;

    default:
        throw std::runtime_error("Binary operator not supported by VM yet: " + expr.op.lexeme);
    }
}

void Compiler::compileLiteralExpr(const LiteralExpr &expr)
{
    uint32_t constantIndex = addConstant(expr.value);

    emit(OpCode::PUSH_LITERAL, constantIndex);
}

uint32_t Compiler::addConstant(const Value &value)
{
    chunk.literals.push_back(value);

    return static_cast<uint32_t>(chunk.literals.size() - 1);
}

void Compiler::emit(OpCode opcode)
{
    chunk.instructions.push_back({opcode, 0});
}

void Compiler::emit(OpCode opcode, uint32_t operand)
{
    chunk.instructions.push_back({opcode, operand});
}

uint32_t Compiler::registerGlobalVar(const std::string &name)
{
    auto it = globals.find(name);

    if (it != globals.end())
        return it->second;

    uint32_t index = globals.size();
    globals[name] = index;
    return index;
}

void Compiler::compileCreateVarStmt(const CreateVarStmt &stmt)
{
    compileExpr(*stmt.value);

    uint32_t index = registerGlobalVar(stmt.name);

    emit(OpCode::CREATE_GLOBAL, index);
}

uint32_t Compiler::getGlobalVar(const std::string &name)
{
    auto it = globals.find(name);

    if (it == globals.end())
        throw std::runtime_error("Unknown global variable: " + name);

    return it->second;
}

void Compiler::compileVariableExpr(const VariableExpr &expr)
{
    uint32_t index = getGlobalVar(expr.name);

    emit(OpCode::READ_GLOBAL, index);
}

void Compiler::compileUpdateVarStmt(const UpdateVarStmt &stmt)
{
    compileExpr(*stmt.value);

    uint32_t index = getGlobalVar(stmt.name);

    emit(OpCode::WRITE_GLOBAL, index);
}

uint32_t Compiler::currentOffset() const
{
    return chunk.instructions.size();
}

uint32_t Compiler::emitPlaceholder(OpCode opcode)
{
    uint32_t index = chunk.instructions.size();

    emit(opcode, 0);

    return index;
}

void Compiler::patchJump(uint32_t instructionIndex)
{
    chunk.instructions[instructionIndex].operand = chunk.instructions.size();
}

void Compiler::compileRepeatWhileStmt(const RepeatWhileStmt &stmt)
{
    uint32_t loopStart = currentOffset();

    compileExpr(*stmt.condition);

    uint32_t jumpOut = emitPlaceholder(OpCode::JUMP_IF_FALSE);

    for (const auto &bodyStmt : stmt.body)
    {
        compileStmt(*bodyStmt);
    }

    emit(OpCode::JUMP, loopStart);

    patchJump(jumpOut);
}
