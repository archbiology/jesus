#pragma once

#include <vector>

#include "spirit/value.hpp"

namespace jit
{

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
};

} // namespace jit
