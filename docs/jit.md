# JIT engine

`jesus --jit program.jesus` compiles the program's platform-independent IR to
native machine code and runs it. This page describes the initial structure and
how to grow it.

## Pipeline

```
Source
  │
  ▼
Lexer → Parser → AST → Optimizer
                          │
                          ▼
                      Compiler
                          │
                          ▼
                IR (vm::Chunk / compiler::ir.hpp)
                          │
            ┌─────────────┴─────────────┐
            ▼                           ▼
       VM (bytecode)              JitEngine
                                        │
                                        ▼
                                    Assembler
                                      ├── x86_64  (asmjit)
                                      └── arm64   (placeholder)
```

The central representation is the existing bytecode `Chunk`. It already names
only abstract operations (`PUSH_LITERAL`, `ADD`, `PRINT`, `JUMP`, `CALL`,
`RETURN`, ...), so it is shared by the VM and the JIT. `compiler/ir.hpp` exposes
it under the name `IR` so assemblers do not need to depend on the VM.

## Directory layout

```
src/jesus/
├── compiler
│   └── ir.hpp                  # platform-independent IR alias
└── jit
    ├── jit_engine.cpp          # JIT compile-or-fallback orchestration
    ├── jit_engine.hpp
    ├── assembler.hpp           # Assembler interface
    ├── arm64
    │   └── arm64_assembler.hpp
    └── x86_64
        ├── x86_assembler.cpp   # x86_64 code generator
        └── x86_assembler.hpp
```

No `asmjit::x86::*` type appears outside `jit/x86_64/`. The JIT engine only
knows `Assembler` and `IR`.

## Current x86-64 support

Use `JESUS_JIT_DUMP=1` to print the IR the assembler receives and whether it
compiled it. `JESUS_JIT_NO_OPT=1` skips the AST optimizer before compilation,
which is useful to inspect the raw bytecode; the test runner sets it for
`tests/jit/` so those tests exercise real arithmetic instructions instead of
constants already folded by the optimizer.

## Adding more IR operations

Extend `jit/assembler.hpp` and `jit/x86_64/x86_assembler.hpp`.

When an operation is not implemented, returning `nullptr` is always safe,
making the operation run in the VM instead.
