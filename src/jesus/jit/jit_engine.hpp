#pragma once

#include <memory>

#include "assembler.hpp"
#include "compiler/ir.hpp"

namespace jit
{

std::unique_ptr<Assembler> createAssembler();

/**
 * @brief Executes IR natively, with a graceful VM fallback.
 *
 * The engine itself is assembler-agnostic: it owns an `Assembler` and asks
 * it to translate the IR. If the assembler cannot handle the whole chunk, the
 * engine reports `false` and the caller runs the same IR on the VM. This is
 * what makes `jesus --jit` always usable while assemblers grow.
 *
 *     IR (Chunk) ──> JitEngine ──> Assembler ──> native code
 *                        │
 *                        └───────> fallback: bytecode VM
 */
class JitEngine
{
public:
    explicit JitEngine(std::unique_ptr<jit::Assembler> assembler);

    /**
     * @brief Compiles and runs @p ir.
     *
     * @return true when the chunk ran as native code; false when the caller
     *         must run it on the VM instead.
     */
    bool execute(const IR &ir);

    const char *assemblerName() const;

private:
    std::unique_ptr<jit::Assembler> assembler;
};

} // namespace jit
