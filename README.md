# Jesus Language

> _“In the beginning was the Word, and the Word was with God, and the Word was God.” — John 1:1_

**Jesus** is a minimal programming language designed to explore clarity, structure, and purpose in code — all for the glory of God.

This repository currently contains the basic lexer (lexical analyzer) and REPL loop for interacting with the language.

## 🛠 Compilation

This project uses **CMake** and **Make** to simplify building.

##### Requirements

Make sure you have:

* A C++ compiler that supports C++17 (e.g., `g++`)
* `cmake` installed (version 3.10 or higher)
* `make`

##### Build & Run

In the root folder of the project, run:

```bash
make clean     # Optional: cleans previous build artifacts
make           # Compiles the project using CMake and Make
make run       # Runs the Jesus interpreter
```

##### Output

After running, you’ll see the REPL prompt:

```
(Jesus)
```

Type any valid expression or command and press enter.


## 🧪 Running the Test Suite

The Jesus language comes with a built-in test runner to validate the behavior of `.jesus` programs.

Each test is composed of:

- A `.jesus` file containing the program to be interpreted.
- A corresponding `.jesus.expected` file that contains the expected combined output from `stdout` and `stderr`.

### ▶️ How to Run Tests

Make sure both the `jesus` binary and the `test_runner` are built. Build the `test_runner` using this command:
```
cd src/jesus/

g++ -std=c++17 -o test_runner test_runner.cpp
```

Then simply run:

```bash
./test_runner
```

✅ What It Does

- Automatically runs all `.jesus` test files in the [tests](src/jesus/tests) directory.

- Captures both stdout and stderr of the Jesus interpreter.

- Compares the actual output against the .expected file.

- Shows a per-line diff with ✅ or 🔴 indicators.

- Fails with exit code 1 if any test fails.

🛠 Example Output

```
[✓] Passed: tests/hello_world.jesus
[✗] Failed: tests/if_else.jesus
Line 3 differs 🔴️:
  Expected: 'Result: 42'
  Actual:   'Result: 0'
```

💡 Tip for Contributors

To add a new test, create a file like `src/jesus/tests/my_test.jesus` and its expected output in `src/jesus/tests/my_test.jesus.expected`.

You can generate the .expected file like this:

```
./jesus < tests/my_test.jesus &> tests/my_test.jesus.expected
```

## 📁 Project Structure

```
├── main.cpp                 # Entry point of the language
├── lexer/
│   ├── lexer.hpp            # Token definitions and lexer interface
│   └── lexer.cpp            # Implementation of the lexer
├── parser
│   ├── parser.hpp           # Parses a list of tokens into an Abstract Syntax Tree (AST)
│   └── parser.cpp
├── ast
│   ├── ast_node.hpp          # Abstract base class for all nodes
│   ├── identifier_node.hpp   # AST node representing an identifier (e.g., variable name)
│   ├── let_there_be_node.hpp # AST node representing a variable declaration and assignment.
│   └── value_node.hpp        # AST node representing a literal value.
├── spirit
│   ├── heart.hpp             # The "Symbol table"
│   └── heart.cpp

```

## 🙏 Purpose

This project is not just about building a programming language — it's about creating with intention, exploring truth, and doing everything as an act of worship.

> _“Whatever you do, do it all for the glory of God.” — 1 Corinthians 10:31_

---

Feel free to build, experiment, and expand. May this be a small tool used for a greater purpose.

## 🕊️ A Note on the Embedded Bible Files

The *Jesus Programming Language* was created to glorify Jesus Christ, the Word of God.
As part of this mission, the the Holy Bible in English and Spanish are included under [src/bible/data/en/](src/bible/data/en/) and [src/bible/data/es/](src/bible/data/es/).

These files are not just data — they are core to the purpose and identity of this project.

While the MIT License permits full modification and redistribution, **we humbly ask that you do not remove or alter these Scripture files** in derived versions,
so that the message of God’s Word may continue to reach hearts through this tool.

> "So is my word that goes out from my mouth:
> It will not return to me empty,
> but will accomplish what I desire
> and achieve the purpose for which I sent it."
— Isaiah 55:11
