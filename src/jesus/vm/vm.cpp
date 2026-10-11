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

    // Cached copy of frames.back().localsBase. Reloading it through the
    // frame vector on every local access was pure overhead; the value only
    // changes on CALL/RETURN, which refresh this cache.
    std::size_t currentLocalsBase = 0;

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
        &&OP_ADD_LOCAL_LOCAL,             // ADD_LOCAL_LOCAL
        &&OP_SUBTRACT_LOCAL_LOCAL,        // SUBTRACT_LOCAL_LOCAL
        &&OP_MULTIPLY_LOCAL_LOCAL,        // MULTIPLY_LOCAL_LOCAL
        &&OP_DIVIDE_LOCAL_LOCAL,          // DIVIDE_LOCAL_LOCAL
        &&OP_MODULO_LOCAL_LOCAL,          // MODULO_LOCAL_LOCAL
        &&OP_EQUAL_LOCAL_LOCAL,           // EQUAL_LOCAL_LOCAL
        &&OP_NOT_EQUAL_LOCAL_LOCAL,       // NOT_EQUAL_LOCAL_LOCAL
        &&OP_LESS_LOCAL_LOCAL,            // LESS_LOCAL_LOCAL
        &&OP_LESS_EQUAL_LOCAL_LOCAL,      // LESS_EQUAL_LOCAL_LOCAL
        &&OP_GREATER_LOCAL_LOCAL,         // GREATER_LOCAL_LOCAL
        &&OP_GREATER_EQUAL_LOCAL_LOCAL,   // GREATER_EQUAL_LOCAL_LOCAL
        &&OP_ADD_LOCAL_LOCAL_STORE,       // ADD_LOCAL_LOCAL_STORE
        &&OP_SUBTRACT_LOCAL_LOCAL_STORE,  // SUBTRACT_LOCAL_LOCAL_STORE
        &&OP_MULTIPLY_LOCAL_LOCAL_STORE,  // MULTIPLY_LOCAL_LOCAL_STORE
        &&OP_DIVIDE_LOCAL_LOCAL_STORE,    // DIVIDE_LOCAL_LOCAL_STORE
        &&OP_MODULO_LOCAL_LOCAL_STORE,    // MODULO_LOCAL_LOCAL_STORE
        &&OP_ADD_LOCAL_LITERAL,           // ADD_LOCAL_LITERAL
        &&OP_SUBTRACT_LOCAL_LITERAL,      // SUBTRACT_LOCAL_LITERAL
        &&OP_MULTIPLY_LOCAL_LITERAL,      // MULTIPLY_LOCAL_LITERAL
        &&OP_DIVIDE_LOCAL_LITERAL,        // DIVIDE_LOCAL_LITERAL
        &&OP_MODULO_LOCAL_LITERAL,        // MODULO_LOCAL_LITERAL
        &&OP_EQUAL_LOCAL_LITERAL,         // EQUAL_LOCAL_LITERAL
        &&OP_NOT_EQUAL_LOCAL_LITERAL,     // NOT_EQUAL_LOCAL_LITERAL
        &&OP_LESS_LOCAL_LITERAL,          // LESS_LOCAL_LITERAL
        &&OP_LESS_EQUAL_LOCAL_LITERAL,    // LESS_EQUAL_LOCAL_LITERAL
        &&OP_GREATER_LOCAL_LITERAL,       // GREATER_LOCAL_LITERAL
        &&OP_GREATER_EQUAL_LOCAL_LITERAL, // GREATER_EQUAL_LOCAL_LITERAL
        &&OP_ADD_LITERAL_LOCAL,           // ADD_LITERAL_LOCAL
        &&OP_SUBTRACT_LITERAL_LOCAL,      // SUBTRACT_LITERAL_LOCAL
        &&OP_MULTIPLY_LITERAL_LOCAL,      // MULTIPLY_LITERAL_LOCAL
        &&OP_DIVIDE_LITERAL_LOCAL,        // DIVIDE_LITERAL_LOCAL
        &&OP_MODULO_LITERAL_LOCAL,        // MODULO_LITERAL_LOCAL
        &&OP_EQUAL_LITERAL_LOCAL,         // EQUAL_LITERAL_LOCAL
        &&OP_NOT_EQUAL_LITERAL_LOCAL,     // NOT_EQUAL_LITERAL_LOCAL
        &&OP_LESS_LITERAL_LOCAL,          // LESS_LITERAL_LOCAL
        &&OP_LESS_EQUAL_LITERAL_LOCAL,    // LESS_EQUAL_LITERAL_LOCAL
        &&OP_GREATER_LITERAL_LOCAL,       // GREATER_LITERAL_LOCAL
        &&OP_GREATER_EQUAL_LITERAL_LOCAL, // GREATER_EQUAL_LITERAL_LOCAL
        &&OP_ADD_LOCAL_LITERAL_STORE,     // ADD_LOCAL_LITERAL_STORE
        &&OP_SUBTRACT_LOCAL_LITERAL_STORE,// SUBTRACT_LOCAL_LITERAL_STORE
        &&OP_MULTIPLY_LOCAL_LITERAL_STORE,// MULTIPLY_LOCAL_LITERAL_STORE
        &&OP_DIVIDE_LOCAL_LITERAL_STORE,  // DIVIDE_LOCAL_LITERAL_STORE
        &&OP_MODULO_LOCAL_LITERAL_STORE,  // MODULO_LOCAL_LITERAL_STORE
        &&OP_ADD_LOCAL_RIGHT,             // ADD_LOCAL_RIGHT
        &&OP_SUBTRACT_LOCAL_RIGHT,        // SUBTRACT_LOCAL_RIGHT
        &&OP_MULTIPLY_LOCAL_RIGHT,        // MULTIPLY_LOCAL_RIGHT
        &&OP_DIVIDE_LOCAL_RIGHT,          // DIVIDE_LOCAL_RIGHT
        &&OP_MODULO_LOCAL_RIGHT,          // MODULO_LOCAL_RIGHT
        &&OP_EQUAL_LOCAL_RIGHT,           // EQUAL_LOCAL_RIGHT
        &&OP_NOT_EQUAL_LOCAL_RIGHT,       // NOT_EQUAL_LOCAL_RIGHT
        &&OP_LESS_LOCAL_RIGHT,            // LESS_LOCAL_RIGHT
        &&OP_LESS_EQUAL_LOCAL_RIGHT,      // LESS_EQUAL_LOCAL_RIGHT
        &&OP_GREATER_LOCAL_RIGHT,         // GREATER_LOCAL_RIGHT
        &&OP_GREATER_EQUAL_LOCAL_RIGHT,   // GREATER_EQUAL_LOCAL_RIGHT
        &&OP_ADD_LITERAL_RIGHT,           // ADD_LITERAL_RIGHT
        &&OP_SUBTRACT_LITERAL_RIGHT,      // SUBTRACT_LITERAL_RIGHT
        &&OP_MULTIPLY_LITERAL_RIGHT,      // MULTIPLY_LITERAL_RIGHT
        &&OP_DIVIDE_LITERAL_RIGHT,        // DIVIDE_LITERAL_RIGHT
        &&OP_MODULO_LITERAL_RIGHT,        // MODULO_LITERAL_RIGHT
        &&OP_EQUAL_LITERAL_RIGHT,         // EQUAL_LITERAL_RIGHT
        &&OP_NOT_EQUAL_LITERAL_RIGHT,     // NOT_EQUAL_LITERAL_RIGHT
        &&OP_LESS_LITERAL_RIGHT,          // LESS_LITERAL_RIGHT
        &&OP_LESS_EQUAL_LITERAL_RIGHT,    // LESS_EQUAL_LITERAL_RIGHT
        &&OP_GREATER_LITERAL_RIGHT,       // GREATER_LITERAL_RIGHT
        &&OP_GREATER_EQUAL_LITERAL_RIGHT, // GREATER_EQUAL_LITERAL_RIGHT
        &&OP_JUMP_IF_NOT_EQUAL_LOCAL_LOCAL,           // JUMP_IF_NOT_EQUAL_LOCAL_LOCAL
        &&OP_JUMP_IF_NOT_NOT_EQUAL_LOCAL_LOCAL,       // JUMP_IF_NOT_NOT_EQUAL_LOCAL_LOCAL
        &&OP_JUMP_IF_NOT_LESS_LOCAL_LOCAL,            // JUMP_IF_NOT_LESS_LOCAL_LOCAL
        &&OP_JUMP_IF_NOT_LESS_EQUAL_LOCAL_LOCAL,      // JUMP_IF_NOT_LESS_EQUAL_LOCAL_LOCAL
        &&OP_JUMP_IF_NOT_GREATER_LOCAL_LOCAL,         // JUMP_IF_NOT_GREATER_LOCAL_LOCAL
        &&OP_JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL,   // JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL
        &&OP_JUMP_IF_NOT_EQUAL_LOCAL_LITERAL,         // JUMP_IF_NOT_EQUAL_LOCAL_LITERAL
        &&OP_JUMP_IF_NOT_NOT_EQUAL_LOCAL_LITERAL,     // JUMP_IF_NOT_NOT_EQUAL_LOCAL_LITERAL
        &&OP_JUMP_IF_NOT_LESS_LOCAL_LITERAL,          // JUMP_IF_NOT_LESS_LOCAL_LITERAL
        &&OP_JUMP_IF_NOT_LESS_EQUAL_LOCAL_LITERAL,    // JUMP_IF_NOT_LESS_EQUAL_LOCAL_LITERAL
        &&OP_JUMP_IF_NOT_GREATER_LOCAL_LITERAL,       // JUMP_IF_NOT_GREATER_LOCAL_LITERAL
        &&OP_JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL, // JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL
        &&OP_JUMP_IF_NOT_EQUAL_LITERAL_LOCAL,         // JUMP_IF_NOT_EQUAL_LITERAL_LOCAL
        &&OP_JUMP_IF_NOT_NOT_EQUAL_LITERAL_LOCAL,     // JUMP_IF_NOT_NOT_EQUAL_LITERAL_LOCAL
        &&OP_JUMP_IF_NOT_LESS_LITERAL_LOCAL,          // JUMP_IF_NOT_LESS_LITERAL_LOCAL
        &&OP_JUMP_IF_NOT_LESS_EQUAL_LITERAL_LOCAL,    // JUMP_IF_NOT_LESS_EQUAL_LITERAL_LOCAL
        &&OP_JUMP_IF_NOT_GREATER_LITERAL_LOCAL,       // JUMP_IF_NOT_GREATER_LITERAL_LOCAL
        &&OP_JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL, // JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL
        &&OP_JUMP_IF_NOT_EQUAL_LOCAL_RIGHT,           // JUMP_IF_NOT_EQUAL_LOCAL_RIGHT
        &&OP_JUMP_IF_NOT_NOT_EQUAL_LOCAL_RIGHT,       // JUMP_IF_NOT_NOT_EQUAL_LOCAL_RIGHT
        &&OP_JUMP_IF_NOT_LESS_LOCAL_RIGHT,            // JUMP_IF_NOT_LESS_LOCAL_RIGHT
        &&OP_JUMP_IF_NOT_LESS_EQUAL_LOCAL_RIGHT,      // JUMP_IF_NOT_LESS_EQUAL_LOCAL_RIGHT
        &&OP_JUMP_IF_NOT_GREATER_LOCAL_RIGHT,         // JUMP_IF_NOT_GREATER_LOCAL_RIGHT
        &&OP_JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT,   // JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT
        &&OP_JUMP_IF_NOT_EQUAL_LITERAL_RIGHT,         // JUMP_IF_NOT_EQUAL_LITERAL_RIGHT
        &&OP_JUMP_IF_NOT_NOT_EQUAL_LITERAL_RIGHT,     // JUMP_IF_NOT_NOT_EQUAL_LITERAL_RIGHT
        &&OP_JUMP_IF_NOT_LESS_LITERAL_RIGHT,          // JUMP_IF_NOT_LESS_LITERAL_RIGHT
        &&OP_JUMP_IF_NOT_LESS_EQUAL_LITERAL_RIGHT,    // JUMP_IF_NOT_LESS_EQUAL_LITERAL_RIGHT
        &&OP_JUMP_IF_NOT_GREATER_LITERAL_RIGHT,       // JUMP_IF_NOT_GREATER_LITERAL_RIGHT
        &&OP_JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT, // JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT
        &&OP_ADD_LOCAL_RIGHT_STORE,                   // ADD_LOCAL_RIGHT_STORE
        &&OP_SUBTRACT_LOCAL_RIGHT_STORE,              // SUBTRACT_LOCAL_RIGHT_STORE
        &&OP_MULTIPLY_LOCAL_RIGHT_STORE,              // MULTIPLY_LOCAL_RIGHT_STORE
        &&OP_DIVIDE_LOCAL_RIGHT_STORE,                // DIVIDE_LOCAL_RIGHT_STORE
        &&OP_MODULO_LOCAL_RIGHT_STORE,                // MODULO_LOCAL_RIGHT_STORE
        &&OP_ADD_LITERAL_RIGHT_STORE,                 // ADD_LITERAL_RIGHT_STORE
        &&OP_SUBTRACT_LITERAL_RIGHT_STORE,            // SUBTRACT_LITERAL_RIGHT_STORE
        &&OP_MULTIPLY_LITERAL_RIGHT_STORE,            // MULTIPLY_LITERAL_RIGHT_STORE
        &&OP_DIVIDE_LITERAL_RIGHT_STORE,              // DIVIDE_LITERAL_RIGHT_STORE
        &&OP_MODULO_LITERAL_RIGHT_STORE,              // MODULO_LITERAL_RIGHT_STORE
        &&OP_JUMP_IF_NOT_ADD_LOCAL_LOCAL_EQUAL_LITERAL,         // JUMP_IF_NOT_ADD_LOCAL_LOCAL_EQUAL_LITERAL
        &&OP_JUMP_IF_NOT_ADD_LOCAL_LOCAL_NOT_EQUAL_LITERAL,     // JUMP_IF_NOT_ADD_LOCAL_LOCAL_NOT_EQUAL_LITERAL
        &&OP_JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_LITERAL,          // JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_LITERAL
        &&OP_JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_EQUAL_LITERAL,    // JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_EQUAL_LITERAL
        &&OP_JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL,       // JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL
        &&OP_JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_EQUAL_LITERAL, // JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_EQUAL_LITERAL
        &&OP_MULTIPLY_ADD_LOCAL_RIGHT_STORE,                    // MULTIPLY_ADD_LOCAL_RIGHT_STORE
        &&OP_ADD_LOCAL_LITERAL_STORE_JUMP,                      // ADD_LOCAL_LITERAL_STORE_JUMP
        &&OP_SUBTRACT_ADD_LOCAL_LOCAL_LOCAL_STORE               // SUBTRACT_ADD_LOCAL_LOCAL_LOCAL_STORE
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
    // Fused LOCAL_LOCAL handlers.
    //
    // The operand packs the two source local slots: `a` in the low 16 bits
    // and `b` in the high 16 bits. The result is pushed directly onto the
    // stack, so the two operand reads, pushes and the following pop
    // disappear. Non-numeric operands fall back to the regular operator
    // overloads (which handle strings, booleans and so on).
    // -----------------------------------------------------------------
