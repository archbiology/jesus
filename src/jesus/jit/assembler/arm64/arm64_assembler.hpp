#pragma once

#include "jit/assembler/assembler.hpp"

namespace jit
{

/**
 * @brief AArch64 assembler placeholder.
 *
 * The structure is ready: once implemented, it will receive the same
 * platform-independent IR as the x86-64 assembler, so neither the parser nor
 * the IR nor the JIT engine needs to change.
 *
 * Until then `compile()` always returns nullptr and the JIT engine falls back
 * to the VM.
 */
class Arm64Assembler : public Assembler
{
public:
    const char *name() const override
    {
        return "arm64";
    }
};

} // namespace jit
