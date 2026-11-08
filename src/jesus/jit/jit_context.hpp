#pragma once

#include <vector>

#include "spirit/value.hpp"

namespace jit
{

struct JitMethod; // forward declaration
using MethodBaseSlot = std::uint32_t;

/**
 * @brief Runtime state shared by every native function on `--jit` runs.
 *
 * The JIT code receives a pointer to this object (System V: `RDI`)
 * and keeps it in the stack frame for helper calls. Global variables live
 * here so CREATE_GLOBAL / READ_GLOBAL / WRITE_GLOBAL access the same values.
 */
struct JitContext
{
    /**
     * @brief Stores global variables and other runtime values.
     *
     * This vector holds two kinds of values:
     *
     * 1. Global variables, stored at indexes assigned by the compiler.
     * 2. Other runtime values, such as instances, appended
     *    after the global variable slots.
     *
     * The global variable slots are reserved before execution by
     * countGlobalSlots(). Other runtime values are appended as needed.
     *
     * Example:
        +-------+----------------------+-------------------+
        | Index | Value                | Purpose           |
        +-------+----------------------+-------------------+
        |   0   | God = 1              | Global variable   |
        |   1   | son = 'Yeshua'       | Global variable   |
        |   2   | holy = 'Spirit'      | Global variable   |
        +-------+----------------------+-------------------+
        |    Indexes 0-2: Reserved before execution.       |
        |    Indexes 3-5: Appended during execution.       |
        +-------+----------------------+-------------------+
        |   3   | Person class         | Runtime value     |
        |   4   | Adam instance        | Runtime value     |
        |   5   | Eve instance         | Runtime value     |
        +-------+----------------------+-------------------+
     */
    std::vector<Value> globals;

    /**
     * @brief Stores values used during method calls.
     *
     * When one method calls another, the instance and arguments are placed
     * on this stack before the called method starts executing.
     *
     * The called method's frame begins at the position of slot 0 (`$self`).
     * This gives the method a predictable starting position for accessing
     * its instance, arguments and local variables.
     *
     * When the called method executes RETURN, its frame is removed from
     * the stack, but its return value is kept. This allows the calling
     * method to use the returned value without losing its own variables
     * and other values already on the stack.
     *
     * After RETURN, the return value occupies slot 0, replacing the
     * instance that was there before the method was called.
     */
    std::vector<Value> jitRuntimeStack;

    /**
     * @brief JIT-compiled methods.
     */
    std::vector<JitMethod> jitMethods;

    /**
     * @brief Slot where the current frame begins in `jitRuntimeStack`.
     */
    MethodBaseSlot currentMethodBaseSlot = 0;
};

/**
 * @brief Entry point of one compiled chunk.
 *
 * RDI receives the shared JitContext.
 * RSI receives the base slot of the current method/frame.
 */
using JitEntrypoint = void (*)(JitContext *, MethodBaseSlot);

/**
 * @brief JIT method
 *
* It provides the JIT entrypoint of methods, the number of
* parameters and local variables required by the corresponding chunk.
 */
struct JitMethod
{
    JitEntrypoint entrypoint = nullptr;
    std::uint32_t paramsCount = 0;
    std::uint32_t localCount = 0;
};

} // namespace jit
