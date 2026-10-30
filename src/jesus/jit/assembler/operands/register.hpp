#pragma once

namespace jit
{

/**
 * @brief 64-bit general-purpose registers (R = 64 bits)
 */
enum class Register
{
    /**
     * @brief RAX = Accumulator: Traditionally used for arithmetic, return values,
     * and certain instructions that implicitly operate on RAX.
     * A function returning a 64-bit integer normally places its result in RAX.
     */
    RAX, // Accumulator
    EAX, // 32-bit lower bit of RAX

    /**
     * @brief RBX = Base: Historically used to hold the
     * base address of a data structure or memory region.
     *
     * Use it to hold a value that must survive across a function call.
     */
    RBX, // Base

    /**
     * @brief RCX = Counter: Traditionally used as a loop counter
     * and by instructions that repeat an operation.
     *
     * Use it to hold a temporary value or loop counter.
     */
    RCX, // Counter
    ECX, // 32-bit lower bit of RCX

    /**
     * @brief RDX = Data: Historically paired with RAX in some arithmetic operations,
     * such as multiplication and division.
     *
     * Use it to hold another operand, a function argument,
     * or part of a multiplication/division result
     */
    RDX, // Data

    /**
     * @brief RSI = Source index: Traditionally points to the source data when copying
     * or processing sequences of bytes, such as strings or arrays.
     *
     * Use it to hold a pointer to source data or a function argument.
     */
    RSI, // Source index

    /**
     * @brief RDI = Destination index: Traditionally points to the destination when
     * copying or processing sequences of bytes, such as strings or arrays.
     *
     * Use it to hold a pointer to destination data or a function argument.
     * RDI: 64-bit register
     * RDI (64 bits)
        ┌────────────────────────────────┐
        │            RDI                 │
        └────────────────────────────────┘
                      ┌──────────────────┐
                      │       EDI        │
                      └──────────────────┘
                      Lower 32 bits of RDI
     */
    RDI, // Destination index
    EDI, // The lower 32-bit of RDI. The first int arg of functions goes here, determined by System V.

    /**
     * @brief RBP = Base/frame pointer: Can hold the address of a function's stack frame,
     * making it easier to access local variables and function arguments
     * at known offsets.
     *
     * Use it to access stack-frame variables when using a frame pointer.
     */
    RBP, // Base/frame pointer

    /**
     * @brief RSP = Stack pointer: Holds the address of the top of the current stack.
     * PUSH and POP, function calls, returns, and local-variable storage
     * interact with the stack. Adjusting RSP changes the stack position,
     * so it must be managed carefully to preserve stack integrity and
     * the required alignment when calling functions.
     *
     * Use it to manage the stack and allocate space for temporary values or local variables.
     */
    RSP, // Stack pointer
};

/**
 * @brief 128-bit SIMD registers used for floating-point operations.
 */
enum class SimdRegister
{
    XMM0,
    XMM1
};

using enum Register;
using enum SimdRegister;

} // namespace jit
