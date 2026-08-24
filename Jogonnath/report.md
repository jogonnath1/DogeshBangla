# DogeshBangla — Final Report
## CSE-4114 Compiler Design and Construction Sessional

**Team Project | Compiler + Report + Presentation**

---

## Section 1: The Pitch — Why DogeshBangla Matters

### 1.1 Introduction

Bangladesh has over 170 million people, and the vast majority communicate, think, and reason in Bengali (Bangla). Yet every programming language students learn — C, Python, Java — forces them to think in English syntax before they can express computational logic. This creates a dual cognitive burden: students must simultaneously learn programming concepts *and* an unfamiliar foreign vocabulary.

**DogeshBangla** is a toy programming language designed to remove that barrier. By expressing fundamental programming constructs using romanized Bangla keywords, DogeshBangla allows Bangladeshi students to focus on learning *how to think computationally* — without the added friction of a foreign vocabulary.

---

### 1.2 Real-World Relevance

**Target Audience:** Primary and secondary school students in Bangladesh (ages 10–16) who are learning programming for the first time.

**The Problem DogeshBangla Solves:**

Most introductory programming materials in Bangladesh are in English. When a student reads `while (condition)`, they must translate "while" → "যতক্ষণ" in their head before they understand what the loop does. DogeshBangla eliminates this translation step.

| English Keyword | DogeshBangla | Student's Natural Thought |
|-----------------|--------------|--------------------------|
| `if`            | `যদি`       | "যদি x boro hoy..."     |
| `else`          | `নাহলে`     | "নাহলে..."              |
| `while`         | `যতক্ষণ`   | "যতক্ষণ শেষ na hoy..." |
| `print`         | `দেখাও`     | "দেখাও ongsho"          |

This is the same pedagogical principle behind MIT's Scratch — make the language feel natural for the learner. DogeshBangla does for Bangla-speaking students what Scratch did for visual learners.

---

### 1.3 Problem-Solving: What Gaps Does DogeshBangla Address?

1. **Language Barrier in CS Education:** First-generation learners in rural Bangladesh struggle with English syntax. DogeshBangla gives them a path to learn logic-first.
2. **No Existing Bangla-Syntax Language:** While there are Bangla IDE tools, there is no formally defined, compilable programming language with Bangla-inspired syntax.
3. **Motivation and Engagement:** Students engage more when code "reads" like their native language. DogeshBangla can spark initial interest before transitioning students to mainstream languages.

---

### 1.4 Long-Term Vision

The long-term vision for DogeshBangla is to become a **nationally recognized introductory programming language** used in the Bangladeshi national curriculum for grades 6–8. A partnership with the National Curriculum and Textbook Board (NCTB) could integrate DogeshBangla into computer science textbooks.

**Vision Milestones:**
1. v1.0 — Core language (current): basic types, arithmetic, IF-ELSE, WHILE, print
2. v2.0 — Add functions, arrays, string type
3. v3.0 — Add a web-based DogeshBangla playground (browser + WASM target)
4. v4.0 — Full Bangla Unicode script support (using actual বাংলা characters)

---

### 1.5 Future Roadmap (Hypothetical Features)

| Feature | Description | Target Version |
|---------|-------------|----------------|
| Functions | `kaj myFunc() শুরু...শেষ` | v2.0 |
| Arrays | `ধরি সংখ্যা[] arr = [1, 2, 3]` | v2.0 |
| String type | `ধরি barna name = "Rahim"` | v2.0 |
| For loop | `jekhane i 1 theke 10` | v2.0 |
| WebAssembly target | Compile to WASM for browser execution | v3.0 |
| Full Bangla Unicode | Use actual বাংলা script for keywords | v4.0 |
| Standard library | Built-in math, string functions | v3.0 |
| DogeshBangla IDE | Browser-based editor + runner | v3.0 |

---

## Section 2: Compiler Design

### 2.1 Architecture Overview

