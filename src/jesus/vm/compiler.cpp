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

// Maps a binary operator token to its fused two-locals opcode. Returns
// false for operators that have no fused form (OR/AND/XOR).
static bool fusedLocalLocalOpcode(TokenType type, OpCode &out)
{
    switch (type)
    {
    case TokenType::PLUS:
        out = OpCode::ADD_LOCAL_LOCAL;
        return true;
    case TokenType::MINUS:
        out = OpCode::SUBTRACT_LOCAL_LOCAL;
        return true;
    case TokenType::STAR:
        out = OpCode::MULTIPLY_LOCAL_LOCAL;
        return true;
    case TokenType::SLASH:
        out = OpCode::DIVIDE_LOCAL_LOCAL;
        return true;
    case TokenType::MOD:
        out = OpCode::MODULO_LOCAL_LOCAL;
        return true;
    case TokenType::IS:
        out = OpCode::EQUAL_LOCAL_LOCAL;
        return true;
    case TokenType::NOT_EQUAL:
        out = OpCode::NOT_EQUAL_LOCAL_LOCAL;
        return true;
    case TokenType::LESS:
        out = OpCode::LESS_LOCAL_LOCAL;
        return true;
    case TokenType::LESS_EQUAL:
        out = OpCode::LESS_EQUAL_LOCAL_LOCAL;
        return true;
    case TokenType::GREATER:
        out = OpCode::GREATER_LOCAL_LOCAL;
        return true;
    case TokenType::GREATER_EQUAL:
        out = OpCode::GREATER_EQUAL_LOCAL_LOCAL;
        return true;
    default:
        return false;
    }
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

void Compiler::compileBinaryExpr(const BinaryExpr &expr)
{
    compileExpr(*expr.left);
    compileExpr(*expr.right);

    // -----------------------------------------------------------------
    // Peephole: fold "READ_LOCAL a; READ_LOCAL b; <binary op>" into a
    // single fused opcode carrying both local slots in its operand. This
    // is the dominant pattern in tight numeric loops (e.g. `x * x`).
    // -----------------------------------------------------------------
    if (OpCode fused; fusedLocalLocalOpcode(expr.op.type, fused))
    {
        auto &ins = currentChunk->instructions;
        if (ins.size() >= 2 &&
            ins[ins.size() - 2].opcode == OpCode::READ_LOCAL &&
            ins[ins.size() - 1].opcode == OpCode::READ_LOCAL)
        {
            const uint32_t slotA = ins[ins.size() - 2].operand;
            const uint32_t slotB = ins[ins.size() - 1].operand;

            if (slotA < 0x10000 && slotB < 0x10000)
            {
                ins.pop_back();
                ins.pop_back();
                emit(fused, slotA | (slotB << 16));
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

    // Only the arithmetic fused forms have a store variant; comparison
    // results are consumed by conditional jumps instead of being stored.
    if (OpCode store; !ins.empty() && slot < 0x100 &&
                      localLocalStoreOpcode(ins.back().opcode, store))
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

void Compiler::patchJump(uint32_t instructionIndex)
{
    currentChunk->instructions[instructionIndex].operand = currentChunk->instructions.size();
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

    uint32_t jumpToOtherwise = emitPlaceholder(OpCode::JUMP_IF_FALSE);

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
