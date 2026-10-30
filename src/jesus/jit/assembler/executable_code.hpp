#pragma once

#include <memory>

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
    /**
     * @brief The address of the generated machine code (assembly).
     */
    Entrypoint entrypoint = nullptr;

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
