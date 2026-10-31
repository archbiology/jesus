#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

#include "compiler/ir.hpp"
#include "executable_code.hpp"
#include "operands/immediate.hpp"
#include "operands/memory.hpp"
#include "operands/register.hpp"
#include "slot.hpp"

namespace jit
{

/**
 * @brief Memory position, measured in bytes, relative to the stack pointer.
 *
 * We use a 32-bit integer because x86-64 instructions encode
 * this offset using at most 32 bits.
 */
using MemoryOffset = std::int32_t;

/**
 * @brief Each JIT slot occupies 8 bytes (64 bits)
 *
 * This constant refers to a slot in the JIT's runtime stack, not to the
 * `Slot` struct used by the compiler when generating machine code.
 */
inline constexpr std::size_t JIT_SLOT_SIZE_BYTES = sizeof(std::uint64_t); // = (8 bytes, 64 bits)

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
    virtual std::unique_ptr<ExecutableCode> assemble(const IR &ir) = 0;

    virtual ~Assembler() = default;

protected:
    /**
     * @brief Validates @p ir and computes the maximum native stack depth.
     *
     * Assemblers call this before emitting the machine code so the
     * stack frame is large enough for every intermediate value.
     */
    static bool validateSupportedJitOperations(const IR &ir, std::size_t &maxSlots);

    /**
     * @brief Compile @p ir (Intermediate Representation) to machine code.
     *
     * Implemented once in assembler.cpp so every backend shares the same
     * platform-independent stack-machine translation.
     */
    bool compileToMachineCode(const IR &ir);

    // ---------
    // Move data
    // ---------
    virtual void mov(Register destination, Immediate value) = 0;
    virtual void mov(Register destination, DwordMemory source) = 0;
    virtual void mov(Register destination, QwordMemory source) = 0;
    virtual void mov(DwordMemory destination, Register source) = 0;
    virtual void mov(DwordMemory destination, Immediate value) = 0;
    virtual void mov(QwordMemory destination, Register source) = 0;

    /**
     * @brief Move data from a general-purpose register to an XMM register.
     */
    virtual void movq(SimdRegister destination, Register source) = 0;

    // ---------------------------------
    // Move scalar floating-point values
    // ---------------------------------
    virtual void movsd(SimdRegister destination, QwordMemory source) = 0;
    virtual void movsd(QwordMemory destination, SimdRegister source) = 0;

    // -------------------------------------------------------------------
    // Convert a signed integer to a double-precision floating-point value
    // -------------------------------------------------------------------
    virtual void cvtsi2sd(SimdRegister destination, Register source) = 0;
    virtual void cvtsi2sd(SimdRegister destination, DwordMemory source) = 0;

    // ------------------
    // Integer arithmetic
    // ------------------
    virtual void add(Register destination, Register source) = 0;
    virtual void add(Register destination, Immediate source) = 0;
    virtual void sub(Register destination, Register source) = 0;
    virtual void sub(Register destination, Immediate value) = 0;
    virtual void imul(Register destination, Register source) = 0;

    // -------------------------
    // Floating-point arithmetic
    // -------------------------
    virtual void addsd(SimdRegister destination, SimdRegister source) = 0;
    virtual void subsd(SimdRegister destination, SimdRegister source) = 0;
    virtual void mulsd(SimdRegister destination, SimdRegister source) = 0;
    virtual void divsd(SimdRegister destination, SimdRegister source) = 0;

    // --------------------------
    // Function calls and returns
    // --------------------------
    virtual void call(Register target) = 0;
    virtual void ret() = 0;

    /**
    * @brief The distance, in bytes, from RSP to the memory location where we store the JitContext*.
    *
    * X86Assembler::assemble sets this offset before calling compileToMachineCode()
    *
    * The JIT value slots start at offset 0.
    * The JitContext pointer is stored immediately after the last value slot.
    * Example:
    *   RSP
        |
        +---------------------------+
        | Value slot 0 (8 bytes)    | Offset 0
        +---------------------------+
        | Value slot 1 (8 bytes)    | Offset 8
        +---------------------------+
        | Value slot 2 (8 bytes)    | Offset 16
        +---------------------------+
        | Value slot 3 (8 bytes)    | Offset 24
        +---------------------------+
        | JitContext* (8 bytes)     | Offset 32  <-- jitContextOffset
        +---------------------------+
    */
    MemoryOffset jitContextOffset = 0;

private:
    std::vector<Slot> jitStack;
    std::vector<JitValueType> globalVariableTypes;
    const IR *ir = nullptr;

    void pushSlot(const Slot &slot)
    {
        jitStack.push_back(slot);
    }
    static std::int32_t offsetOf(std::size_t byteOffset);
    void emitPushLiteral(const Instruction &instruction);
    /**
     * @brief RDI contains the first arg of functions
     *
     * Loads the JitContext for the JitRuntime* functions when storing/reading globals.
     */
    void loadJitContextPointerIntoRDI();
    void recordGlobalVarType(std::uint32_t index, JitValueType valueType);
    bool emitStoreGlobal(const Instruction &instruction);
    bool emitReadGlobal(const Instruction &instruction);
    void loadNumberFromMemoryToSimd(const Slot &slot, MemoryOffset offset, const SimdRegister &simdRegister);
    void loadNumberToSimd(
        const Slot &slot,
        const bool isRightOperandInMemory,
        std::size_t memoryOffset,
        const SimdRegister &simdRegister);
    static bool isArithmetic(OpCode opcode);
    static bool isFusedOpForRightLiteral(OpCode opcode, OpCode &operation);
    bool emitArithmeticOperation(
        OpCode opcode,
        const Slot &left,
        MemoryOffset leftOffset,
        const Slot &right,
        const bool isRightOperandInMemory,
        MemoryOffset rightOffset,
        Slot &result);
    bool emitBinaryArithmeticOperation(OpCode opcode);
    bool emitFusedLiteralRight(OpCode base, const Instruction &instruction);
    bool emitPrint();
};

inline std::uint32_t extractLower16Bits(std::uint32_t value)
{
    // -----------------------------------------------------------
    // Each hexadecimal is 4 bits (0xF = 1111 -> 0xFFFF = 16 bits)
    //
    //      value:  10101010 11001100 11110000 00001111
    //      mask:   00000000 00000000 11111111 11111111
    //              -----------------------------------
    //      result: 00000000 00000000 11110000 00001111
    // -----------------------------------------------------------
    return value & 0xFFFF;
}

} // namespace jit
