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
    src/jesus/parser/parser.cpp
    src/jesus/interpreter/interpreter.cpp
    src/jesus/parser/grammar/group_rule.cpp
    src/jesus/parser/grammar/unary_rule.cpp
    src/jesus/parser/grammar/primitives/number_rule.cpp
    src/jesus/parser/grammar/primitives/string_rule.cpp
    src/jesus/parser/grammar/primitives/versus_rule.cpp
    src/jesus/parser/grammar/primitives/yes_no_rule.cpp
    src/jesus/parser/grammar/primitives/addition_rule.cpp
    src/jesus/parser/grammar/primitives/equality_rule.cpp
    src/jesus/parser/grammar/primitives/variable_rule.cpp
    src/jesus/parser/grammar/primitives/comparison_rule.cpp
    src/jesus/parser/grammar/primitives/logical_or_rule.cpp
    src/jesus/parser/grammar/primitives/logical_and_rule.cpp
    src/jesus/parser/grammar/primitives/multiplication_rule.cpp
)

# --------------------------
# Add the `jesus` executable
# --------------------------
add_executable(jesus src/jesus/main.cpp ${JESUS_CPP_FILES})
