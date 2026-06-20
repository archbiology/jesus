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
 * Literals table:
 *
 *     0 -> 10
 *     1 -> 20
 *
 * Bytecode instructions:
 *
 *     PUSH_LITERAL 0
 *     PUSH_LITERAL 1
 *     ADD
 *     PRINT
 *     RETURN
 *
 * The VM executes instructions stored in `code`
 * and loads literal values from `literals`.
 */
class Chunk
{
public:
    std::vector<Instruction> instructions;
    std::vector<Value> literals;
};
