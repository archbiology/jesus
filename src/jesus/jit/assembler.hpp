#pragma once

#include <memory>

#include "compiler/ir.hpp"

namespace jit
{

/**
 * @brief A native function produced by am Assembler.
 *
 * `entrypoint` is the callable address. `runtime` keeps whatever keeps that code
 * alive (for asmjit, the JitRuntime that owns the executable memory).
 */
class ExecutableCode
{
public:
    using Entrypoint = void (*)();

    ExecutableCode(Entrypoint entrypoint, std::shared_ptr<void> runtime)
        : entrypoint(entrypoint), runtime(std::move(runtime))
    {
    }

    void execute() const
    {
        entrypoint();
    }

private:
    Entrypoint entrypoint = nullptr;
    std::shared_ptr<void> runtime; // asmjit::JitRuntime
};

/**
 * @brief Assembler Interface.
 *
 * An assembler receives platform-independent IR and either translates the whole
 * program to native code or reports that it cannot handle it. Assemblers are
 * deliberately unaware of the parser, the AST and the VM: the only contract
 * between the compiler and an assembler is `IR`.
 *
 * The x86_64 assembler is the first implementation. An arm64 assembler will be
 * added later without touching the parser, the IR or the JIT engine.
 */
class Assembler
{
public:
    /**
     * @brief Human-readable assembler name (used by diagnostics/tests).
     */
    virtual const char *name() const = 0;

    /**
     * @brief Assemble @p ir, or returns nullptr when it uses an operation this
     * assembler does not implement yet.
     *
     * Returning nullptr is the normal fallback path: the JIT engine then runs
     * the same IR on the bytecode VM, so `--jit` never rejects a program.
     */
    std::unique_ptr<ExecutableCode> assemble(const IR &ir)
    {
        return nullptr;
    }

    virtual ~Assembler() = default;
};

} // namespace jit
