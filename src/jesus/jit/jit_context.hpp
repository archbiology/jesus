#pragma once

#include <vector>

#include "spirit/value.hpp"

namespace jit
{

/**
 * @brief Runtime state shared by every native function of one `--jit` run.
 *
 * The generated code receives a pointer to this object (System V: `RDI`)
 * and keeps it in the stack frame for helper calls. Globals live here so
 * `CREATE_GLOBAL` / `READ_GLOBAL` / `WRITE_GLOBAL` see the same array.
 */
struct JitContext
{
    std::vector<Value> globals;
};

} // namespace jit
