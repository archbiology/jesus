#include "compiler.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/expr/get_attr_expr.hpp"
#include "interpreter/runtime/method.hpp"
#include "spirit/heart.hpp"

Chunk Compiler::compile(const std::vector<std::unique_ptr<Stmt>> &statements)
{
    chunk.instructions.clear();
    chunk.literals.clear();

    localSlots.clear();
    localCount = 0;
    compilingMethodBody = false;
    currentChunk = &chunk;

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

    if (auto assign = dynamic_cast<const AssignStmt *>(&stmt))
    {
        compileAssignStmt(*assign);
        return;
    }

    if (auto if_stmt = dynamic_cast<const IfStmt *>(&stmt))
    {
        compileIfStmt(*if_stmt);
        return;
    }

    if (auto return_stmt = dynamic_cast<const ReturnStmt *>(&stmt))
    {
        compileReturnStmt(*return_stmt);
        return;
    }

    if (auto repeat_while = dynamic_cast<const RepeatWhileStmt *>(&stmt))
    {
        compileRepeatWhileStmt(*repeat_while);
        return;
    }

    if (auto create_class = dynamic_cast<const CreateClassStmt *>(&stmt))
    {
        compileCreateClassStmt(*create_class);
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

    if (auto instance = dynamic_cast<const CreateInstanceExpr *>(&expr))
    {
        compileCreateInstanceExpr(*instance);
        return;
    }

    if (auto methodCall = dynamic_cast<const MethodCallExpr *>(&expr))
    {
        compileMethodCallExpr(*methodCall);
        return;
    }

    throw std::runtime_error("Expression not supported by VM yet: " + expr.toString());
}

// Fused binary operations come in families that share one opcode per
// operator and differ only in where the operands live. fusedOpIndex() maps
// an operator token to its position inside a family; the tables below map
// that position to the concrete opcode of each family.
enum
{
    FUSED_ADD = 0,
    FUSED_SUBTRACT,
    FUSED_MULTIPLY,
    FUSED_DIVIDE,
    FUSED_MODULO,
    FUSED_EQUAL,
    FUSED_NOT_EQUAL,
    FUSED_LESS,
    FUSED_LESS_EQUAL,
    FUSED_GREATER,
    FUSED_GREATER_EQUAL,
    FUSED_OP_COUNT
};

static int fusedOpIndex(TokenType type)
{
    switch (type)
    {
    case TokenType::PLUS:
        return FUSED_ADD;
    case TokenType::MINUS:
        return FUSED_SUBTRACT;
    case TokenType::STAR:
        return FUSED_MULTIPLY;
    case TokenType::SLASH:
        return FUSED_DIVIDE;
    case TokenType::MOD:
        return FUSED_MODULO;
    case TokenType::IS:
        return FUSED_EQUAL;
    case TokenType::NOT_EQUAL:
        return FUSED_NOT_EQUAL;
    case TokenType::LESS:
        return FUSED_LESS;
    case TokenType::LESS_EQUAL:
        return FUSED_LESS_EQUAL;
    case TokenType::GREATER:
        return FUSED_GREATER;
    case TokenType::GREATER_EQUAL:
        return FUSED_GREATER_EQUAL;
    default:
        return -1;
    }
}

static const OpCode kLocalLocalOps[FUSED_OP_COUNT] = {
    OpCode::ADD_LOCAL_LOCAL, OpCode::SUBTRACT_LOCAL_LOCAL, OpCode::MULTIPLY_LOCAL_LOCAL,
    OpCode::DIVIDE_LOCAL_LOCAL, OpCode::MODULO_LOCAL_LOCAL, OpCode::EQUAL_LOCAL_LOCAL,
    OpCode::NOT_EQUAL_LOCAL_LOCAL, OpCode::LESS_LOCAL_LOCAL, OpCode::LESS_EQUAL_LOCAL_LOCAL,
    OpCode::GREATER_LOCAL_LOCAL, OpCode::GREATER_EQUAL_LOCAL_LOCAL};

static const OpCode kLocalLiteralOps[FUSED_OP_COUNT] = {
    OpCode::ADD_LOCAL_LITERAL, OpCode::SUBTRACT_LOCAL_LITERAL, OpCode::MULTIPLY_LOCAL_LITERAL,
    OpCode::DIVIDE_LOCAL_LITERAL, OpCode::MODULO_LOCAL_LITERAL, OpCode::EQUAL_LOCAL_LITERAL,
    OpCode::NOT_EQUAL_LOCAL_LITERAL, OpCode::LESS_LOCAL_LITERAL, OpCode::LESS_EQUAL_LOCAL_LITERAL,
    OpCode::GREATER_LOCAL_LITERAL, OpCode::GREATER_EQUAL_LOCAL_LITERAL};

static const OpCode kLiteralLocalOps[FUSED_OP_COUNT] = {
    OpCode::ADD_LITERAL_LOCAL, OpCode::SUBTRACT_LITERAL_LOCAL, OpCode::MULTIPLY_LITERAL_LOCAL,
    OpCode::DIVIDE_LITERAL_LOCAL, OpCode::MODULO_LITERAL_LOCAL, OpCode::EQUAL_LITERAL_LOCAL,
    OpCode::NOT_EQUAL_LITERAL_LOCAL, OpCode::LESS_LITERAL_LOCAL, OpCode::LESS_EQUAL_LITERAL_LOCAL,
    OpCode::GREATER_LITERAL_LOCAL, OpCode::GREATER_EQUAL_LITERAL_LOCAL};

static const OpCode kLocalRightOps[FUSED_OP_COUNT] = {
    OpCode::ADD_LOCAL_RIGHT, OpCode::SUBTRACT_LOCAL_RIGHT, OpCode::MULTIPLY_LOCAL_RIGHT,
    OpCode::DIVIDE_LOCAL_RIGHT, OpCode::MODULO_LOCAL_RIGHT, OpCode::EQUAL_LOCAL_RIGHT,
    OpCode::NOT_EQUAL_LOCAL_RIGHT, OpCode::LESS_LOCAL_RIGHT, OpCode::LESS_EQUAL_LOCAL_RIGHT,
    OpCode::GREATER_LOCAL_RIGHT, OpCode::GREATER_EQUAL_LOCAL_RIGHT};

static const OpCode kLiteralRightOps[FUSED_OP_COUNT] = {
    OpCode::ADD_LITERAL_RIGHT, OpCode::SUBTRACT_LITERAL_RIGHT, OpCode::MULTIPLY_LITERAL_RIGHT,
    OpCode::DIVIDE_LITERAL_RIGHT, OpCode::MODULO_LITERAL_RIGHT, OpCode::EQUAL_LITERAL_RIGHT,
    OpCode::NOT_EQUAL_LITERAL_RIGHT, OpCode::LESS_LITERAL_RIGHT, OpCode::LESS_EQUAL_LITERAL_RIGHT,
    OpCode::GREATER_LITERAL_RIGHT, OpCode::GREATER_EQUAL_LITERAL_RIGHT};

// Maps a fused comparison opcode to its comparison-and-branch counterpart.
// Returns false for anything that is not a comparison.
static bool branchVariant(OpCode comparison, OpCode &branch)
{
    struct Pair
    {
        OpCode comparison;
        OpCode branch;
    };

    static const Pair kBranches[] = {
        {OpCode::EQUAL_LOCAL_LOCAL, OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LOCAL},
        {OpCode::NOT_EQUAL_LOCAL_LOCAL, OpCode::JUMP_IF_NOT_NOT_EQUAL_LOCAL_LOCAL},
        {OpCode::LESS_LOCAL_LOCAL, OpCode::JUMP_IF_NOT_LESS_LOCAL_LOCAL},
        {OpCode::LESS_EQUAL_LOCAL_LOCAL, OpCode::JUMP_IF_NOT_LESS_EQUAL_LOCAL_LOCAL},
        {OpCode::GREATER_LOCAL_LOCAL, OpCode::JUMP_IF_NOT_GREATER_LOCAL_LOCAL},
        {OpCode::GREATER_EQUAL_LOCAL_LOCAL, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL},

        {OpCode::EQUAL_LOCAL_LITERAL, OpCode::JUMP_IF_NOT_EQUAL_LOCAL_LITERAL},
        {OpCode::NOT_EQUAL_LOCAL_LITERAL, OpCode::JUMP_IF_NOT_NOT_EQUAL_LOCAL_LITERAL},
        {OpCode::LESS_LOCAL_LITERAL, OpCode::JUMP_IF_NOT_LESS_LOCAL_LITERAL},
        {OpCode::LESS_EQUAL_LOCAL_LITERAL, OpCode::JUMP_IF_NOT_LESS_EQUAL_LOCAL_LITERAL},
        {OpCode::GREATER_LOCAL_LITERAL, OpCode::JUMP_IF_NOT_GREATER_LOCAL_LITERAL},
        {OpCode::GREATER_EQUAL_LOCAL_LITERAL, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL},

        {OpCode::EQUAL_LITERAL_LOCAL, OpCode::JUMP_IF_NOT_EQUAL_LITERAL_LOCAL},
        {OpCode::NOT_EQUAL_LITERAL_LOCAL, OpCode::JUMP_IF_NOT_NOT_EQUAL_LITERAL_LOCAL},
        {OpCode::LESS_LITERAL_LOCAL, OpCode::JUMP_IF_NOT_LESS_LITERAL_LOCAL},
        {OpCode::LESS_EQUAL_LITERAL_LOCAL, OpCode::JUMP_IF_NOT_LESS_EQUAL_LITERAL_LOCAL},
        {OpCode::GREATER_LITERAL_LOCAL, OpCode::JUMP_IF_NOT_GREATER_LITERAL_LOCAL},
        {OpCode::GREATER_EQUAL_LITERAL_LOCAL, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL},

        {OpCode::EQUAL_LOCAL_RIGHT, OpCode::JUMP_IF_NOT_EQUAL_LOCAL_RIGHT},
        {OpCode::NOT_EQUAL_LOCAL_RIGHT, OpCode::JUMP_IF_NOT_NOT_EQUAL_LOCAL_RIGHT},
        {OpCode::LESS_LOCAL_RIGHT, OpCode::JUMP_IF_NOT_LESS_LOCAL_RIGHT},
        {OpCode::LESS_EQUAL_LOCAL_RIGHT, OpCode::JUMP_IF_NOT_LESS_EQUAL_LOCAL_RIGHT},
        {OpCode::GREATER_LOCAL_RIGHT, OpCode::JUMP_IF_NOT_GREATER_LOCAL_RIGHT},
        {OpCode::GREATER_EQUAL_LOCAL_RIGHT, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT},

        {OpCode::EQUAL_LITERAL_RIGHT, OpCode::JUMP_IF_NOT_EQUAL_LITERAL_RIGHT},
        {OpCode::NOT_EQUAL_LITERAL_RIGHT, OpCode::JUMP_IF_NOT_NOT_EQUAL_LITERAL_RIGHT},
        {OpCode::LESS_LITERAL_RIGHT, OpCode::JUMP_IF_NOT_LESS_LITERAL_RIGHT},
        {OpCode::LESS_EQUAL_LITERAL_RIGHT, OpCode::JUMP_IF_NOT_LESS_EQUAL_LITERAL_RIGHT},
        {OpCode::GREATER_LITERAL_RIGHT, OpCode::JUMP_IF_NOT_GREATER_LITERAL_RIGHT},
        {OpCode::GREATER_EQUAL_LITERAL_RIGHT, OpCode::JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT},
    };

    for (const auto &pair : kBranches)
    {
        if (pair.comparison == comparison)
        {
            branch = pair.branch;
            return true;
        }
    }

    return false;
}

// Maps a comparison whose LEFT operand is on the stack (the *_LITERAL_RIGHT
// forms) to the branch that folds in the ADD which produced that operand.
// This recognises the divergence test `if x2 + y2 > 4.0` and turns the ADD
// plus the comparison-branch into a single instruction.
static bool sumLiteralBranch(OpCode comparison, OpCode &branch)
{
    switch (comparison)
    {
    case OpCode::EQUAL_LITERAL_RIGHT:
        branch = OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_EQUAL_LITERAL;
        return true;
    case OpCode::NOT_EQUAL_LITERAL_RIGHT:
        branch = OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_NOT_EQUAL_LITERAL;
        return true;
    case OpCode::LESS_LITERAL_RIGHT:
        branch = OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_LITERAL;
        return true;
    case OpCode::LESS_EQUAL_LITERAL_RIGHT:
        branch = OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_EQUAL_LITERAL;
        return true;
    case OpCode::GREATER_LITERAL_RIGHT:
        branch = OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL;
        return true;
    case OpCode::GREATER_EQUAL_LITERAL_RIGHT:
        branch = OpCode::JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_EQUAL_LITERAL;
        return true;
    default:
        return false;
    }
}

// Looks up the opcode for `type` in a family table. Returns false for
// operators without a fused form (OR/AND/XOR).
static bool fusedOp(const OpCode *table, TokenType type, OpCode &out)
{
    const int index = fusedOpIndex(type);

    if (index < 0)
        return false;

    out = table[index];
    return true;
}

// Maps a fused two-locals opcode to its "store result into a local" form.
// Returns false for comparison opcodes, which have no store form (their
// result is consumed by a conditional jump rather than stored).
static bool localLocalStoreOpcode(OpCode fused, OpCode &out)
{
    switch (fused)
    {
    case OpCode::ADD_LOCAL_LOCAL:
        out = OpCode::ADD_LOCAL_LOCAL_STORE;
        return true;
    case OpCode::SUBTRACT_LOCAL_LOCAL:
        out = OpCode::SUBTRACT_LOCAL_LOCAL_STORE;
        return true;
    case OpCode::MULTIPLY_LOCAL_LOCAL:
        out = OpCode::MULTIPLY_LOCAL_LOCAL_STORE;
        return true;
    case OpCode::DIVIDE_LOCAL_LOCAL:
        out = OpCode::DIVIDE_LOCAL_LOCAL_STORE;
        return true;
    case OpCode::MODULO_LOCAL_LOCAL:
        out = OpCode::MODULO_LOCAL_LOCAL_STORE;
        return true;
    default:
        return false;
    }
}

// Same idea for the local/literal family (e.g. `i = i + 1`).
static bool localLiteralStoreOpcode(OpCode fused, OpCode &out)
{
    switch (fused)
    {
    case OpCode::ADD_LOCAL_LITERAL:
        out = OpCode::ADD_LOCAL_LITERAL_STORE;
        return true;
    case OpCode::SUBTRACT_LOCAL_LITERAL:
        out = OpCode::SUBTRACT_LOCAL_LITERAL_STORE;
        return true;
    case OpCode::MULTIPLY_LOCAL_LITERAL:
        out = OpCode::MULTIPLY_LOCAL_LITERAL_STORE;
        return true;
    case OpCode::DIVIDE_LOCAL_LITERAL:
        out = OpCode::DIVIDE_LOCAL_LITERAL_STORE;
        return true;
    case OpCode::MODULO_LOCAL_LITERAL:
        out = OpCode::MODULO_LOCAL_LITERAL_STORE;
        return true;
    default:
        return false;
    }
}

// For the *_RIGHT family, where the result currently replaces a value on the
// stack (e.g. `x = (x2 - y2) + cx`).
static bool localRightStoreOpcode(OpCode fused, OpCode &out)
{
    switch (fused)
    {
    case OpCode::ADD_LOCAL_RIGHT:
        out = OpCode::ADD_LOCAL_RIGHT_STORE;
        return true;
    case OpCode::SUBTRACT_LOCAL_RIGHT:
        out = OpCode::SUBTRACT_LOCAL_RIGHT_STORE;
        return true;
    case OpCode::MULTIPLY_LOCAL_RIGHT:
        out = OpCode::MULTIPLY_LOCAL_RIGHT_STORE;
        return true;
    case OpCode::DIVIDE_LOCAL_RIGHT:
        out = OpCode::DIVIDE_LOCAL_RIGHT_STORE;
        return true;
    case OpCode::MODULO_LOCAL_RIGHT:
        out = OpCode::MODULO_LOCAL_RIGHT_STORE;
        return true;
    default:
        return false;
    }
}

static bool literalRightStoreOpcode(OpCode fused, OpCode &out)
{
    switch (fused)
    {
    case OpCode::ADD_LITERAL_RIGHT:
        out = OpCode::ADD_LITERAL_RIGHT_STORE;
        return true;
    case OpCode::SUBTRACT_LITERAL_RIGHT:
        out = OpCode::SUBTRACT_LITERAL_RIGHT_STORE;
        return true;
    case OpCode::MULTIPLY_LITERAL_RIGHT:
        out = OpCode::MULTIPLY_LITERAL_RIGHT_STORE;
        return true;
    case OpCode::DIVIDE_LITERAL_RIGHT:
        out = OpCode::DIVIDE_LITERAL_RIGHT_STORE;
        return true;
    case OpCode::MODULO_LITERAL_RIGHT:
        out = OpCode::MODULO_LITERAL_RIGHT_STORE;
        return true;
    default:
        return false;
    }
}

void Compiler::compileBinaryExpr(const BinaryExpr &expr)
{
    compileExpr(*expr.left);
    compileExpr(*expr.right);

    // -----------------------------------------------------------------
    // Peephole: fold operand fetches plus the binary op into a single fused
    // opcode that carries the operands in its own operand. This is the
    // dominant pattern in tight numeric loops.
    //
    // When both operands are simple fetches:
    //   READ_LOCAL a; READ_LOCAL b; <op>   -> <OP>_LOCAL_LOCAL   (a, b)
    //   READ_LOCAL a; PUSH_LITERAL k; <op> -> <OP>_LOCAL_LITERAL (a, k)
    //   PUSH_LITERAL k; READ_LOCAL b; <op> -> <OP>_LITERAL_LOCAL (k, b)
    //
    // When only the right operand is a fetch, the left one is already on
    // the stack and the operation reuses its slot:
    //   ...; READ_LOCAL b; <op>   -> ...; <OP>_LOCAL_RIGHT   (b)
    //   ...; PUSH_LITERAL k; <op> -> ...; <OP>_LITERAL_RIGHT (k)
    // -----------------------------------------------------------------
    {
        auto &ins = currentChunk->instructions;

        if (!ins.empty())
        {
            const Instruction &last = ins[ins.size() - 1];

            OpCode fused;
            bool matched = false;
            uint32_t packed = 0;
            int pops = 0;

            if (ins.size() >= 2)
            {
                const Instruction &beforeLast = ins[ins.size() - 2];

                if (fusedOp(kLocalLocalOps, expr.op.type, fused) &&
                    beforeLast.opcode == OpCode::READ_LOCAL &&
                    last.opcode == OpCode::READ_LOCAL &&
                    beforeLast.operand < 0x10000 && last.operand < 0x10000)
                {
                    packed = beforeLast.operand | (last.operand << 16);
                    matched = true;
                    pops = 2;
                }
                else if (fusedOp(kLocalLiteralOps, expr.op.type, fused) &&
                         beforeLast.opcode == OpCode::READ_LOCAL &&
                         last.opcode == OpCode::PUSH_LITERAL &&
                         beforeLast.operand < 0x10000 && last.operand < 0x10000)
                {
                    packed = beforeLast.operand | (last.operand << 16);
                    matched = true;
                    pops = 2;
                }
                else if (fusedOp(kLiteralLocalOps, expr.op.type, fused) &&
                         beforeLast.opcode == OpCode::PUSH_LITERAL &&
                         last.opcode == OpCode::READ_LOCAL &&
                         beforeLast.operand < 0x10000 && last.operand < 0x10000)
                {
                    packed = last.operand | (beforeLast.operand << 16);
                    matched = true;
                    pops = 2;
                }
            }

            if (!matched && last.opcode == OpCode::READ_LOCAL &&
                fusedOp(kLocalRightOps, expr.op.type, fused) && last.operand < 0x10000)
            {
                packed = last.operand;
                matched = true;
                pops = 1;
            }

            if (!matched && last.opcode == OpCode::PUSH_LITERAL &&
                fusedOp(kLiteralRightOps, expr.op.type, fused) && last.operand < 0x10000)
            {
                packed = last.operand;
                matched = true;
                pops = 1;
            }

            if (matched)
            {
                ins.resize(ins.size() - pops);
                emit(fused, packed);
                return;
            }
        }
    }

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
    currentChunk->literals.push_back(value);

    return static_cast<uint32_t>(currentChunk->literals.size() - 1);
}

void Compiler::emit(OpCode opcode)
{
    currentChunk->instructions.push_back({opcode, 0});
}

void Compiler::emit(OpCode opcode, uint32_t operand)
{
    currentChunk->instructions.push_back({opcode, operand});
}

void Compiler::emitStoreToLocal(uint32_t slot)
{
    auto &ins = currentChunk->instructions;

    if (ins.empty() || slot >= 0x100)
    {
        emit(OpCode::WRITE_LOCAL, slot);
        return;
    }

    const OpCode last = ins.back().opcode;

    // Arithmetic fused forms have a store variant; comparison results are
    // consumed by conditional jumps instead of being stored.
    if (OpCode store; localLocalStoreOpcode(last, store))
    {
        // LOCAL_LOCAL packs its two source slots as 16-bit fields; the store
        // form repacks them as 8-bit fields plus the destination slot.
        const uint32_t left = ins.back().operand & 0xFFFF;
        const uint32_t right = ins.back().operand >> 16;

        if (left < 0x100 && right < 0x100)
        {
            ins.back() = {store, left | (right << 8) | (slot << 16)};
            return;
        }
    }
    else if (OpCode store; localLiteralStoreOpcode(last, store))
    {
        // LOCAL_LITERAL packs a 16-bit slot and a 16-bit constant; the store
        // form uses 8 bits for the slot and 24 for the constant plus dest.
        const uint32_t local = ins.back().operand & 0xFFFF;
        const uint32_t constant = ins.back().operand >> 16;

        if (local < 0x100 && constant < 0x10000)
        {
            ins.back() = {store, local | (constant << 8) | (slot << 24)};
            return;
        }
    }
    else if (OpCode store; localRightStoreOpcode(last, store))
    {
        // The *_RIGHT form already holds the right-hand slot in the low 16
        // bits; add the destination slot in the high 16 bits.
        ins.back() = {store, ins.back().operand | (slot << 16)};
        return;
    }
    else if (OpCode store; literalRightStoreOpcode(last, store))
    {
        // Same, but the low 16 bits hold a constant index.
        ins.back() = {store, ins.back().operand | (slot << 16)};
        return;
    }

    emit(OpCode::WRITE_LOCAL, slot);
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

    if (compilingMethodBody)
    {
        emitStoreToLocal(localSlot(stmt.name));
        return;
    }

    uint32_t index = registerGlobalVar(stmt.name);

    emit(OpCode::CREATE_GLOBAL, index);
}

uint32_t Compiler::localSlot(const std::string &name)
{
    auto it = localSlots.find(name);
    if (it != localSlots.end())
        return it->second;

    uint32_t slot = localCount++;
    localSlots[name] = slot;

    return slot;
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
    if (compilingMethodBody)
    {
        auto it = localSlots.find(expr.name);
        if (it != localSlots.end())
        {
            emit(OpCode::READ_LOCAL, it->second);
            return;
        }
    }

    uint32_t index = getGlobalVar(expr.name);

    emit(OpCode::READ_GLOBAL, index);
}

void Compiler::compileUpdateVarStmt(const UpdateVarStmt &stmt)
{
    compileExpr(*stmt.value);

    if (compilingMethodBody)
    {
        auto it = localSlots.find(stmt.name);
        if (it != localSlots.end())
        {
            emitStoreToLocal(it->second);
            return;
        }
    }

    uint32_t index = getGlobalVar(stmt.name);

    emit(OpCode::WRITE_GLOBAL, index);
}

uint32_t Compiler::currentOffset() const
{
    return currentChunk->instructions.size();
}

uint32_t Compiler::emitPlaceholder(OpCode opcode)
{
    uint32_t index = currentChunk->instructions.size();

    emit(opcode, 0);

    return index;
}

uint32_t Compiler::emitConditionalJump()
{
    auto &instructions = currentChunk->instructions;

    if (!instructions.empty())
    {
        // Fold `(a + b) CMP constant` — the ADD that produced the comparison's
        // left operand and the comparison itself become one branch.
        if (instructions.size() >= 2)
        {
            const Instruction &cmp = instructions.back();
            const Instruction &add = instructions[instructions.size() - 2];

            OpCode sumBranch;

            if (add.opcode == OpCode::ADD_LOCAL_LOCAL &&
                sumLiteralBranch(cmp.opcode, sumBranch))
            {
                const uint32_t a = add.operand & 0xFFFF;
                const uint32_t b = add.operand >> 16;
                const uint32_t constant = cmp.operand & 0xFFFF;

                if (a < 0x100 && b < 0x100 && constant < 0x10000)
                {
                    // Drop the comparison; rewrite the ADD in place as the
                    // combined branch, packing a, b and the constant.
                    instructions.pop_back();
                    instructions.back() = {sumBranch, a | (b << 8) | (constant << 16)};

                    return emitPlaceholder(OpCode::JUMP);
                }
            }
        }

        OpCode branch;

        if (branchVariant(instructions.back().opcode, branch))
        {
            // Turn the comparison into a comparison-and-branch. It keeps its
            // packed operands and reads the jump target from the operand of
            // the instruction emitted just below.
            instructions.back().opcode = branch;

            return emitPlaceholder(OpCode::JUMP);
        }
    }

    return emitPlaceholder(OpCode::JUMP_IF_FALSE);
}

void Compiler::patchJump(uint32_t instructionIndex)
{
    currentChunk->instructions[instructionIndex].operand = currentChunk->instructions.size();
}

void Compiler::compileRepeatWhileStmt(const RepeatWhileStmt &stmt)
{
    uint32_t loopStart = currentOffset();

    compileExpr(*stmt.condition);

    uint32_t jumpOut = emitConditionalJump();

    for (const auto &bodyStmt : stmt.body)
    {
        compileStmt(*bodyStmt);
    }

    emit(OpCode::JUMP, loopStart);

    patchJump(jumpOut);
}

void Compiler::compileCreateClassStmt(const CreateClassStmt &stmt)
{
    std::vector<std::shared_ptr<IConstraint>> constraints; // no constraints yet

    auto scope = std::make_shared<Heart>("class::" + stmt.name);
    auto userClass = stmt.userClass;

    for (const auto &member : stmt.body)
    {
        // -----------------
        // handle attributes
        // -----------------
        if (auto attr = dynamic_cast<CreateVarStmt *>(member.get()))
        {
            auto initialValue = attr->value->evaluate(scope);
            userClass->class_attributes->updateVar(attr->address.slot, initialValue);
        }
        // --------------
        // handle methods
        // --------------
        else if (auto methodStmt = dynamic_cast<CreateMethodStmt *>(member.get()))
        {
            compileMethodBody(*methodStmt);
        }
        else
        {
            throw std::runtime_error("Class body member not supported yet: " + member->toString());
        }
    }

    classes[stmt.name] = std::move(userClass);
}

void Compiler::compileCreateInstanceExpr(const CreateInstanceExpr &expr)
{
    auto it = classes.find(expr.klass->name);
    if (it == classes.end())
        throw std::runtime_error("Unknown class: " + expr.klass->name);

    uint32_t classIndex = addConstant(Value(it->second));
    emit(OpCode::PUSH_LITERAL, classIndex);
    emit(OpCode::CREATE_INSTANCE);
}

void Compiler::compileMethodCallExpr(const MethodCallExpr &expr)
{
    // ---------------------------------------------------------------
    // For a method call:
    //
    //     object.method(arg1, arg2, ..., argN)
    //
    // compile the object first, followed by the arguments:
    //
    //     <object> <arg1> ... <argN> CALL <method>
    //
    // The object is the instance on which the method is called.
    // The VM binds the arguments to local slots 1..N, so every parameter
    // must currently be provided. Calls relying on default values
    // (argIndices) are not compiled yet.
    // ---------------------------------------------------------------
    compileExpr(*expr.object);

    auto method = std::dynamic_pointer_cast<Method>(expr.method);
    if (method)
    {
        uint32_t paramsCount = method->params ? method->params->paramsCount : 0;

        if (expr.args.size() != paramsCount)
            throw std::runtime_error(
                "Calls with default parameter values are not supported by the VM yet: " + method->name);
    }

    for (const auto &arg : expr.args)
    {
        compileExpr(*arg);
    }

    uint32_t methodIndex = addConstant(Value(expr.method));

    emit(OpCode::CALL, methodIndex);
}

void Compiler::compileMethodBody(const CreateMethodStmt &method)
{
    // ---------------------------------------------------------------
    // Every method body is compiled into its own Chunk, so the VM can
    // run it with its own localSlots:
    //
    //     slot 0   -> "$self", the instance the method was called on
    //     slot 1.. -> the parameters, then the vars created in the body
    //
    // Reaching the end of a body returns nothing;
    // an explicit 'return' is not compiled by the VM yet.
    // ---------------------------------------------------------------

    // Saving the top-level context, so a method body does not leak its
    // localSlots into the program being compiled.
    auto previousChunk = currentChunk;
    auto previousLocals = std::move(localSlots);
    auto previouscompilingMethodBody = compilingMethodBody;
    auto previousLocalCount = localCount;

    Chunk bodyChunk;
    currentChunk = &bodyChunk;
    compilingMethodBody = true;
    localSlots.clear();
    localCount = 0;

    localSlots[SELF_VARIABLE] = localCount++;

    if (method.params)
    {
        for (const auto &paramName : method.params->getParameterNames())
        {
            localSlots[paramName] = localCount++;
        }
    }

    for (const auto &stmt : method.body)
    {
        compileStmt(*stmt);
    }

    // The body ended without an explicit return, so it returns nothing.
    emit(OpCode::PUSH_LITERAL, addConstant(Value::formless()));
    emit(OpCode::RETURN);

    bodyChunk.localCount = localCount;
    method.compiledBody = std::make_unique<Chunk>(std::move(bodyChunk));

    currentChunk = previousChunk;
    localSlots = std::move(previousLocals);
    compilingMethodBody = previouscompilingMethodBody;
    localCount = previousLocalCount;
}

void Compiler::compileAssignStmt(const AssignStmt &stmt)
{
    // ---------------------------------------------------------------
    // Only attribute assignment is supported for now:
    //
    //     adam name = "Adam"
    //
    // An indexed target (`list[0] = 10`) needs its own instruction,
    // since the index is an expression that has to be evaluated
    // before the value is written.
    // ---------------------------------------------------------------
    auto attribute = dynamic_cast<const GetAttributeExpr *>(stmt.target.get());
    if (!attribute)
        throw std::runtime_error("Assignment target not supported by VM yet: " + stmt.target->toString());

    // --------------------------------------------
    // Stack layout for WRITE_ATTR:
    //  first: the instance that owns the attribute,
    //  then:  the value being written to it.
    // --------------------------------------------
    compileExpr(*attribute->object);
    compileExpr(*stmt.value);

    emit(OpCode::WRITE_ATTR, attribute->address.slot);
}

void Compiler::compileIfStmt(const IfStmt &stmt)
{
    // ---------------------------------------------------------------
    // Compiles:
    //
    //     if condition:
    //         then
    //     otherwise:
    //         otherwise
    //     amen
    //
    // Control flow shape:
    //
    //   evaluate condition
    //   JUMP_IF_FALSE -> otherwise (or to the end when there is no 'othewise')
    //   then
    //   JUMP -> end         (only when there is an 'otherwise' branch)
    //   otherwise
    //   end:
    // ---------------------------------------------------------------
    compileExpr(*stmt.condition);

    uint32_t jumpToOtherwise = emitConditionalJump();

    for (const auto &thenStmt : stmt.thenBranch)
    {
        compileStmt(*thenStmt);
    }

    if (stmt.otherwiseBranch.empty())
    {
        // No 'otherwise'; let's end here.
        patchJump(jumpToOtherwise);
        return;
    }

    uint32_t jumpToEnd = emitPlaceholder(OpCode::JUMP);

    patchJump(jumpToOtherwise);
    for (const auto &otherwiseStmt : stmt.otherwiseBranch)
    {
        compileStmt(*otherwiseStmt);
    }

    patchJump(jumpToEnd);
}

void Compiler::compileReturnStmt(const ReturnStmt &stmt)
{
    if (stmt.value)
    {
        compileExpr(*stmt.value);
    }
    else
    {
        emit(OpCode::PUSH_LITERAL, addConstant(Value::formless()));
    }
    emit(OpCode::RETURN);
}
