#pragma once

#include "chunk.hpp"

#include "ast/expr/binary_expr.hpp"
#include "ast/expr/unary_expr.hpp"
#include "ast/expr/conditional_expr.hpp"
#include "ast/expr/create_instance_expr.hpp"
#include "ast/expr/literal_expr.hpp"
#include "ast/expr/variable_expr.hpp"
#include "ast/expr/method_call_expr.hpp"

#include "ast/stmt/create_var_stmt.hpp"
#include "ast/stmt/create_class_stmt.hpp"
#include "ast/stmt/create_method_stmt.hpp"
#include "ast/stmt/update_var_stmt.hpp"
#include "ast/stmt/assign_stmt.hpp"
#include "ast/stmt/if_stmt.hpp"
#include "ast/stmt/return_stmt.hpp"
#include "ast/stmt/print_stmt.hpp"
#include "ast/stmt/repeat_while_stmt.hpp"
#include "ast/stmt/break_stmt.hpp"

/**
 * @brief Converts an Abstract Syntax Tree (AST) into bytecode.
 *
 * The Compiler is responsible for transforming parsed Jesus
 * language structures into instructions that can be executed
 * by the Virtual Machine.
 *
 * Compilation pipeline:
 *
 *     Jesus Source
 *            ↓
 *         Lexer
 *            ↓
 *         Parser
 *            ↓
 *           AST
 *            ↓
 *        Compiler
 *            ↓
 *          Chunk
 *            ↓
 *           VM
 *
 * Example:
 *
 * Source:
 *
 *     say 10 + 20
 *
 * AST:
 *
 *     OutputStmt
 *         └── BinaryExpr(+)
 *                 ├── NumberExpr(10)
 *                 └── NumberExpr(20)
 *
 * Generated Chunk:
 *
 *     Constants:
 *
 *         0 -> 10
 *         1 -> 20
 *
 *     Instructions:
 *
 *         PUSH_LITERAL 0
 *         PUSH_LITERAL 1
 *         ADD
 *         PRINT
 *         RETURN
 *
 * The Compiler does not execute code.
 * Its only responsibility is generating bytecode.
 */
class Compiler
{
public:
    /**
     * @brief Compiles a program AST into executable bytecode.
     *
     * @param program Root AST node.
     *
     * @return Chunk containing instructions and constants
     *         ready for execution by the VM.
     */
    Chunk compile(const std::vector<std::unique_ptr<Stmt>> &statements);

private:
    Chunk chunk;
    Chunk *currentChunk = nullptr;

    std::unordered_map<std::string, uint32_t> globals;
    std::unordered_map<std::string, std::shared_ptr<CreationType>> classes;

    /**
     * @brief Maps local variable names to their slots in the current method.
     *
     * Slot 0 is reserved for "$self", followed by the parameters
     * and then the variables created inside the body. Example:
     *
     *   ┌──────────────┬──────┐
     *   │ "$self"      │  0   │
     *   │ "name"       │  1   │
     *   │ "message"    │  2   │
     *   └──────────────┴──────┘
     *
     * This map is COMPILER STATE and is discarded when method compilation
     * finishes (again, used only during compilation)
     */
    std::unordered_map<std::string, uint32_t> localSlots;

    /**
     * @brief Number of local slots handed out so far in the current method.
     */
    uint32_t localCount = 0;

    /**
     * @brief True while a method body is being compiled, so that variables
     * resolve to localSlots instead of globals.
     */
    bool compilingMethodBody = false;

    /**
     * @brief Jumps awaiting a patch, one entry per enclosing loop (innermost
     * last).
     *
     * A `break` emits a JUMP placeholder recorded in the innermost entry;
     * once the loop is compiled, every recorded jump is patched to the loop
     * exit (the same target as the loop's own exit jump).
     */
    std::vector<std::vector<uint32_t>> breakJumps;

    void compileStmt(const Stmt &stmt);

    /**
     * @brief Compiles an expression, leaving its result on the operand stack.
     *
     * @return true when the expression's compilation ends in a "join" —
     *  the point where the two paths of a conditional expression meet —
     *  so only the last path's instructions are visible in the chunk tail.
     *
     *  Optimizations must not combine instructions across such a
     *  point (see emitStoreToLocal and emitConditionalJump).
     *  All other expressions end with their own single instruction
     *  and return false.
     */
    bool compileExpr(const Expr &expr);

    void compileLiteralExpr(const LiteralExpr &expr);

    void compileBinaryExpr(const BinaryExpr &expr);

    void compileUnaryExpr(const UnaryExpr &expr);

