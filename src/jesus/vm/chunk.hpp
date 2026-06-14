#pragma once

#include <vector>
#include "instruction.hpp"
#include "spirit/value.hpp"

/**
 * @brief Compiled bytecode program.
 *
 * A Chunk (pedaço) contains:
 *
 * 1. Executable bytecode instructions.
 * 2. Constant values referenced by those instructions.
 *
 * Example:
 *
 * Jesus source:
 *
 *     say 10 + 20
 *
 * Constants table:
 *
 *     0 -> 10
 *     1 -> 20
 *
 * Bytecode instructions:
 *
 *     LOAD_CONST 0
 *     LOAD_CONST 1
 *     ADD
 *     PRINT
 *     RETURN
 *
 * The VM executes instructions stored in `code`
 * and loads literal values from `constants`.
 */
class Chunk
{
public:
    std::vector<Instruction> instructions;
    std::vector<Value> constants;
};
