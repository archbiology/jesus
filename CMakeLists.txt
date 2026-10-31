cmake_minimum_required(VERSION 3.19)

# --------------------
# Set the project name
# --------------------
project(Jesus)

include_directories(src/jesus src/bible/include)

# --------------------
# Set the C++ standard
# --------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

message(NOTICE "\n   Jesus Christ loves YOU!!!\n")
message(NOTICE "   Confess Jesus Christ as your lord and savior, and live (type: jesus confess)")

# ------------------------------------------------------------------------------
# Version Information (auto-generated)
#
# We don't hardcode version numbers inside the source code.
# Instead, we generate a header file from a template (version.hpp.in).
#
# - version.hpp.in lives in src/jesus/utils/
# - CMake replaces tokens (@JESUS_BUILD_DATE@, @GIT_COMMIT_HASH@)
# - The final header is written to src/jesus/utils/version.hpp
#
# This ensures the binary always carries correct metadata (date + commit hash)
# without requiring manual updates in source files.
# ---
#
# JESUS_BUILD_DATE is generated in UTC+9, which we call "Jerusalem Scriptural Time (JST)".
# Why UTC+9?
# - In the Bible, a new day begins at sundown in Jerusalem (around 18:00 local time).
# - This moment aligns with 00:00 in Japan Standard Time (UTC+9).
# - Using UTC+9 therefore provides a consistent, unambiguous "scriptural day boundary".
#
# Why "Jerusalem Scriptural Time"?
# - To avoid confusion with "Jerusalem Standard Time", which refers to UTC+2/UTC+3.
# - To highlight the Biblical reasoning instead of civil timezone definitions.
# - To provide a clear, memorable alias (JST) that reflects the spiritual meaning.
#
# Thus, the build date is always stamped in UTC+9 to reflect the Biblical day cycle.
# ------------------------------------------------------------------------------
execute_process(
    COMMAND /bin/sh -c "TZ=Asia/Tokyo date '+%Y.%m.%d JST'" # JST = UTC+9
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE JESUS_BUILD_DATE
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND /bin/sh -c "TZ=Asia/Tokyo date '+%H:%M:%S'" # JST = UTC+9
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE JESUS_BUILD_TIME
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
execute_process(
    COMMAND git rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
configure_file(
    ${CMAKE_SOURCE_DIR}/src/jesus/utils/version.hpp.in
    ${CMAKE_SOURCE_DIR}/src/jesus/utils/version.hpp
    @ONLY
)
message(NOTICE "\n   Build date (Jerusalem Scriptural Time / UTC+9): ${JESUS_BUILD_DATE} ${JESUS_BUILD_TIME}\n")
message(NOTICE "   Build type: ${CMAKE_BUILD_TYPE}\n")

# -----------------
# asmjit (for JIT)
# -----------------
set(ASMJIT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/src/third_party/asmjit")
if(EXISTS "${ASMJIT_SOURCE_DIR}/CMakeLists.txt")
    message(NOTICE "   [Jesus] Using existing dependency: asmjit")
else()
    message(NOTICE "   [Jesus] Downloading dependency: asmjit")
    message(NOTICE "   [Jesus] Repository: https://github.com/archbiology/asmjit.git")
endif()
include(FetchContent)
FetchContent_Declare(
    asmjit
    GIT_REPOSITORY https://github.com/archbiology/asmjit.git
    GIT_TAG        0464e1c8f1f43c861a0c73d4e59437fff255cdd9
    SOURCE_DIR     "${ASMJIT_SOURCE_DIR}"
    BINARY_DIR     "${CMAKE_BINARY_DIR}/asmjit-build"
)
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
FetchContent_MakeAvailable(asmjit)
set(ASMJIT_STATIC ON CACHE BOOL "" FORCE)  # Link asmjit statically so the interpreter binary stays self-contained.
message(NOTICE "\n   [Jesus] Dependency ready: src/third_party/asmjit\n")
include(.config/cmake/configure_vscode_intellisense.cmake)

# -------------------
# Define source files
# -------------------
set(JESUS_CPP_FILES

    src/jesus/lexer/lexer.cpp
    src/jesus/utils/terminal/terminal.cpp
    src/jesus/utils/string_utils.cpp
    src/jesus/spirit/value.cpp
    src/jesus/spirit/heart.cpp
    src/jesus/spirit/spirit_of_understanding.cpp  # Semantics Analyzer

    # ----------------------
    # Initial variable types
    # ----------------------
    src/jesus/types/creation_type.cpp
    src/jesus/types/known_types.cpp
    src/jesus/types/composite/list_type.cpp
    src/jesus/types/composite/dict_type.cpp

    # ---------------
    # AST Expressions
    # ---------------
    src/jesus/ast/expr/ask_expr.cpp
    src/jesus/ast/expr/bible_expr.cpp
    src/jesus/ast/expr/unary_expr.cpp
    src/jesus/ast/expr/binary_expr.cpp
    src/jesus/ast/expr/literal_expr.cpp
    src/jesus/ast/expr/variable_expr.cpp
    src/jesus/ast/expr/create_instance_expr.cpp
    src/jesus/ast/expr/get_attr_expr.cpp
    src/jesus/ast/expr/parity_check_expr.cpp
    src/jesus/ast/expr/method_call_expr.cpp
    src/jesus/ast/expr/formatted_string_expr.cpp
    src/jesus/ast/expr/grouping_expr.cpp
    src/jesus/ast/expr/conditional_expr.cpp
    src/jesus/ast/expr/list_expr.cpp
    src/jesus/ast/expr/index_expr.cpp
    src/jesus/ast/expr/dict_expr.cpp
    src/jesus/ast/expr/enum_member_expr.cpp
    src/jesus/ast/expr/format_expr.cpp

    # --------------
    # AST Statements
    # --------------
    src/jesus/ast/stmt/break_stmt.cpp
    src/jesus/ast/stmt/skip_stmt.cpp
    src/jesus/ast/stmt/create_class_stmt.cpp
    src/jesus/ast/stmt/create_var_type_stmt.cpp
    src/jesus/ast/stmt/create_var_stmt.cpp
    src/jesus/ast/stmt/create_var_with_ask_stmt.cpp
    src/jesus/ast/stmt/update_var_with_ask_stmt.cpp
    src/jesus/ast/stmt/for_each_stmt.cpp
    src/jesus/ast/stmt/print_stmt.cpp
    src/jesus/ast/stmt/repeat_times_stmt.cpp
    src/jesus/ast/stmt/repeat_while_stmt.cpp
    src/jesus/ast/stmt/repeat_forever_stmt.cpp
    src/jesus/ast/stmt/update_var_stmt.cpp
    src/jesus/ast/stmt/assign_stmt.cpp
    src/jesus/ast/stmt/return_stmt.cpp
    src/jesus/ast/stmt/if_stmt.cpp
    src/jesus/ast/stmt/try_stmt.cpp
    src/jesus/ast/stmt/resist_stmt.cpp
    src/jesus/ast/stmt/import_module_stmt.cpp
    src/jesus/ast/stmt/inspect_stmt.cpp
    src/jesus/ast/stmt/memory_inspect_stmt.cpp
    src/jesus/ast/stmt/on_stmt.cpp
    src/jesus/ast/stmt/serve_stmt.cpp
    src/jesus/ast/stmt/create_enum_stmt.cpp

    # --------------
    # Parser classes
    # --------------
    src/jesus/parser/parser.cpp
    src/jesus/parser/parser_context.cpp
    src/jesus/parser/grammar/group_rule.cpp
    src/jesus/parser/grammar/unary_rule.cpp

    # ------------------
    # Expression parsers
    # ------------------
    src/jesus/parser/grammar/expr/ask_expr_rule.cpp
    src/jesus/parser/grammar/expr/conditional_expr_rule.cpp

    src/jesus/parser/grammar/expr/atomic/literals/number_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/string_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/formatted_string_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/format_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/yes_no_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/variable_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/self_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/born_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/weekday_rule.cpp

    src/jesus/parser/grammar/expr/atomic/operators/versus_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/addition_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/equality_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/comparison_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/logical_or_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/logical_and_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/multiplication_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/convert_to_rule.cpp

    src/jesus/parser/grammar/expr/keyword/bible_expr_rule.cpp
    src/jesus/parser/grammar/expr/keyword/confess_rule.cpp
    src/jesus/parser/grammar/expr/keyword/giants_rule.cpp

    src/jesus/parser/grammar/expr/postfix/get_attr_rule.cpp

    src/jesus/parser/grammar/expr/collection/list_rule.cpp
    src/jesus/parser/grammar/expr/collection/dict_rule.cpp

    src/jesus/parser/grammar/expr/instantiation/instantiation_rule.cpp

    # -----------------
    # Statement parsers
    # -----------------
    src/jesus/parser/grammar/stmt/create_class_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/create_var_type_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/create_var_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/update_var_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/assign_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/create_method_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/repeat_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/foreach_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/if_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/resist_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/try_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/import_module_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/inspect_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/memory_inspect_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/on_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/serve_stmt_rule.cpp
    src/jesus/parser/grammar/stmt/create_enum_stmt_rule.cpp

    # ---------
    # Optimizer
    # ---------
    src/jesus/optimizer/optimizer.cpp
    src/jesus/optimizer/constant_folder.cpp
    src/jesus/optimizer/constant_propagator.cpp
    src/jesus/optimizer/dead_code_eliminator.cpp
    src/jesus/optimizer/method_inliner.cpp
    src/jesus/optimizer/loop_unroller.cpp
    src/jesus/optimizer/deterministic_loop_optimizer.cpp

    # -----------
    # Interpreter
    # -----------
    src/jesus/interpreter/interpreter.cpp
    src/jesus/interpreter/runtime/method.cpp
    src/jesus/interpreter/runtime/http/http_runtime.cpp
    src/jesus/vm/opcode.cpp
    src/jesus/vm/compiler.cpp
    src/jesus/vm/vm.cpp

    # ---
    # CLI
    # ---
    src/jesus/cli/cli_parser.cpp    # Parser
    src/jesus/cli/disciple.cpp      # REPL
    src/jesus/cli/faith.cpp         # Michael, to read and execute files.jesus
    src/jesus/cli/bible.cpp         # Bible CLI parser: jesus john 3:16 --spanish
    src/jesus/cli/help.cpp          # HelpCLI: general help
    src/jesus/cli/uml_exporter.cpp
    src/jesus/understanding/shared/uml_graph.cpp
    src/jesus/understanding/scripture/book_aliases.cpp
    src/jesus/understanding/inspection/ast_inspector.cpp
    src/jesus/understanding/inspection/bytecode_inspector.cpp
    src/jesus/understanding/inspection/vm_inspector.cpp

    # --------
    # Builtins
    # --------
    src/jesus/builtins/io/socket/tcpv4_socket.cpp
    src/jesus/builtins/net/protocols/http/http_server.cpp

    # --------------
    # JIT (Assembly)
    # --------------
    src/jesus/jit/jit_engine.cpp
    src/jesus/jit/assembler/assembler_validation.cpp
    src/jesus/jit/assembler/assembler_compilation.cpp

    src/jesus/jit/assembler/assembler_arithmetic.cpp
    src/jesus/jit/assembler/assembler_print.cpp
    src/jesus/jit/assembler/assembler_values.cpp
    src/jesus/jit/assembler/assembler_globals.cpp

    src/jesus/jit/assembler/x86_64/x86_assembler.cpp
)

# --------------------------
# Add the `jesus` executable
# --------------------------
add_executable(jesus src/jesus/main.cpp ${JESUS_CPP_FILES})

# ------------------------------------------------------------------------
# Ensure the compressed Bible header is generated before building `jesus`.
#
# Build flow:
#   jesus (executable)
#     → depends on `compressed_bible` (custom target)
#         → depends on generated file: compressed_bible.hpp
#             → produced by `bible_compressor` via add_custom_command
#                 → re-runs when:
#                     - bible_compressor changes
#                     - any src/bible/data/en/*.hpp changes
# ------------------------------------------------------------------------
set(COMPRESSED_BIBLE_HPP ${CMAKE_SOURCE_DIR}/src/bible/include/bible/compressed_bible.hpp)
add_executable(bible_compressor ${CMAKE_SOURCE_DIR}/src/bible/tools/bible_compressor.cpp)
file(GLOB BIBLE_SRC_FILES CONFIGURE_DEPENDS ${CMAKE_SOURCE_DIR}/src/bible/data/en/*.hpp)
add_custom_command(
    OUTPUT ${COMPRESSED_BIBLE_HPP}
    COMMAND bible_compressor ${COMPRESSED_BIBLE_HPP}
    DEPENDS bible_compressor ${BIBLE_SRC_FILES}
    COMMENT "\nGenerating compressed_bible.hpp from src/bible/data/en/ files."
    VERBATIM
)
add_custom_target(generate_compressed_bible DEPENDS ${COMPRESSED_BIBLE_HPP})
target_link_libraries(jesus PRIVATE asmjit)
add_dependencies(jesus generate_compressed_bible)

# ------------------------------------------------------------------------------
# Add the `test_runner` executable
#
# It is emitted next to its source in `src/jesus/` (and NOT in the build folder)
# so that it can be run straight from the source tree, matching README.md.
#
# It uses C++23 (`std::string::contains`), hence the per-target standard below.
# ------------------------------------------------------------------------------
add_executable(test_runner src/jesus/test_runner.cpp)
set_target_properties(test_runner PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED True
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/src/jesus
)
