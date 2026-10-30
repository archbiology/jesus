#pragma once

#include "jit/assembler.hpp"

namespace jit
{

/**
 * @brief x86-64 code generator built on top of asmjit.
 *
 * It receives platform-independent IR and emits native x86-64 instructions.
 * No x86-specific type leaks into the IR, the parser or the JIT engine.
 */
class X86Assembler : public Assembler
{
public:
    const char *name() const override
    {
        return "x86-64";
    }
};

} // namespace jit
