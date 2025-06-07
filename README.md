# Jesus Language

> _“In the beginning was the Word, and the Word was with God, and the Word was God.” — John 1:1_

**Jesus** is a minimal programming language designed to explore clarity, structure, and purpose in code — all for the glory of God.

This repository currently contains the basic lexer (lexical analyzer) and REPL loop for interacting with the language.

## 🛠 Compilation

To build and run the project, make sure you have a C++ compiler (like `g++`) installed.

Compile using the following command:

```bash
cd src/jesus

g++ main.cpp lexer/lexer.cpp parser/parser.cpp spirit/heart.cpp parser/expression_parser.cpp -I . -o jesus
```

Then run:

```bash
./jesus
```

You will see the REPL prompt:

```
(Jesus)
```

Type a line and see the tokens printed as output.

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
