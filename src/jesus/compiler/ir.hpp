#pragma once

#include "vm/chunk.hpp"

/**
 * @brief Platform-independent intermediate representation (IR).
 *
 * The Jesus compiler converts the AST into a `Chunk` of stack-machine
 * instructions. A Chunk is already free of any host-specific detail: it only
 * names abstract operations (PUSH_LITERAL, ADD, PRINT, JUMP, CALL, RETURN,
 * ...) and literal values.
 *
 * That makes it the central representation consumed by every execution
 * strategy:
 *
 *     AST ──> Compiler ──> IR (Chunk) ──┬──> VM        (bytecode interpreter)
 *                                       └──> JIT engine ──> Assembler
 *                                                          ├── x86_64
 *                                                          └── arm64 (future)
 *
 * The alias below gives the IR a name of its own, so assemblers can depend on
 * `IR` instead of the VM's `Chunk` and the two layers stay conceptually
 * separate even while they share the same data structure.
 */
using IR = Chunk;
