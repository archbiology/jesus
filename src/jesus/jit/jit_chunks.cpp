#include "jit_chunks.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>

#include "ast/stmt/create_method_stmt.hpp"
#include "interpreter/runtime/method.hpp"
#include "vm/opcode.hpp"

namespace jit
{

JitChunks::JitChunks(const IR &program) : program(program)
{
}

bool JitChunks::registerAllChunks()
{
    // ---------------------------------
    // Register the `main` program chunk
    // ---------------------------------
    constexpr std::uint32_t mainParamsCount = 0;
    if (!registerChunk(program, mainParamsCount))
        return false;

    // --------------------------
    // Now register method chunks
    // --------------------------
    return registerMethodDefinitions();
}

const ChunksMap &JitChunks::map() const
{
    return chunksMap;
}

const std::vector<JitChunk> &JitChunks::all() const
{
    return chunks;
}

std::size_t JitChunks::countGlobalSlots() const
{
    std::uint32_t maxIndex = 0;
    bool found = false;

    for (const JitChunk &jitChunk : chunks)
    {
        for (const Instruction &instruction : jitChunk.chunk->instructions)
        {
            switch (instruction.opcode)
            {
            case OpCode::CREATE_GLOBAL:
            case OpCode::READ_GLOBAL:
            case OpCode::WRITE_GLOBAL:
            case OpCode::INIT_REPEAT_COUNT:
                if (!found || instruction.operand > maxIndex)
                    maxIndex = instruction.operand;
                found = true;
                break;

            default:
                break;
            }
        }
    }

    return found ? static_cast<std::size_t>(maxIndex) + 1 : 0;
}

void JitChunks::dump(const Assembler &assembler) const
{
    for (std::size_t id = 0; id < chunks.size(); ++id)
    {
        const JitChunk &jitChunk = chunks[id];
        std::cerr << "[JIT] " << assembler.name() << " chunk " << id;
        std::cerr << (id == 0 ? " (program)" : " (method)") << ":\n";

        for (std::size_t i = 0; i < jitChunk.chunk->instructions.size(); ++i)
        {
            const Instruction &instruction = jitChunk.chunk->instructions[i];
            std::cerr << "  " << i << ": " << opcodeToString(instruction.opcode) << " " << instruction.operand << "\n";
        }
    }
}

bool JitChunks::registerChunk(const IR &chunk, std::uint32_t paramsCount)
{
    if (chunksMap.contains(&chunk))
        return false;

    const ChunkId chunkId = static_cast<ChunkId>(chunks.size());
    chunks.push_back(JitChunk{&chunk, paramsCount, chunk.localCount});

    chunksMap.emplace(&chunk, chunkId);

    return true;
}

bool JitChunks::registerMethod(const std::shared_ptr<IMethod> &method)
{
    const auto userMethod = std::dynamic_pointer_cast<Method>(method);
    const bool isBuiltInMethod = !userMethod;
    const bool hasNoDefinition = userMethod->definition == nullptr;
    const bool hasNoCompiledBody = userMethod->definition->compiledBody == nullptr;
    const bool emptyBody = hasNoDefinition || hasNoCompiledBody;

    if (isBuiltInMethod || emptyBody)
        return true;

    const std::uint32_t paramsCount = userMethod->params ? userMethod->params->paramsCount : 0;
    return registerChunk(*userMethod->definition->compiledBody, paramsCount);
}

bool JitChunks::registerMethodDefinitions()
{
    for (const Value &literal : program.literals)
    {
        if (!literal.IS_CLASS)
            continue;

        const auto klass = literal.asClass();

        if (!klass)
            continue;

        for (const auto &method : klass->methods)
        {
            if (!registerMethod(method.second))
                return false;
        }
    }

    return true;
}

} // namespace jit
