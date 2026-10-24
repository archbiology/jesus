
#include <cstdlib>
#include <iostream>
#include <string>

#include "jit_engine.hpp"
#include "x86_64/x86_assembler.hpp"
// #include "arm64/arm64_assembler.hpp"

#include "vm/opcode.hpp"

namespace jit
{

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

    if (std::getenv("JESUS_JIT_DUMP"))
    {
        std::cerr << "[JIT] " << assembler->name() << " chunk:\n";
        for (std::size_t i = 0; i < ir.instructions.size(); ++i)
        {
            const Instruction &instruction = ir.instructions[i];
            std::cerr << "  " << i << ": " << opcodeToString(instruction.opcode) << " " << instruction.operand << "\n";
        }
    }

    auto code = assembler->assemble(ir);

    if (!code)
    {
        if (std::getenv("JESUS_JIT_DUMP"))
            std::cerr << "[JIT] " << assembler->name() << " cannot compile this chunk; falling back to VM\n";

        return false;
    }

    std::cout << "[JIT:" << assembler->name() << "]\n";
    code->execute();
    return true;
}

} // namespace jit
