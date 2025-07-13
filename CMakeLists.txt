cmake_minimum_required(VERSION 3.10)

# --------------------
# Set the project name
# --------------------
project(Jesus)

include_directories(src/jesus)

# --------------------
# Set the C++ standard
# --------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# -------------------
# Define source files
# -------------------
set(JESUS_CPP_FILES

    src/jesus/lexer/lexer.cpp
    src/jesus/spirit/value.cpp
    src/jesus/spirit/heart.cpp

    # ---------------
    # AST Expressions
    # ---------------
    src/jesus/ast/expr/unary_expr.cpp
    src/jesus/ast/expr/binary_expr.cpp
    src/jesus/ast/expr/literal_expr.cpp
    src/jesus/ast/expr/variable_expr.cpp
    src/jesus/ast/expr/grouping_expr.cpp
    src/jesus/ast/expr/conditional_expr.cpp

    # --------------
    # AST Statements
    # --------------
    src/jesus/ast/stmt/break_stmt.cpp
    src/jesus/ast/stmt/continue_stmt.cpp
    src/jesus/ast/stmt/create_var_stmt.cpp
    src/jesus/ast/stmt/for_each_stmt.cpp
    src/jesus/ast/stmt/output_stmt.cpp
    src/jesus/ast/stmt/repeat_times_stmt.cpp
    src/jesus/ast/stmt/repeat_while_stmt.cpp
    src/jesus/ast/stmt/update_var_stmt.cpp

    # --------------
    # Parser classes
    # --------------
    src/jesus/parser/parser.cpp
    src/jesus/parser/grammar/group_rule.cpp
    src/jesus/parser/grammar/unary_rule.cpp

    # ------------------
    # Expression parsers
    # ------------------
    src/jesus/parser/grammar/expr/conditional_expr_rule.cpp

    src/jesus/parser/grammar/expr/atomic/literals/number_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/string_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/yes_no_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/variable_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/born_rule.cpp
    src/jesus/parser/grammar/expr/atomic/literals/weekday_rule.cpp

    src/jesus/parser/grammar/expr/atomic/operators/versus_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/addition_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/equality_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/comparison_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/logical_or_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/logical_and_rule.cpp
    src/jesus/parser/grammar/expr/atomic/operators/multiplication_rule.cpp

    # -----------------
    # Statement parsers
    # -----------------
    src/jesus/parser/grammar/stmt/create_var_stmt_rule.cpp

    # -----------
    # Interpreter
    # -----------
    src/jesus/interpreter/interpreter.cpp

)

# --------------------------
# Add the `jesus` executable
# --------------------------
add_executable(jesus src/jesus/main.cpp ${JESUS_CPP_FILES})
