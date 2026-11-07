#include "assembler.hpp"

#include <memory>

#include "interpreter/runtime/instance.hpp"
#include "jit/jit_context.hpp"
#include "spirit/value.hpp"

namespace jit
{

// --------------------------------------------------------------------------
// Runtime value storage
//
// JitContext::globals contains two kinds of values:
//
// 1. Global variables, stored at indexes assigned by the compiler.
// 2. Other runtime values, appended after the global variable slots.
//
// The global variable slots are created before execution by
// countGlobalSlots(). This allows runtime helpers to access global variables
// always using the same compiler-assigned indexes.
// --------------------------------------------------------------------------
static std::uint32_t appendRuntimeValue(JitContext *ctx, const Value &value)
{
    ctx->globals.push_back(value);
    return static_cast<std::uint32_t>(ctx->globals.size() - 1);
}

/**
 * @brief Appends a literal value to `globals` and returns its index.
 */
extern "C" std::uint32_t JitRuntimeAppendLiteral(JitContext *ctx, const Value *value)
{
    return appendRuntimeValue(ctx, *value);
}

/**
 * @brief Creates an instance of a class and returns the instance's index.
 */
extern "C" std::uint32_t JitRuntimeCreateInstance(JitContext *ctx, std::uint32_t klassIndex)
{
    auto klass = ctx->globals[klassIndex].asClass();
    return appendRuntimeValue(ctx, Value(std::make_shared<Instance>(klass)));
}

/**
 * @brief Copies a runtime value into a global variable slot.
 *
 * Called when creating or updating global variables
 */
extern "C" void JitRuntimeUpdateGlobalValue(JitContext *ctx, std::uint32_t globalIndex, std::uint32_t valueIndex)
{
    // ---------------------------------------------------------------
    // Here we don't need:
    //
    // if (globalIndex >= ctx->globals.size())
    //     ctx->globals.resize(globalIndex + 1);
    //
    // because countGlobalSlots() in jit_engine.cpp scans the
    // entire IR and reserves all compiler-assigned global
    // indexes before execution.
    //
    // Do not remove this explanation or the initialization in
    // jit_engine.cpp without first ensuring that global indexes
    // are still valid before this function is called.
    //
    // Removing an apparently unnecessary operation without
    // understanding the assumptions behind it can introduce
    // serious bugs, as demonstrated in SSH CVE-2026-123158
    // that spans across decades, for assumptions made in 1970
    // was broken by refactor in 2012 without knowing the assumptions.
    // See: https://www.youtube.com/watch?v=ErB1SRcksDc
    // ---------------------------------------------------------------

    ctx->globals[globalIndex] = ctx->globals[valueIndex];
}

/**
 * @brief Copies a global variable's value into runtime storage.
 *
 * Returns the index of the copied value.
 *
 * Example:
 *   create globalVar = 1
 *
 *   let there be Grace:
 *       purpose increase():
 *           create localVar = globalVar # <--- here
 *           ...
 *       amen
 *   amen
 */
extern "C" std::uint32_t JitRuntimeCopyGlobalValue(JitContext *ctx, std::uint32_t globalIndex)
{
    return appendRuntimeValue(ctx, ctx->globals[globalIndex]);
}

bool Assembler::emitCreateInstance()
{
    if (jitStack.empty() || !jitStack.back().isValueIndex())
        return false;

    jitStack.pop_back();

    const MemoryOffset offset = static_cast<MemoryOffset>(jitStack.size() * JIT_SLOT_SIZE_BYTES);

    // -----------------------------------------------------
    // Pass the args of the function JitRuntimeCreateInstance
    //      RDI/EDI: first argument  (JitContext)
    //      ESI:     second argument (class value index)
    // -----------------------------------------------------
    loadJitContextPointerIntoRDI();
    mov(RSI, dword_ptr(RSP, offset));

    mov(RAX, Immediate(&JitRuntimeCreateInstance));
    call(RAX);

    // ----------------------------------------------------
    // The helper returns the new instance index in EAX.
    // It replaces the class value in the same stack slot.
    // ----------------------------------------------------
    mov(dword_ptr(RSP, offset), EAX);
    pushSlot(Slot::ValueIndex());
    return true;
}

} // namespace jit