The DogeshBangla compiler is a single-pass, multi-phase compiler implemented in **C++17**. It translates `.db` source files into executable Python (`.py`) files through four sequential phases:

```
Source (.db)
    │
    ▼
[Phase 1] LEXER        → Token stream
    │
    ▼
[Phase 2] PARSER       → Abstract Syntax Tree (AST)
    │
    ▼
[Phase 3] SEMANTIC     → Validated AST + Symbol Table
    │
    ▼
[Phase 4] CODE GEN     → Python source (.py)
```

### 2.2 Phase 1 — Lexical Analysis (Lexer)

**Class:** `Lexer` (lexer.h / lexer.cpp)

The lexer reads the source file character by character and groups characters into **tokens** — the smallest meaningful units of the language.

**Responsibilities:**
- Recognize all 9 DogeshBangla keywords
- Recognize identifiers, integer literals, float literals
- Recognize all operators (+, -, *, /, =, ==, !=, <, >, <=, >=)
- Recognize punctuation: `(`, `)`, `;`
- Skip whitespace and single-line comments (`//`)
- Report unknown character errors without crashing

**Token types produced:** 25 distinct token types (see `token.h`)

### 2.3 Phase 2 — Parsing (Recursive Descent Parser)

**Class:** `Parser` (parser.h / parser.cpp)

The parser consumes the token stream and builds an **Abstract Syntax Tree (AST)** — a tree representation of the program's structure.

**Technique:** Recursive Descent Parsing — each grammar rule is a C++ function.

**Grammar rules implemented:**
- `parseStatement()` — dispatches to specific statement parsers
- `parseDeclaration()` — `ধরি সংখ্যা x = expr;`
- `parseAssignment()` — `x = expr;`
- `parseIf()` — `যদি (...) শুরু...শেষ [নাহলে শুরু...শেষ]`
- `parseWhile()` — `যতক্ষণ (...) শুরু...শেষ`
- `parsePrint()` — `দেখাও expr;`
- `parseBlock()` — `শুরু statements শেষ`
- `parseExpression()` through `parsePrimary()` — expression precedence

**Error Recovery:** Panic-mode recovery — on syntax error, skip tokens until `;` or `শেষ` is found, then resume parsing. This allows multiple errors to be reported in a single compilation run.

### 2.4 Phase 3 — Semantic Analysis

**Class:** `SemanticAnalyzer` (semantic.h / semantic.cpp)

The semantic analyzer walks the AST and checks for logical correctness that cannot be detected by the parser alone.

**Symbol Table:** A hash map (`unordered_map<string, Symbol>`) that stores variable name → (type, declaration line, column).

**Checks performed:**
1. **Undeclared variable usage** — using a variable before `ধরি`
2. **Redeclaration** — declaring the same variable twice
3. **Type mismatch** — assigning `দশমিক` value to `সংখ্যা` variable
4. **Type propagation** — resolving the type of every expression node

**Type Rules:**
- `সংখ্যা op সংখ্যা` → `সংখ্যা`
- `দশমিক op any` → `দশমিক`
- `/` (division) → always `দশমিক`
- Relational operators → `সংখ্যা` (0 or 1)

### 2.5 Phase 4 — Code Generation

**Class:** `CodeGenerator` (codegen.h / codegen.cpp)

The code generator performs an **AST traversal** (visitor pattern) and emits Python source code as a string, then writes it to a `.py` file.

**DogeshBangla → Python mapping:**

| DogeshBangla | Generated Python |
|---|---|
| `ধরি সংখ্যা x = 5;` | `x = int(5)` |
| `ধরি দশমিক y = 3.14;` | `y = float(3.14)` |
| `x = x + 1;` | `x = (x + 1)` |
| `দেখাও x;` | `print(x)` |
| `যদি (x > 5) শুরু...শেষ` | `if (x > 5):\n    ...` |
| `নাহলে শুরু...শেষ` | `else:\n    ...` |
| `যতক্ষণ (x > 0) শুরু...শেষ` | `while (x > 0):\n    ...` |