#define FUSED_OPERANDS \
    const uint32_t packed = ip->operand; \
    const std::size_t lb = currentLocalsBase; \
    const Value &a = stack[lb + (packed & 0xFFFF)]; \
    const Value &b = stack[lb + (packed >> 16)];

#define FUSED_ARITH_LOCAL_LOCAL(label, op) \
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

    FUSED_ARITH_LOCAL_LOCAL(ADD_LOCAL_LOCAL, +)
    FUSED_ARITH_LOCAL_LOCAL(SUBTRACT_LOCAL_LOCAL, -)
    FUSED_ARITH_LOCAL_LOCAL(MULTIPLY_LOCAL_LOCAL, *)

    OP_DIVIDE_LOCAL_LOCAL:
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

    OP_MODULO_LOCAL_LOCAL:
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

#define FUSED_CMP_LOCAL_LOCAL(label, op) \
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

    FUSED_CMP_LOCAL_LOCAL(EQUAL_LOCAL_LOCAL, ==)
    FUSED_CMP_LOCAL_LOCAL(NOT_EQUAL_LOCAL_LOCAL, !=)
    FUSED_CMP_LOCAL_LOCAL(LESS_LOCAL_LOCAL, <)
    FUSED_CMP_LOCAL_LOCAL(LESS_EQUAL_LOCAL_LOCAL, <=)
    FUSED_CMP_LOCAL_LOCAL(GREATER_LOCAL_LOCAL, >)
    FUSED_CMP_LOCAL_LOCAL(GREATER_EQUAL_LOCAL_LOCAL, >=)

    // -----------------------------------------------------------------
    // Fused LOCAL_LITERAL / LITERAL_LOCAL handlers.
    //
    // These cover binary operations where one operand is a local and the
    // other is a literal constant, e.g. `i < 100` or `2.0 * x`. The
    // operand packs the local slot in the low 16 bits and the constant
    // index in the high 16 bits; the opcode name fixes which side is which.
    // -----------------------------------------------------------------
