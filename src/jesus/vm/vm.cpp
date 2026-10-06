#include <sstream>
#include <memory>

#include "vm.hpp"
#include "opcode.hpp"
#include "ast/stmt/return_stmt.hpp"
#include "ast/expr/literal_expr.hpp"
#include "ast/expr/variable_expr.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "interpreter/runtime/instance.hpp"
#include "interpreter/runtime/method.hpp"

VM::VM(const std::unordered_map<const CreateMethodStmt *, Chunk> &methodChunks)
    : methodChunks(methodChunks)
{
}

void VM::run(const Chunk &chunk)
{
    std::cout << "[VM]\n";
    auto begin = chunk.instructions.data();
    auto ip = begin;

    // The main program is the bottom/first frame;
    frames.push_back(CallFrame{&chunk, ip, /* locals =*/ {}});

    while (true)
    {
        const size_t frameDepth = frames.size();

        begin = frames.back().chunk->instructions.data();
        ip = frames.back().ip;

        switch (ip->opcode)
        {
        case OpCode::PUSH_LITERAL:
        {
            stack.push_back(frames.back().chunk->literals[ip->operand]);

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

        case OpCode::WRITE_ATTR:
        {
            Value value = stack.back();
            stack.pop_back();

            auto instance = stack.back().toInstance();
            stack.pop_back();

            uint32_t index = ip->operand;

            instance->attributes->updateVar(index, value);

            ++ip;
            break;
        }

        case OpCode::READ_LOCAL:
        {
            stack.push_back(frames.back().locals[ip->operand]);

            ++ip;
            break;
        }

        case OpCode::WRITE_LOCAL:
        {
            Value value = stack.back();
            stack.pop_back();

            auto &locals = frames.back().locals;
            if (ip->operand >= locals.size())
                locals.resize(ip->operand + 1);

            locals[ip->operand] = value;

            ++ip;
            break;
        }

        case OpCode::JUMP_IF_FALSE:
        {
            Value condition = stack.back();
            stack.pop_back();

            if (!condition.AS_BOOLEAN)
            {
                // false ? jump to end of the loop
                ip = begin + ip->operand;
            }
            else
            {
                // true? execute the loop
                ++ip;
            }

            break;
        }

        case OpCode::JUMP:
        {
            // Back to the beginning of the loop
            ip = begin + ip->operand;
            break;
        }

        case OpCode::CREATE_INSTANCE:
        {
            auto klass = stack.back().asClass();
            stack.pop_back();

            stack.push_back(Value(std::make_shared<Instance>(klass)));

            ++ip;
            break;
        }

        case OpCode::CALL:
        {
            // -----------------------------------------------------------
            // For a method call:
            //
            //     object.method(arg1, arg2, ..., argN)
            //
            // The compiler emits:
            //
            //     <object> <arg1> ... <argN> CALL <method>
            //
            // The object is the instance on which the method is called.
            // Each argument becomes a local slot (1..N) of the new frame,
            // and the object becomes slot 0, which is "$self".
            //    ┌──────────────┬──────┐
            //    │ "$self"      │  0   │
            //    │ "name"       │  1   │
            //    │ "message"    │  2   │
            //    └──────────────┴──────┘
            // -----------------------------------------------------------
            auto method = frames.back().chunk->literals[ip->operand].asMethod();

            auto userMethod = std::dynamic_pointer_cast<Method>(method);
            if (!userMethod)
                throw std::runtime_error("VM CALL: only user methodChunks are supported in VM mode currently.");

            uint32_t paramsCount = userMethod->params ? userMethod->params->paramsCount : 0;

            auto compiled = methodChunks.find(userMethod->definition);
            if (compiled == methodChunks.end())
                throw std::runtime_error("VM CALL: method body not compiled: " + userMethod->name);

            // The last argument is on top of the stack. Let's put as last in `locals`.
            //  bottom
            //      ↓
            //  ┌─────────┐
            //  │ object  │  ← $self
            //  ├─────────┤
            //  │ arg1    │
            //  ├─────────┤
            //  │ arg2    │
            //  ├─────────┤
            //  │ arg3    │  ← stack.back()
            //  └─────────┘
            //      ↑
            //     top
            std::vector<Value> locals(paramsCount + 1);
            for (uint32_t slot = paramsCount; slot > 0; --slot)
            {
                locals[slot] = stack.back();
                stack.pop_back();
            }

            locals[0] = stack.back(); // "$self"
            stack.pop_back();

            const Chunk &body = compiled->second;
            frames.push_back(CallFrame{&body, body.instructions.data(), std::move(locals)});
            break;
        }

        case OpCode::RETURN:
        {
            if (frames.size() == 1)
            {
                // The program itself is returning, so finish execution.
                return;
            }

            // A method is returning: hand the value back to the caller.
            Value result = stack.back();
            stack.pop_back();

            frames.pop_back();

            // Resume the caller right after the CALL that started this method.
            ++frames.back().ip;

            stack.push_back(result);
            break;
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

        // ---------------------------------------------------------------
        // Save the instruction pointer only if CALL/RETURN did not change
        // the current frame.
        //
        // CALL pushes a new frame and RETURN removes one. After either
        // operation, the local `ip` may no longer belong to `frames.back()`.
        // ---------------------------------------------------------------
        if (frames.size() == frameDepth)
        {
            frames.back().ip = ip;
        }
    }
}