**Indentation:** The code generator maintains an `indentLevel` counter, increasing it inside blocks and decreasing it on exit, producing correctly-indented Python.

### 2.6 Error Handling Strategy

The `ErrorHandler` class collects all errors across all phases. It never throws exceptions that crash the compiler — instead:
- Errors are stored in a `vector<CompilerError>`
- After each phase, if errors exist, a formatted report is printed and compilation halts
- The compiler always exits cleanly (no null pointer exceptions, no uncaught exceptions)

---

## Section 3: Language Grammar (BNF)

*(See full grammar in `docs/grammar.md`)*

### Summary of Key Rules:

```bnf
<program>          ::= { <statement> }
<statement>        ::= <declaration_stmt> | <assignment_stmt>
                     | <if_stmt> | <while_stmt> | <print_stmt> | <block>
<declaration_stmt> ::= "ধরি" <type> <identifier> "=" <expression> ";"
<type>             ::= "সংখ্যা" | "দশমিক"
<assignment_stmt>  ::= <identifier> "=" <expression> ";"
<if_stmt>          ::= "যদি" "(" <expression> ")" <block>
                     | "যদি" "(" <expression> ")" <block> "নাহলে" <block>
<while_stmt>       ::= "যতক্ষণ" "(" <expression> ")" <block>
<print_stmt>       ::= "দেখাও" <expression> ";"
<block>            ::= "শুরু" { <statement> } "শেষ"
<expression>       ::= <comparison>
<comparison>       ::= <add_sub> { <relop> <add_sub> }
<add_sub>          ::= <mul_div> { ("+" | "-") <mul_div> }
<mul_div>          ::= <unary> { ("*" | "/") <unary> }
<unary>            ::= "-" <unary> | <primary>
<primary>          ::= <integer_literal> | <float_literal>
                     | <identifier> | "(" <expression> ")"
```

## Section 4: Project Requirements Fulfillment (Part 1 & Part 2)

As requested by the course instructor, the minimum acceptable features from the PDF guidelines are divided and fulfilled across two parts:

### Part 1 Features (Fundamentals & Conditionals)
1. **Two data types with type checking:** Implemented. Variables can be declared as `সংখ্যা` (integer) or `দশমিক` (float). The Semantic Analyzer enforces type safety.
2. **Basic arithmetic operations with correct operator precedence:** Implemented. Multiplication (`*`) and division (`/`) bind tighter than addition (`+`) and subtraction (`-`).
3. **Assignment statements:** Implemented. Variables can be reassigned (e.g., `x = 10;`).
4. **Conditional control statement (IF-ELSE):** Implemented using the `যদি (...) শুরু...শেষ নাহলে শুরু...শেষ` syntax.

### Part 2 Features (Looping, Recovery & Target Gen)
1. **Looping control statements (WHILE loop):** Implemented using the `যতক্ষণ (...) শুরু...শেষ` syntax.
2. **Basic syntax error recovery (skip to semicolon or end of line):** Implemented in `Parser::panicMode()`. The parser synchronizes on `;` and `শেষ` tokens to avoid cascading syntax errors.
3. **No runtime crashes:** Implemented. The compiler catches lexical, syntax, and semantic errors gracefully via the `ErrorHandler` and halts safely without SegFaults or unhandled exceptions.
4. **Generation of a valid, executable Java or Python target code file:** Implemented. Generates standard Python 3 output (`.py` files).

---

## Conclusion

DogeshBangla demonstrates that a functional compiler — with lexical analysis, parsing, semantic type-checking, and code generation — can be built in C++ in a semester. More importantly, it demonstrates a real-world use case: lowering the barrier to entry for Bangla-speaking students learning to code for the first time. The language is intentionally simple, pedagogically motivated, and designed to transition learners to mainstream languages as a next step.

