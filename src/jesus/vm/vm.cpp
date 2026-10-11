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

void VM::run(const Chunk &chunk)
{
    std::cout << "[VM]\n";
    const Instruction *begin = chunk.instructions.data();
    const Instruction *ip = begin;
    const Chunk *currentChunk = &chunk;

    // The main program is the bottom/first frame; it has no locals.
    frames.push_back(CallFrame{&chunk, begin, 0});

    // -----------------------------------------------------------------
    // Computed-goto dispatch (GCC/Clang GNU extension).
    //
    // Every opcode gets a dedicated label in the interpreter loop; the
    // dispatch table maps an opcode to the address of its label. This is
    // faster than a plain switch/jump-table because the "next instruction"
    // dispatch is folded into the tail of every case (no re-switching).
    // -----------------------------------------------------------------
    static const void *const dispatchTable[] = {
        &&OP_PUSH_LITERAL,    // PUSH_LITERAL
        &&OP_CREATE_GLOBAL,   // CREATE_GLOBAL
        &&OP_READ_GLOBAL,     // READ_GLOBAL
        &&OP_WRITE_GLOBAL,    // WRITE_GLOBAL
        &&OP_READ_LOCAL,      // READ_LOCAL
        &&OP_WRITE_LOCAL,     // WRITE_LOCAL
        &&OP_CREATE_INSTANCE, // CREATE_INSTANCE
        &&OP_DEFAULT,         // READ_ATTR (not implemented)
        &&OP_WRITE_ATTR,      // WRITE_ATTR
        &&OP_ADD,             // ADD
        &&OP_SUBTRACT,        // SUBTRACT
        &&OP_MULTIPLY,        // MULTIPLY
        &&OP_DIVIDE,          // DIVIDE
        &&OP_MODULO,          // MODULO
        &&OP_EQUAL,           // EQUAL
        &&OP_NOT_EQUAL,       // NOT_EQUAL
        &&OP_LESS,            // LESS
        &&OP_LESS_EQUAL,      // LESS_EQUAL
        &&OP_GREATER,         // GREATER
        &&OP_GREATER_EQUAL,   // GREATER_EQUAL
        &&OP_OR,              // OR
        &&OP_AND,             // AND
        &&OP_BINARY_GENERIC,  // XOR
        &&OP_PRINT,           // PRINT
        &&OP_JUMP,            // JUMP
        &&OP_JUMP_IF_FALSE,   // JUMP_IF_FALSE
        &&OP_CALL,            // CALL
        &&OP_RETURN,          // RETURN
        &&OP_ADD_LL,          // ADD_LL
        &&OP_SUBTRACT_LL,     // SUBTRACT_LL
        &&OP_MULTIPLY_LL,     // MULTIPLY_LL
        &&OP_DIVIDE_LL,       // DIVIDE_LL
        &&OP_MODULO_LL,       // MODULO_LL
        &&OP_EQUAL_LL,        // EQUAL_LL
        &&OP_NOT_EQUAL_LL,    // NOT_EQUAL_LL
        &&OP_LESS_LL,         // LESS_LL
        &&OP_LESS_EQUAL_LL,   // LESS_EQUAL_LL
        &&OP_GREATER_LL,      // GREATER_LL
        &&OP_GREATER_EQUAL_LL // GREATER_EQUAL_LL
    };

    // Dispatch to the main program's first instruction.
    goto NEXT_OP;

    OP_PUSH_LITERAL:
        {
            stack.push_back(currentChunk->literals[ip->operand]);

            ++ip;
            goto NEXT_OP;
        }

    OP_PRINT:
        {
            std::cout << stack.back().toString() << std::endl;
            stack.pop_back();

            ++ip;
            goto NEXT_OP;
        }

    // -----------------------------------------------------------------
    // Binary operators: each opcode dispatches to its own label, so the
    // interpreter never needs a second switch on ip->opcode. The common
    // numeric case is handled in place with the raw setters (the IS_NUMBER
    // check above already proved the operands are scalars); non-numeric
    // operands fall through to OP_BINARY_GENERIC.
    // -----------------------------------------------------------------
#define BINARY_OPERANDS \
    Value &right = stack.back(); \
    Value &left = *(stack.end() - 2);

#define ARITH_BINARY(label, dblop, intop) \
    OP_##label: \
    { \
        BINARY_OPERANDS \
        if (left.IS_NUMBER && right.IS_NUMBER) \
        { \
            if (left.IS_DOUBLE || right.IS_DOUBLE) \
                left.vmSetAsDouble(left.number dblop right.number); \
            else \
                left.vmSetAsInt(left.intValue intop right.intValue); \
            stack.pop_back(); \
            ++ip; \
            goto NEXT_OP; \
        } \
        goto OP_BINARY_GENERIC; \
    }

    ARITH_BINARY(ADD, +, +)
    ARITH_BINARY(SUBTRACT, -, -)
    ARITH_BINARY(MULTIPLY, *, *)

    OP_DIVIDE:
        {
            BINARY_OPERANDS
            if (left.IS_NUMBER && right.IS_NUMBER)
            {
                const double divisor = right.number;
                if (divisor == 0.0)
                    throw std::runtime_error("Division by zero");
                left.vmSetAsDouble(left.number / divisor);
                stack.pop_back();
                ++ip;
                goto NEXT_OP;
            }
            goto OP_BINARY_GENERIC;
        }

    OP_MODULO:
        {
            BINARY_OPERANDS
            if (left.IS_NUMBER && right.IS_NUMBER)
            {
                if (left.IS_DOUBLE || right.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(left.number) + " mod " + std::to_string(right.number) + "' invalid — 'mod' only supports integers.");
                left.vmSetAsInt(left.toInt() % right.toInt());
                stack.pop_back();
                ++ip;
                goto NEXT_OP;
            }
            goto OP_BINARY_GENERIC;
        }

#define CMP_BINARY(label, dblop) \
    OP_##label: \
    { \
        BINARY_OPERANDS \
        if (left.IS_NUMBER && right.IS_NUMBER) \
        { \
            left.vmSetAsBoolean(left.number dblop right.number); \
            stack.pop_back(); \
            ++ip; \
            goto NEXT_OP; \
        } \
        goto OP_BINARY_GENERIC; \
    }

    CMP_BINARY(EQUAL, ==)
    CMP_BINARY(NOT_EQUAL, !=)
    CMP_BINARY(LESS, <)
    CMP_BINARY(LESS_EQUAL, <=)
    CMP_BINARY(GREATER, >)
    CMP_BINARY(GREATER_EQUAL, >=)

    // -----------------------------------------------------------------
    // Fused "READ_LOCAL a; READ_LOCAL b; <binary op>" handlers.
    //
    // The operand packs the two local slots: `a` in the low 16 bits and
    // `b` in the high 16 bits. The result is pushed directly onto the
    // stack, so the two operand pushes and the subsequent pop disappear.
    // Non-numeric operands fall back to the regular operator overloads
    // (which handle strings, booleans and so on).
    // -----------------------------------------------------------------
#define FUSED_OPERANDS \
    const uint32_t packed = ip->operand; \
    const std::size_t lb = frames.back().localsBase; \
    const Value &a = stack[lb + (packed & 0xFFFF)]; \
    const Value &b = stack[lb + (packed >> 16)];

#define FUSED_ARITH_LL(label, op) \
    OP_##label: \
    { \
        FUSED_OPERANDS \
        if (a.IS_NUMBER && b.IS_NUMBER) \
        { \
            if (a.IS_DOUBLE || b.IS_DOUBLE) \
            { \
                const double result = a.number op b.number; \
                stack.emplace_back(); \
                stack.back().vmSetAsDouble(result); \
            } \
            else \
            { \
                const int result = a.intValue op b.intValue; \
                stack.emplace_back(); \
                stack.back().vmSetAsInt(result); \
            } \
        } \
        else \
        { \
            stack.push_back(a op b); \
        } \
        ++ip; \
        goto NEXT_OP; \
    }

    FUSED_ARITH_LL(ADD_LL, +)
    FUSED_ARITH_LL(SUBTRACT_LL, -)
    FUSED_ARITH_LL(MULTIPLY_LL, *)

    OP_DIVIDE_LL:
        {
            FUSED_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                const double result = a.number / b.number;
                stack.emplace_back();
                stack.back().vmSetAsDouble(result);
            }
            else
            {
                stack.push_back(a / b);
            }
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LL:
        {
            FUSED_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (a.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(a.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                const int result = a.intValue % b.intValue;
                stack.emplace_back();
                stack.back().vmSetAsInt(result);
            }
            else
            {
                stack.push_back(a % b);
            }
            ++ip;
            goto NEXT_OP;
        }

#define FUSED_CMP_LL(label, op) \
    OP_##label: \
    { \
        FUSED_OPERANDS \
        if (a.IS_NUMBER && b.IS_NUMBER) \
        { \
            const bool result = a.number op b.number; \
            stack.emplace_back(); \
            stack.back().vmSetAsBoolean(result); \
        } \
        else \
        { \
            stack.push_back(Value(a op b)); \
        } \
        ++ip; \
        goto NEXT_OP; \
    }

    FUSED_CMP_LL(EQUAL_LL, ==)
    FUSED_CMP_LL(NOT_EQUAL_LL, !=)
    FUSED_CMP_LL(LESS_LL, <)
    FUSED_CMP_LL(LESS_EQUAL_LL, <=)
    FUSED_CMP_LL(GREATER_LL, >)
    FUSED_CMP_LL(GREATER_EQUAL_LL, >=)

    OP_OR:
        {
            BINARY_OPERANDS
            Value r = std::move(right);
            stack.pop_back();
            Value l = std::move(left);
            stack.back() = (l.AS_BOOLEAN ? l : r);
            ++ip;
            goto NEXT_OP;
        }

    OP_AND:
        {
            BINARY_OPERANDS
            Value r = std::move(right);
            stack.pop_back();
            Value l = std::move(left);
            stack.back() = (l.AS_BOOLEAN ? r : l);
            ++ip;
            goto NEXT_OP;
        }

    OP_BINARY_GENERIC:
        {

            Value right2 = std::move(stack.back());
            stack.pop_back();
            Value left2 = std::move(stack.back());

            switch (ip->opcode)
            {
            // ----------
            // Arithmetic
            // ----------
            case OpCode::ADD:
                stack.back() = (left2 + right2);
                break;
            case OpCode::SUBTRACT:
                stack.back() = (left2 - right2);
                break;
            case OpCode::MULTIPLY:
                stack.back() = (left2 * right2);
                break;
            case OpCode::DIVIDE:
                stack.back() = (left2 / right2);
                break;
            case OpCode::MODULO:
                stack.back() = (left2 % right2);
                break;
            // -----------------------
            // Relational / Comparison
            // -----------------------
            case OpCode::EQUAL:
                stack.back() = (Value(left2 == right2));
                break;
            case OpCode::NOT_EQUAL:
                stack.back() = (Value(left2 != right2));
                break;
            case OpCode::LESS:
                stack.back() = (Value(left2 < right2));
                break;
            case OpCode::LESS_EQUAL:
                stack.back() = (Value(left2 <= right2));
                break;
            case OpCode::GREATER:
                stack.back() = (Value(left2 > right2));
                break;
            case OpCode::GREATER_EQUAL:
                stack.back() = (Value(left2 >= right2));
                break;
            // -----
            // Logic
            // -----
            case OpCode::OR:
                stack.back() = (left2.AS_BOOLEAN ? left2 : right2);
                break;
            case OpCode::AND:
                stack.back() = (left2.AS_BOOLEAN ? right2 : left2);
                break;

            case OpCode::XOR:
                if (left2.IS_BOOLEAN && right2.IS_BOOLEAN)
                {
                    // Logical XOR
                    stack.back() = (Value(left2.AS_BOOLEAN != right2.AS_BOOLEAN));
                    break;
                }

                if (left2.IS_NUMBER && right2.IS_NUMBER)
                {
                    // Bitwise XOR
                    stack.back() = (Value(left2.toInt() ^ right2.toInt()));
                    break;
                }

                stack.back() = (Value::formless()); // fallback if types mismatch
                break;

            default:
                break;
            }

            ++ip;
            goto NEXT_OP;
        }

    OP_CREATE_GLOBAL:
        {
            Value value = std::move(stack.back());
            stack.pop_back();

            uint32_t index = ip->operand;

            if (index >= globals.size())
                globals.resize(index + 1);

            globals[index] = value;

            ++ip;
            goto NEXT_OP;
        }

    OP_READ_GLOBAL:
        {
            uint32_t index = ip->operand;

            stack.push_back(globals[index]);

            ++ip;
            goto NEXT_OP;
        }

    OP_WRITE_GLOBAL:
        {
            uint32_t index = ip->operand;
            Value value = std::move(stack.back());

            stack.pop_back();
            globals[index] = value;

            ++ip;
            goto NEXT_OP;
        }

    OP_WRITE_ATTR:
        {
            Value value = std::move(stack.back());
            stack.pop_back();

            auto instance = std::move(stack.back()).toInstance();
            stack.pop_back();

            uint32_t index = ip->operand;

            instance->attributes->updateVar(index, value);

            ++ip;
            goto NEXT_OP;
        }

    OP_READ_LOCAL:
        {
            stack.push_back(stack[frames.back().localsBase + ip->operand]);

            ++ip;
            goto NEXT_OP;
        }

    OP_WRITE_LOCAL:
        {
            stack[frames.back().localsBase + ip->operand] = std::move(stack.back());
            stack.pop_back();

            ++ip;
            goto NEXT_OP;
        }

    OP_JUMP_IF_FALSE:
        {
            Value condition = std::move(stack.back());
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

            goto NEXT_OP;
        }

    OP_JUMP:
        {
            // Back to the beginning of the loop
            ip = begin + ip->operand;
            goto NEXT_OP;
        }

    OP_CREATE_INSTANCE:
        {
            auto klass = std::move(stack.back()).asClass();
            stack.pop_back();

            stack.push_back(Value(std::make_shared<Instance>(klass)));

            ++ip;
            goto NEXT_OP;
        }

    OP_CALL:
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

            const Chunk *compiledBody = userMethod->definition->compiledBody.get();
            if (compiledBody == nullptr)
                throw std::runtime_error("VM CALL: method body not compiled: " + userMethod->name);

            // The object and arguments are already on the evaluation stack:
            //
            //  bottom
            //      ↓
            //  ┌─────────┐
            //  │ object  │  ← $self  (slot 0, at `base`)
            //  ├─────────┤
            //  │ arg1    │
            //  ├─────────┤
            //  │ arg2    │
            //  ├─────────┤
            //  │ arg3    │  ← stack.back()
            //  └─────────┘
            //      ↑
            //     top
            const Chunk &body = *compiledBody;

            // The frame's locals region starts at the object, and its slots
            // stay at the bottom of the evaluation stack for the frame's
            // whole lifetime. Reserve room for created vars beyond the params.
            const std::size_t base = stack.size() - (paramsCount + 1);
            stack.resize(base + body.localCount);

            // The caller resumes right after this CALL; remember it, then
            // start executing the callee's chunk.
            frames.back().ip = ip;
            currentChunk = &body;
            begin = currentChunk->instructions.data();
            ip = begin;
            frames.push_back(CallFrame{&body, begin, base});
            goto NEXT_OP;
        }

    OP_RETURN:
        {
            if (frames.size() == 1)
            {
                // The program itself is returning, so finish execution.
                return;
            }

            // A method is returning: hand the value back to the caller.
            Value result = std::move(stack.back());

            // Discard the returning frame's locals and temporaries,
            // restoring the stack to the caller's pre-call height.
            const std::size_t base = frames.back().localsBase;
            stack.resize(base);

            frames.pop_back();

            // Resume the caller right after the CALL that started this method.
            ++frames.back().ip;

            currentChunk = frames.back().chunk;
            begin = currentChunk->instructions.data();
            ip = frames.back().ip;

            stack.push_back(std::move(result));
            goto NEXT_OP;
        }

    OP_DEFAULT:
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

    // Re-dispatch on the (possibly updated) instruction pointer.
    NEXT_OP:
        goto *dispatchTable[static_cast<uint8_t>(ip->opcode)];
}
