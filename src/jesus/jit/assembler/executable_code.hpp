#pragma once

#include <memory>

#include "jit/jit_context.hpp"

namespace jit
{

/**
 * @brief Represents executable machine code produced by an Assembler.
 *
 * It holds the entrypoint to the generated code and keeps the runtime responsible
 * for managing its executable memory alive, ensuring the code remains valid
 * for as long as this object exists.
 */
class ExecutableCode
{
public:
    ExecutableCode(JitEntrypoint entrypoint, std::shared_ptr<void> runtime)
        : entrypoint(entrypoint), runtime(std::move(runtime))
    {
    }

    /**
     * @brief `JitContext` is passed in `RDI` (System V first argument).
     *
     * The initial program frame base is passed in `RSI` (System V second arg)
     */
    void execute(JitContext &context) const
    {
        constexpr MethodBaseSlot slotOfFirstFrame = 0;
        entrypoint(&context, slotOfFirstFrame);
    }

    /**
     * @brief The address of the generated machine code (assembly).
     */
    const JitEntrypoint entrypoint = nullptr;

private:
    /**
     * @brief (asmjit::JitRuntime) keeps the executable memory alive.
     *
     * This is necessary because the entrypoint points into memory owned by
     * the runtime. Without keeping the runtime alive, the generated code
     * could be released before execute() is called, leaving entrypoint
     * pointing to invalid memory.
     */
    std::shared_ptr<void> runtime;
};

} // namespace jit
