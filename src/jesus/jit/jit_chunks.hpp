#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "compiler/ir.hpp"
#include "jit/assembler/assembler.hpp"

class IMethod;

namespace jit
{

struct JitChunk
{
    const IR *chunk;
    std::uint32_t paramsCount;
    std::uint32_t localCount;
};

/**
 * @brief Owns the chunks discovered for one program and their runtime ids.
 *
 * Registration walks the IR only while the program is being prepared. The
 * engine destroys this object before native execution starts, so chunk
 * metadata does not stay alive during the run (to save RAM).
 */
class JitChunks
{
public:
    explicit JitChunks(const IR &program);

    bool registerAllChunks();

    const ChunksMap &map() const;
    const std::vector<JitChunk> &all() const;

    /**
     * @brief Counts the slots needed for global variables.
     *
     * Scans all IR instructions to find the highest global index.
     * Returns the highest index plus one because indexes start at zero.
     *
     * This ensures all global variable slots exist before execution,
     * so the runtime can safely access globals[index].
     *
     * Check JitRuntimeUpdateGlobalValue if you plan to
     * refactor this function.
     */
    std::size_t countGlobalSlots() const;

    void dump(const Assembler &assembler) const;

private:
    /**
     * @brief Registers an IR chunk and assigns its ID.
     *
     * The chunk ID is its position in the chunks vector.
     * The main program is registered first and therefore receives ID 0.
     *
     * Returns false if the same IR has already been registered.
     */
    bool registerChunk(const IR &chunk, std::uint32_t paramsCount);

    /**
     * @brief Registers a compiled user-defined method.
     *
     * Built-in methods and methods without a compiled body are ignored.
     */
    bool registerMethod(const std::shared_ptr<IMethod> &method);

    /**
     * @brief Registers all compiled methods declared by classes in the program.
     */
    bool registerMethodDefinitions();

    const IR &program;
    ChunksMap chunksMap;
    std::vector<JitChunk> chunks;
};

} // namespace jit
