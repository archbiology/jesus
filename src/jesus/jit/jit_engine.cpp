#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "jit_engine.hpp"
#include "jit_chunks.hpp"
#include "jit_context.hpp"
#include "assembler/x86_64/x86_assembler.hpp"
// #include "assembler/arm64/arm64_assembler.hpp"

namespace jit
{
namespace
{

/**
 * @brief Releases assembler state as soon as code generation finishes.
 *
 * The destructor runs while the chunk map is still alive, so the assembler
 * never keeps a dangling pointer to compilation-only chunk data.
 */
class CompilationDataGuard
{
public:
    explicit CompilationDataGuard(Assembler &assembler) : assembler(assembler)
    {
    }

    ~CompilationDataGuard()
    {
        assembler.releaseCompilationData();
    }

    CompilationDataGuard(const CompilationDataGuard &) = delete;
    CompilationDataGuard &operator=(const CompilationDataGuard &) = delete;

private:
    Assembler &assembler;
};

} // namespace

std::unique_ptr<Assembler> createAssembler()
{
#if defined(__x86_64__) || defined(_M_X64)
    return std::make_unique<X86Assembler>();
#elif defined(__aarch64__) || defined(_M_ARM64)
    return std::make_unique<Arm64Assembler>();
#else
#error "Jesus JIT currently supports only x86-64 and ARM64"
#endif
}

JitEngine::JitEngine(std::unique_ptr<jit::Assembler> assembler) : assembler(std::move(assembler))
{
}

const char *JitEngine::assemblerName() const
{
    return assembler ? assembler->name() : "none";
}

bool JitEngine::execute(const IR &ir)
{
    if (!assembler)
        return false;

    const bool shouldDump = std::getenv("JESUS_JIT_DUMP") != nullptr;

    std::vector<std::unique_ptr<ExecutableCode>> executableChunks;
    JitContext context;

    // ----------------------------------------------------------
    // Compilation-only data lives only in this block.
    // It is released before main->execute() below,
    // so it does not occupy memory while native code is running.
    // ----------------------------------------------------------
    {
        JitChunks jitChunks(ir);
        CompilationDataGuard compilationDataGuard(*assembler);

        // -------------------------
        // First, extract all chunks
        // -------------------------
        if (!jitChunks.registerAllChunks())
        {
            if (shouldDump)
                std::cerr << "[JIT] " << assembler->name()
                          << " cannot register all method chunks; falling back to VM\n";

            return false;
        }

        assembler->setChunksMap(jitChunks.map());

        // -----------------------------------------
        // Assemble each chunk to JIT executable code
        // -----------------------------------------
        if (shouldDump)
            jitChunks.dump(*assembler);

        const auto &chunks = jitChunks.all();
        executableChunks.reserve(chunks.size());

        for (std::size_t chunkId = 0; chunkId < chunks.size(); ++chunkId)
        {
            auto jitCode = assembler->assemble(*chunks[chunkId].chunk);

            if (!jitCode)
            {
                if (shouldDump)
                    std::cerr << "[JIT] " << assembler->name() << " cannot compile this chunk; falling back to VM\n";

                return false;
            }

            executableChunks.push_back(std::move(jitCode));
        }

        // -------------------------
        // Prepare runtime-only state
        // -------------------------
        context.globals.resize(jitChunks.countGlobalSlots());
        context.jitMethods.resize(chunks.size());

        for (std::size_t id = 0; id < chunks.size(); ++id)
        {
            context.jitMethods[id] = JitMethod{
                executableChunks[id]->entrypoint,
                chunks[id].paramsCount,
                chunks[id].localCount,
            };
        }
    }

    // -------------------------
    // Now execute the JIT code
    // -------------------------
    std::cout << "[JIT:" << assembler->name() << "]\n";

    auto &main = executableChunks.front();
    main->execute(context);

    return true;
}

} // namespace jit