    /**
     * @brief Compiles `then if condition otherwise else` with jumps:
     *
     *     evaluate condition
     *     JUMP_IF_FALSE -> elseBranch
     *     <thenBranch>       (leaves its value on the stack)
     *     JUMP -> end
     *     elseBranch:        (leaves its value on the stack)
     *     end:
     *
     * Both paths leave exactly one value, so the whole expression behaves
     * like any other stack value. Optimizations must not combine instructions
     * from opposite sides of the point where the two branches meet.
     */
    void compileConditionalExpr(const ConditionalExpr &expr);

    void compilePrintStmt(const PrintStmt &stmt);

    uint32_t addConstant(const Value &value);

    /**
     * @brief Appends an instruction to the current Chunk.
     */
    void emit(OpCode opcode);

    /**
     * @brief Appends an instruction that requires an operand.
     *
     * Example: LOAD_CONST 3
     *  Opcode: LOAD_CONST
     *  Operand: 3
     */
    void emit(OpCode opcode, uint32_t operand);

    /**
     * @brief Stores the value currently on top of the operand stack into a
     * local slot, fusing when possible.
     *
     * If the most recently emitted instruction is a fused arithmetic opcode
     * (e.g. MULTIPLY_LOCAL_LOCAL), it is rewritten into its store form
     * (MULTIPLY_LOCAL_LOCAL_STORE) so the result goes straight into the slot
     * without ever touching the operand stack. Otherwise a plain WRITE_LOCAL
     * is emitted.
     *
     * @param allowFusion must be false when the compiled value ends in a
     *  conditional join (see compileExpr): the fused form rewrites the last
     *  instruction, which only the else path ever reaches.
     */
    void emitStoreToLocal(uint32_t slot, bool allowFusion);

    uint32_t registerGlobalVar(const std::string &name);
    void compileCreateVarStmt(const CreateVarStmt &stmt);
    uint32_t getGlobalVar(const std::string &name);
    void compileVariableExpr(const VariableExpr &expr);
    void compileUpdateVarStmt(const UpdateVarStmt &stmt);

    /**
     * Returns the current instruction index in the chunk.
     *
     * Used as a label for jumps and control flow targets.
     *
     * Meaning: Where am I in the bytecode stream?
     */
    uint32_t currentOffset() const;

    /**
     * @brief Emit an instruction whose operand will be patched later, like JUMP_IF_FALSE
     *
     * @param opcode the code to be emited
     * @return uint32_t the instruction index
     */
    /**
     * Emits a jump-like instruction with a temporary operand (0)
     * that will be patched later once the target address is known.
     *
     * @param opcode the code to be emited, like JUMP_IF_FALSE
     * @return uint32_t the instruction index so it can be patched.
     */
    uint32_t emitPlaceholder(OpCode opcode);

    /**
     * @brief Emits a conditional jump that leaves when the last emitted
     * comparison is false.
     *
     * When the previous instruction is a fused comparison the call fuses it
     * with the jump (the comparison becomes a comparison-and-branch opcode
     * that skips materialising a boolean); otherwise it emits a plain
     * JUMP_IF_FALSE.
     *
     * @param allowFusion must be false when the compiled condition ends in a
     *  conditional join (see compileExpr): the fusion rewrites the last
     *  instruction, which only one of the paths ever reaches.
     * @return uint32_t the index of the instruction whose operand is patched
     *                  with the jump target.
     */
    uint32_t emitConditionalJump(bool allowFusion);

    /**
     * @brief Patches a previously emitted jump instruction with the current instruction position.
     *
     * @param instructionIndex the index of the instruction to be patched
     */
    void patchJump(uint32_t instructionIndex);

    /**
     * Compiles:
     *
     * repeat while condition:
     *     body
     * amen
     *
     * Control flow shape:
     *
     *  loopStart:
     *     evaluate condition
     *     if false → exit
     *     body
     *     jump loopStart
     *  exit:
     */
    void compileRepeatWhileStmt(const RepeatWhileStmt &stmt);

    /**
     * Compiles `break` as a jump to the exit of the innermost enclosing
     * loop, instead of throwing a C++ signal like the AST interpreter does.
     *
     * Throws when there is no enclosing loop (the AST interpreter lets the
     * signal escape and crashes at that point; both abort the program).
     */
    void compileBreakStmt(const BreakStmt &stmt);

    void compileCreateClassStmt(const CreateClassStmt &stmt);
    void compileCreateInstanceExpr(const CreateInstanceExpr &expr);
    void compileMethodCallExpr(const MethodCallExpr &expr);
    void compileAssignStmt(const AssignStmt &stmt);
    void compileIfStmt(const IfStmt &stmt);
    void compileMethodBody(const CreateMethodStmt &method);
    void compileReturnStmt(const ReturnStmt &stmt);

    /**
     * @brief Returns the local slot of a name, handing out a new slot the
     * first time the name is seen inside the current method.
     */
    uint32_t localSlot(const std::string &name);
};