#define FUSED_LOCAL_LITERAL_OPERANDS \
    const uint32_t packed = ip->operand; \
    Value *const lb = stack.data() + currentLocalsBase; \
    const Value &a = lb[packed & 0xFFFF]; \
    const Value &b = currentChunk->literals[packed >> 16];

#define FUSED_LITERAL_LOCAL_OPERANDS \
    const uint32_t packed = ip->operand; \
    const Value &a = currentChunk->literals[packed >> 16]; \
    Value *const lb = stack.data() + currentLocalsBase; \
    const Value &b = lb[packed & 0xFFFF];

#define FUSED_ARITH_OP(label, operands, op) \
    OP_##label: \
    { \
        operands \
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

#define FUSED_CMP_OP(label, operands, op) \
    OP_##label: \
    { \
        operands \
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

    FUSED_ARITH_OP(ADD_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, +)
    FUSED_ARITH_OP(SUBTRACT_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, -)
    FUSED_ARITH_OP(MULTIPLY_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, *)

    OP_DIVIDE_LOCAL_LITERAL:
        {
            FUSED_LOCAL_LITERAL_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                stack.emplace_back();
                stack.back().vmSetAsDouble(a.number / b.number);
            }
            else
            {
                stack.push_back(a / b);
            }
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LOCAL_LITERAL:
        {
            FUSED_LOCAL_LITERAL_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (a.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(a.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                stack.emplace_back();
                stack.back().vmSetAsInt(a.intValue % b.intValue);
            }
            else
            {
                stack.push_back(a % b);
            }
            ++ip;
            goto NEXT_OP;
        }

    FUSED_CMP_OP(EQUAL_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, ==)
    FUSED_CMP_OP(NOT_EQUAL_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, !=)
    FUSED_CMP_OP(LESS_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, <)
    FUSED_CMP_OP(LESS_EQUAL_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, <=)
    FUSED_CMP_OP(GREATER_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, >)
    FUSED_CMP_OP(GREATER_EQUAL_LOCAL_LITERAL, FUSED_LOCAL_LITERAL_OPERANDS, >=)

    FUSED_ARITH_OP(ADD_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, +)
    FUSED_ARITH_OP(SUBTRACT_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, -)
    FUSED_ARITH_OP(MULTIPLY_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, *)

    OP_DIVIDE_LITERAL_LOCAL:
        {
            FUSED_LITERAL_LOCAL_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                stack.emplace_back();
                stack.back().vmSetAsDouble(a.number / b.number);
            }
            else
            {
                stack.push_back(a / b);
            }
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LITERAL_LOCAL:
        {
            FUSED_LITERAL_LOCAL_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (a.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(a.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                stack.emplace_back();
                stack.back().vmSetAsInt(a.intValue % b.intValue);
            }
            else
            {
                stack.push_back(a % b);
            }
            ++ip;
            goto NEXT_OP;
        }

    FUSED_CMP_OP(EQUAL_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, ==)
    FUSED_CMP_OP(NOT_EQUAL_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, !=)
    FUSED_CMP_OP(LESS_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, <)
    FUSED_CMP_OP(LESS_EQUAL_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, <=)
    FUSED_CMP_OP(GREATER_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, >)
    FUSED_CMP_OP(GREATER_EQUAL_LITERAL_LOCAL, FUSED_LITERAL_LOCAL_OPERANDS, >=)

    // -----------------------------------------------------------------
    // Fused LOCAL_LOCAL_STORE handlers.
    //
    // These handle patterns like `x2 = x * x`, where a two-local arithmetic
    // operation is immediately written to a local. The operand packs three
    // 8-bit slots: `a`, `b` and the destination. Writing the result straight
    // into the destination removes all operand-stack traffic. The public
    // setters are used (not the VM-only ones) because the destination may
    // already hold a heap-backed value that has to be released.
    // -----------------------------------------------------------------
#define FUSED_STORE_OPERANDS \
    const uint32_t packed = ip->operand; \
    Value *const lb = stack.data() + currentLocalsBase; \
    const Value &a = lb[packed & 0xFF]; \
    const Value &b = lb[(packed >> 8) & 0xFF]; \
    Value &dest = lb[packed >> 16];

#define FUSED_ARITH_LOCAL_LOCAL_STORE(label, op) \
    OP_##label: \
    { \
        FUSED_STORE_OPERANDS \
        if (a.IS_NUMBER && b.IS_NUMBER) \
        { \
            if (a.IS_DOUBLE || b.IS_DOUBLE) \
                dest.setAsDouble(a.number op b.number); \
            else \
                dest.setAsInt(a.intValue op b.intValue); \
        } \
        else \
        { \
            dest = a op b; \
        } \
        ++ip; \
        goto NEXT_OP; \
    }

    FUSED_ARITH_LOCAL_LOCAL_STORE(ADD_LOCAL_LOCAL_STORE, +)
    FUSED_ARITH_LOCAL_LOCAL_STORE(SUBTRACT_LOCAL_LOCAL_STORE, -)
    FUSED_ARITH_LOCAL_LOCAL_STORE(MULTIPLY_LOCAL_LOCAL_STORE, *)

    OP_DIVIDE_LOCAL_LOCAL_STORE:
        {
            FUSED_STORE_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                dest.setAsDouble(a.number / b.number);
            }
            else
            {
                dest = a / b;
            }
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LOCAL_LOCAL_STORE:
        {
            FUSED_STORE_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (a.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(a.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                dest.setAsInt(a.intValue % b.intValue);
            }
            else
            {
                dest = a % b;
            }
            ++ip;
            goto NEXT_OP;
        }

    // -----------------------------------------------------------------
    // Fused LOCAL_LITERAL_STORE handlers, for patterns like `i = i + 1`.
    // The operand packs the local slot (bits 0-7), the constant index
    // (bits 8-23) and the destination slot (bits 24-31).
    // -----------------------------------------------------------------
#define FUSED_LOCAL_LITERAL_STORE_OPERANDS \
    const uint32_t packed = ip->operand; \
    Value *const lb = stack.data() + currentLocalsBase; \
    const Value &a = lb[packed & 0xFF]; \
    const Value &b = currentChunk->literals[(packed >> 8) & 0xFFFF]; \
    Value &dest = lb[packed >> 24];

#define FUSED_ARITH_STORE_OP(label, operands, op) \
    OP_##label: \
    { \
        operands \
        if (a.IS_NUMBER && b.IS_NUMBER) \
        { \
            if (a.IS_DOUBLE || b.IS_DOUBLE) \
                dest.setAsDouble(a.number op b.number); \
            else \
                dest.setAsInt(a.intValue op b.intValue); \
        } \
        else \
        { \
            dest = a op b; \
        } \
        ++ip; \
        goto NEXT_OP; \
    }

    FUSED_ARITH_STORE_OP(ADD_LOCAL_LITERAL_STORE, FUSED_LOCAL_LITERAL_STORE_OPERANDS, +)
    FUSED_ARITH_STORE_OP(SUBTRACT_LOCAL_LITERAL_STORE, FUSED_LOCAL_LITERAL_STORE_OPERANDS, -)
    FUSED_ARITH_STORE_OP(MULTIPLY_LOCAL_LITERAL_STORE, FUSED_LOCAL_LITERAL_STORE_OPERANDS, *)

    OP_DIVIDE_LOCAL_LITERAL_STORE:
        {
            FUSED_LOCAL_LITERAL_STORE_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                dest.setAsDouble(a.number / b.number);
            }
            else
            {
                dest = a / b;
            }
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LOCAL_LITERAL_STORE:
        {
            FUSED_LOCAL_LITERAL_STORE_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (a.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(a.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                dest.setAsInt(a.intValue % b.intValue);
            }
            else
            {
                dest = a % b;
            }
            ++ip;
            goto NEXT_OP;
        }

    // -----------------------------------------------------------------
    // Fused LOCAL_RIGHT / LITERAL_RIGHT handlers.
    //
    // Here the LEFT operand is already on top of the stack (produced by the
    // preceding instruction) and only the RIGHT operand is fetched, from a
    // local or a constant. The result overwrites the left operand in place,
    // so the stack depth is unchanged and no extra Value is constructed.
    // The in-place setters are safe because the left slot was just checked
    // to hold a number.
    // -----------------------------------------------------------------
#define FUSED_LOCAL_RIGHT_OPERANDS \
    Value &left = stack.back(); \
    Value *const lb = stack.data() + currentLocalsBase; \
    const Value &b = lb[ip->operand & 0xFFFF];

#define FUSED_LITERAL_RIGHT_OPERANDS \
    Value &left = stack.back(); \
    const Value &b = currentChunk->literals[ip->operand & 0xFFFF];

#define FUSED_ARITH_RIGHT_OP(label, operands, op) \
    OP_##label: \
    { \
        operands \
        if (left.IS_NUMBER && b.IS_NUMBER) \
        { \
            if (left.IS_DOUBLE || b.IS_DOUBLE) \
                left.vmSetAsDouble(left.number op b.number); \
            else \
                left.vmSetAsInt(left.intValue op b.intValue); \
        } \
        else \
        { \
            left = left op b; \
        } \
        ++ip; \
        goto NEXT_OP; \
    }

#define FUSED_CMP_RIGHT_OP(label, operands, op) \
    OP_##label: \
    { \
        operands \
        if (left.IS_NUMBER && b.IS_NUMBER) \
            left.vmSetAsBoolean(left.number op b.number); \
        else \
            left = Value(left op b); \
        ++ip; \
        goto NEXT_OP; \
    }

    FUSED_ARITH_RIGHT_OP(ADD_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, +)
    FUSED_ARITH_RIGHT_OP(SUBTRACT_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, -)
    FUSED_ARITH_RIGHT_OP(MULTIPLY_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, *)

    OP_DIVIDE_LOCAL_RIGHT:
        {
            FUSED_LOCAL_RIGHT_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                left.vmSetAsDouble(left.number / b.number);
            }
            else
            {
                left = left / b;
            }
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LOCAL_RIGHT:
        {
            FUSED_LOCAL_RIGHT_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (left.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(left.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                left.vmSetAsInt(left.intValue % b.intValue);
            }
            else
            {
                left = left % b;
            }
            ++ip;
            goto NEXT_OP;
        }

    FUSED_CMP_RIGHT_OP(EQUAL_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, ==)
    FUSED_CMP_RIGHT_OP(NOT_EQUAL_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, !=)
    FUSED_CMP_RIGHT_OP(LESS_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, <)
    FUSED_CMP_RIGHT_OP(LESS_EQUAL_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, <=)
    FUSED_CMP_RIGHT_OP(GREATER_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, >)
    FUSED_CMP_RIGHT_OP(GREATER_EQUAL_LOCAL_RIGHT, FUSED_LOCAL_RIGHT_OPERANDS, >=)

    FUSED_ARITH_RIGHT_OP(ADD_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, +)
    FUSED_ARITH_RIGHT_OP(SUBTRACT_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, -)
    FUSED_ARITH_RIGHT_OP(MULTIPLY_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, *)

    OP_DIVIDE_LITERAL_RIGHT:
        {
            FUSED_LITERAL_RIGHT_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                left.vmSetAsDouble(left.number / b.number);
            }
            else
            {
                left = left / b;
            }
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LITERAL_RIGHT:
        {
            FUSED_LITERAL_RIGHT_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (left.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(left.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                left.vmSetAsInt(left.intValue % b.intValue);
            }
            else
            {
                left = left % b;
            }
            ++ip;
            goto NEXT_OP;
        }

    FUSED_CMP_RIGHT_OP(EQUAL_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, ==)
    FUSED_CMP_RIGHT_OP(NOT_EQUAL_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, !=)
    FUSED_CMP_RIGHT_OP(LESS_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, <)
    FUSED_CMP_RIGHT_OP(LESS_EQUAL_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, <=)
    FUSED_CMP_RIGHT_OP(GREATER_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, >)
    FUSED_CMP_RIGHT_OP(GREATER_EQUAL_LITERAL_RIGHT, FUSED_LITERAL_RIGHT_OPERANDS, >=)

    // -----------------------------------------------------------------
    // Comparison-and-branch handlers.
    //
    // A comparison whose only consumer is a conditional jump does not need
    // to materialise a boolean on the stack. The operand packs the same
    // operands as the matching comparison opcode and the jump target is the
    // operand of the instruction immediately after this one. When the
    // comparison is false control jumps to the target, otherwise execution
    // skips the target word. The *_RIGHT forms also drop the left operand
    // they consumed from the stack.
    // -----------------------------------------------------------------
#define BRANCH_IF_NOT(cond) \
    if (!(cond)) \
        ip = begin + (ip + 1)->operand; \
    else \
        ip += 2; \
    goto NEXT_OP;

#define CMP_IS_TRUE(a, b, cmpop) \
    (((a).IS_NUMBER && (b).IS_NUMBER) ? ((a).number cmpop (b).number) : Value((a) cmpop (b)).truthy())

#define BRANCH_LOCAL_LOCAL(label, cmpop) \
    OP_##label: \
    { \
        const Value &a = stack[currentLocalsBase + (ip->operand & 0xFFFF)]; \
        const Value &b = stack[currentLocalsBase + ((ip->operand >> 16) & 0xFFFF)]; \
        BRANCH_IF_NOT(CMP_IS_TRUE(a, b, cmpop)) \
    }

#define BRANCH_LOCAL_LITERAL(label, cmpop) \
    OP_##label: \
    { \
        const Value &a = stack[currentLocalsBase + (ip->operand & 0xFFFF)]; \
        const Value &b = currentChunk->literals[(ip->operand >> 16) & 0xFFFF]; \
        BRANCH_IF_NOT(CMP_IS_TRUE(a, b, cmpop)) \
    }

#define BRANCH_LITERAL_LOCAL(label, cmpop) \
    OP_##label: \
    { \
        const Value &a = currentChunk->literals[(ip->operand >> 16) & 0xFFFF]; \
        const Value &b = stack[currentLocalsBase + (ip->operand & 0xFFFF)]; \
        BRANCH_IF_NOT(CMP_IS_TRUE(a, b, cmpop)) \
    }

#define BRANCH_LOCAL_RIGHT(label, cmpop) \
    OP_##label: \
    { \
        const Value &a = stack.back(); \
        const Value &b = stack[currentLocalsBase + (ip->operand & 0xFFFF)]; \
        const bool cond = CMP_IS_TRUE(a, b, cmpop); \
        stack.pop_back(); \
        BRANCH_IF_NOT(cond) \
    }

#define BRANCH_LITERAL_RIGHT(label, cmpop) \
    OP_##label: \
    { \
        const Value &a = stack.back(); \
        const Value &b = currentChunk->literals[ip->operand & 0xFFFF]; \
        const bool cond = CMP_IS_TRUE(a, b, cmpop); \
        stack.pop_back(); \
        BRANCH_IF_NOT(cond) \
    }

    BRANCH_LOCAL_LOCAL(JUMP_IF_NOT_EQUAL_LOCAL_LOCAL, ==)
    BRANCH_LOCAL_LOCAL(JUMP_IF_NOT_NOT_EQUAL_LOCAL_LOCAL, !=)
    BRANCH_LOCAL_LOCAL(JUMP_IF_NOT_LESS_LOCAL_LOCAL, <)
    BRANCH_LOCAL_LOCAL(JUMP_IF_NOT_LESS_EQUAL_LOCAL_LOCAL, <=)
    BRANCH_LOCAL_LOCAL(JUMP_IF_NOT_GREATER_LOCAL_LOCAL, >)
    BRANCH_LOCAL_LOCAL(JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LOCAL, >=)

    BRANCH_LOCAL_LITERAL(JUMP_IF_NOT_EQUAL_LOCAL_LITERAL, ==)
    BRANCH_LOCAL_LITERAL(JUMP_IF_NOT_NOT_EQUAL_LOCAL_LITERAL, !=)
    BRANCH_LOCAL_LITERAL(JUMP_IF_NOT_LESS_LOCAL_LITERAL, <)
    BRANCH_LOCAL_LITERAL(JUMP_IF_NOT_LESS_EQUAL_LOCAL_LITERAL, <=)
    BRANCH_LOCAL_LITERAL(JUMP_IF_NOT_GREATER_LOCAL_LITERAL, >)
    BRANCH_LOCAL_LITERAL(JUMP_IF_NOT_GREATER_EQUAL_LOCAL_LITERAL, >=)

    BRANCH_LITERAL_LOCAL(JUMP_IF_NOT_EQUAL_LITERAL_LOCAL, ==)
    BRANCH_LITERAL_LOCAL(JUMP_IF_NOT_NOT_EQUAL_LITERAL_LOCAL, !=)
    BRANCH_LITERAL_LOCAL(JUMP_IF_NOT_LESS_LITERAL_LOCAL, <)
    BRANCH_LITERAL_LOCAL(JUMP_IF_NOT_LESS_EQUAL_LITERAL_LOCAL, <=)
    BRANCH_LITERAL_LOCAL(JUMP_IF_NOT_GREATER_LITERAL_LOCAL, >)
    BRANCH_LITERAL_LOCAL(JUMP_IF_NOT_GREATER_EQUAL_LITERAL_LOCAL, >=)

    BRANCH_LOCAL_RIGHT(JUMP_IF_NOT_EQUAL_LOCAL_RIGHT, ==)
    BRANCH_LOCAL_RIGHT(JUMP_IF_NOT_NOT_EQUAL_LOCAL_RIGHT, !=)
    BRANCH_LOCAL_RIGHT(JUMP_IF_NOT_LESS_LOCAL_RIGHT, <)
    BRANCH_LOCAL_RIGHT(JUMP_IF_NOT_LESS_EQUAL_LOCAL_RIGHT, <=)
    BRANCH_LOCAL_RIGHT(JUMP_IF_NOT_GREATER_LOCAL_RIGHT, >)
    BRANCH_LOCAL_RIGHT(JUMP_IF_NOT_GREATER_EQUAL_LOCAL_RIGHT, >=)

    BRANCH_LITERAL_RIGHT(JUMP_IF_NOT_EQUAL_LITERAL_RIGHT, ==)
    BRANCH_LITERAL_RIGHT(JUMP_IF_NOT_NOT_EQUAL_LITERAL_RIGHT, !=)
    BRANCH_LITERAL_RIGHT(JUMP_IF_NOT_LESS_LITERAL_RIGHT, <)
    BRANCH_LITERAL_RIGHT(JUMP_IF_NOT_LESS_EQUAL_LITERAL_RIGHT, <=)
    BRANCH_LITERAL_RIGHT(JUMP_IF_NOT_GREATER_LITERAL_RIGHT, >)
    BRANCH_LITERAL_RIGHT(JUMP_IF_NOT_GREATER_EQUAL_LITERAL_RIGHT, >=)

    // -----------------------------------------------------------------
    // Comparison-and-branch over a sum of two locals against a constant.
    //
    // Matches the divergence test `if x2 + y2 > 4.0`: the sum never reaches
    // the stack, so this replaces the ADD plus the comparison-branch with a
    // single dispatch. The comparison mirrors the two separate operations
    // exactly (integer addition wraps like vmSetAsInt, and non-numeric
    // operands fall back to Value's comparison operators).
    // -----------------------------------------------------------------
#define BRANCH_SUM_LOCAL_LOCAL_LITERAL(label, cmpop) \
    OP_##label: \
    { \
        const Value &a = stack[currentLocalsBase + (ip->operand & 0xFF)]; \
        const Value &b = stack[currentLocalsBase + ((ip->operand >> 8) & 0xFF)]; \
        const Value &lit = currentChunk->literals[(ip->operand >> 16) & 0xFFFF]; \
        bool cond; \
        if (a.IS_NUMBER && b.IS_NUMBER && lit.IS_NUMBER) \
        { \
            const double sum = (a.IS_DOUBLE || b.IS_DOUBLE) \
                                   ? a.number + b.number \
                                   : static_cast<double>(a.intValue + b.intValue); \
            cond = sum cmpop lit.number; \
        } \
        else \
        { \
            const Value sum = a + b; \
            cond = CMP_IS_TRUE(sum, lit, cmpop); \
        } \
        BRANCH_IF_NOT(cond) \
    }

    BRANCH_SUM_LOCAL_LOCAL_LITERAL(JUMP_IF_NOT_ADD_LOCAL_LOCAL_EQUAL_LITERAL, ==)
    BRANCH_SUM_LOCAL_LOCAL_LITERAL(JUMP_IF_NOT_ADD_LOCAL_LOCAL_NOT_EQUAL_LITERAL, !=)
    BRANCH_SUM_LOCAL_LOCAL_LITERAL(JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_LITERAL, <)
    BRANCH_SUM_LOCAL_LOCAL_LITERAL(JUMP_IF_NOT_ADD_LOCAL_LOCAL_LESS_EQUAL_LITERAL, <=)
    BRANCH_SUM_LOCAL_LOCAL_LITERAL(JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_LITERAL, >)
    BRANCH_SUM_LOCAL_LOCAL_LITERAL(JUMP_IF_NOT_ADD_LOCAL_LOCAL_GREATER_EQUAL_LITERAL, >=)

    // -----------------------------------------------------------------
    // Fused multiply-then-add store: `dest = (left * b) + c`.
    //
    // `left` is on top of the stack; `b`, `c` and `dest` are locals. This is
    // the `2.0 * x * y + cy` step of the Mandelbrot inner loop, where the
    // trailing multiply-in-place and add-and-store collapse into one
    // instruction. All operands are read before `dest` is written, so any
    // aliasing between them stays correct.
    // -----------------------------------------------------------------
    OP_MULTIPLY_ADD_LOCAL_RIGHT_STORE:
        {
            Value &left = stack.back();
            Value *const lb = stack.data() + currentLocalsBase;
            const Value &b = lb[ip->operand & 0xFF];
            const Value &c = lb[(ip->operand >> 8) & 0xFF];
            Value &dest = lb[(ip->operand >> 16) & 0xFF];

            if (left.IS_NUMBER && b.IS_NUMBER && c.IS_NUMBER)
            {
                if (left.IS_DOUBLE || b.IS_DOUBLE || c.IS_DOUBLE)
                    dest.setAsDouble(left.number * b.number + c.number);
                else
                    dest.setAsInt(left.intValue * b.intValue + c.intValue);
            }
            else
            {
                dest = left * b + c;
            }

            stack.pop_back();
            ++ip;
            goto NEXT_OP;
        }

    // -----------------------------------------------------------------
    // Counted-loop increment (`i = i + k`) fused with the back-edge JUMP,
    // removing one dispatch per iteration. Same operand layout as
    // ADD_LOCAL_LITERAL_STORE; the jump target is read from the operand of
    // the following instruction (the original JUMP, kept as a target slot).
    // -----------------------------------------------------------------
    OP_ADD_LOCAL_LITERAL_STORE_JUMP:
        {
            FUSED_LOCAL_LITERAL_STORE_OPERANDS
            if (a.IS_NUMBER && b.IS_NUMBER)
            {
                if (a.IS_DOUBLE || b.IS_DOUBLE)
                    dest.setAsDouble(a.number + b.number);
                else
                    dest.setAsInt(a.intValue + b.intValue);
            }
            else
            {
                dest = a + b;
            }
            ip = begin + (ip + 1)->operand;
            goto NEXT_OP;
        }

    // -----------------------------------------------------------------
    // Fused `dest = a - b + c` store. Reads a, b and c before writing dest,
    // so it also handles the case where c aliases the destination (which the
    // two chained in-place stores cannot).
    // -----------------------------------------------------------------
    OP_SUBTRACT_ADD_LOCAL_LOCAL_LOCAL_STORE:
        {
            const uint32_t packed = ip->operand;
            Value *const lb = stack.data() + currentLocalsBase;
            const Value &a = lb[packed & 0xFF];
            const Value &b = lb[(packed >> 8) & 0xFF];
            const Value &c = lb[(packed >> 16) & 0xFF];
            Value &dest = lb[(packed >> 24) & 0xFF];

            if (a.IS_NUMBER && b.IS_NUMBER && c.IS_NUMBER)
            {
                if (a.IS_DOUBLE || b.IS_DOUBLE || c.IS_DOUBLE)
                    dest.setAsDouble(a.number - b.number + c.number);
                else
                    dest.setAsInt(a.intValue - b.intValue + c.intValue);
            }
            else
            {
                dest = a - b + c;
            }

            ++ip;
            goto NEXT_OP;
        }

    // -----------------------------------------------------------------
    // Store variants of the *_RIGHT arithmetic forms.
    //
    // The left operand is on top of the stack, the right operand is a local
    // or constant in the low 16 bits of the operand and the destination
    // local slot is in the high 16 bits. The result is written straight to
    // the destination and the left operand is popped.
    // -----------------------------------------------------------------
#define FUSED_LOCAL_RIGHT_STORE_OPERANDS \
    Value &left = stack.back(); \
    Value *const lb = stack.data() + currentLocalsBase; \
    const Value &b = lb[ip->operand & 0xFFFF]; \
    Value &dest = lb[(ip->operand >> 16) & 0xFFFF];

#define FUSED_LITERAL_RIGHT_STORE_OPERANDS \
    Value &left = stack.back(); \
    const Value &b = currentChunk->literals[ip->operand & 0xFFFF]; \
    Value *const lb = stack.data() + currentLocalsBase; \
    Value &dest = lb[(ip->operand >> 16) & 0xFFFF];

#define FUSED_ARITH_RIGHT_STORE_OP(label, operands, op) \
    OP_##label: \
    { \
        operands \
        if (left.IS_NUMBER && b.IS_NUMBER) \
        { \
            if (left.IS_DOUBLE || b.IS_DOUBLE) \
                dest.setAsDouble(left.number op b.number); \
            else \
                dest.setAsInt(left.intValue op b.intValue); \
        } \
        else \
        { \
            dest = left op b; \
        } \
        stack.pop_back(); \
        ++ip; \
        goto NEXT_OP; \
    }

    FUSED_ARITH_RIGHT_STORE_OP(ADD_LOCAL_RIGHT_STORE, FUSED_LOCAL_RIGHT_STORE_OPERANDS, +)
    FUSED_ARITH_RIGHT_STORE_OP(SUBTRACT_LOCAL_RIGHT_STORE, FUSED_LOCAL_RIGHT_STORE_OPERANDS, -)
    FUSED_ARITH_RIGHT_STORE_OP(MULTIPLY_LOCAL_RIGHT_STORE, FUSED_LOCAL_RIGHT_STORE_OPERANDS, *)

    OP_DIVIDE_LOCAL_RIGHT_STORE:
        {
            FUSED_LOCAL_RIGHT_STORE_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                dest.setAsDouble(left.number / b.number);
            }
            else
            {
                dest = left / b;
            }
            stack.pop_back();
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LOCAL_RIGHT_STORE:
        {
            FUSED_LOCAL_RIGHT_STORE_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (left.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(left.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                dest.setAsInt(left.intValue % b.intValue);
            }
            else
            {
                dest = left % b;
            }
            stack.pop_back();
            ++ip;
            goto NEXT_OP;
        }

    FUSED_ARITH_RIGHT_STORE_OP(ADD_LITERAL_RIGHT_STORE, FUSED_LITERAL_RIGHT_STORE_OPERANDS, +)
    FUSED_ARITH_RIGHT_STORE_OP(SUBTRACT_LITERAL_RIGHT_STORE, FUSED_LITERAL_RIGHT_STORE_OPERANDS, -)
    FUSED_ARITH_RIGHT_STORE_OP(MULTIPLY_LITERAL_RIGHT_STORE, FUSED_LITERAL_RIGHT_STORE_OPERANDS, *)

    OP_DIVIDE_LITERAL_RIGHT_STORE:
        {
            FUSED_LITERAL_RIGHT_STORE_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (b.number == 0.0)
                    throw std::runtime_error("Division by zero");
                dest.setAsDouble(left.number / b.number);
            }
            else
            {
                dest = left / b;
            }
            stack.pop_back();
            ++ip;
            goto NEXT_OP;
        }

    OP_MODULO_LITERAL_RIGHT_STORE:
        {
            FUSED_LITERAL_RIGHT_STORE_OPERANDS
            if (left.IS_NUMBER && b.IS_NUMBER)
            {
                if (left.IS_DOUBLE || b.IS_DOUBLE)
                    throw std::runtime_error("'" + std::to_string(left.number) + " mod " + std::to_string(b.number) + "' invalid — 'mod' only supports integers.");
                dest.setAsInt(left.intValue % b.intValue);
            }
            else
            {
                dest = left % b;
            }
            stack.pop_back();
            ++ip;
            goto NEXT_OP;
        }

    OP_OR:
        {
            BINARY_OPERANDS
            Value r = std::move(right);
            stack.pop_back();
            Value l = std::move(left);
            stack.back() = (l.truthy() ? l : r);
            ++ip;
            goto NEXT_OP;
        }

    OP_AND:
        {
            BINARY_OPERANDS
            Value r = std::move(right);
            stack.pop_back();
            Value l = std::move(left);
            stack.back() = (l.truthy() ? r : l);
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
                stack.back() = (left2.truthy() ? left2 : right2);
                break;
            case OpCode::AND:
                stack.back() = (left2.truthy() ? right2 : left2);
                break;

            case OpCode::XOR:
                if (left2.IS_BOOLEAN && right2.IS_BOOLEAN)
                {
                    // Logical XOR
                    stack.back() = (Value(left2.truthy() != right2.truthy()));
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
            stack.push_back(stack[currentLocalsBase + ip->operand]);

            ++ip;
            goto NEXT_OP;
        }

    OP_WRITE_LOCAL:
        {
            stack[currentLocalsBase + ip->operand] = std::move(stack.back());
            stack.pop_back();

            ++ip;
            goto NEXT_OP;
        }

    OP_JUMP_IF_FALSE:
        {
            Value condition = std::move(stack.back());
            stack.pop_back();

            if (!condition.truthy())
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
            currentLocalsBase = base;
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
            currentLocalsBase = frames.back().localsBase;

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
