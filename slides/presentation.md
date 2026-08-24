# DogeshBangla — Presentation Slides
## CSE-4114 Compiler Design and Construction Sessional
### "The Pitch: Why DogeshBangla Should Exist"

---

## SLIDE 1 — Title Slide

```
┌────────────────────────────────────────────────────────────────┐
│                                                                │
│          ██████╗  ██████╗  ██████╗ ███████╗███████╗██╗  ██╗  │
│          ██╔══██╗██╔═══██╗██╔════╝ ██╔════╝██╔════╝██║  ██║  │
│          ██║  ██║██║   ██║██║  ███╗█████╗  ███████╗███████║  │
│          ██║  ██║██║   ██║██║   ██║██╔══╝  ╚════██║██╔══██║  │
│          ██████╔╝╚██████╔╝╚██████╔╝███████╗███████║██║  ██║  │
│                                                                │
│              DogeshBangla Programming Language                 │
│                    Compiler v1.0                               │
│                                                                │
│    "Bringing programming to Bangladeshi learners               │
│     in the language they think in."                            │
│                                                                │
│    CSE-4114 | Compiler Design and Construction Sessional       │
│                                                                │
│    Submitted To:                                               │
│    Alian Ahmed Ferdous (Adjunct Lecturer)                      │
└────────────────────────────────────────────────────────────────┘
```

---

## SLIDE 2 — The Problem

### Programming Education in Bangladesh Has a Hidden Barrier

- 🇧🇩 **170 million** people in Bangladesh speak Bangla as their first language
- 📚 Every programming language forces students to think in **English syntax**
- 🧠 This creates a **dual cognitive burden**:
  - Learn programming logic
  - Learn English vocabulary simultaneously

### The Student's Mental Translation Loop:
```
Student thinks in Bangla
        ↓
Must translate to English keyword
        ↓
Writes code in English syntax
        ↓
Result: Confusion, frustration, dropout
```

> **"If a student can't read the code like a sentence, they can't think like a programmer."**

---

## SLIDE 3 — The Solution: DogeshBangla

### A Bangla-First Programming Language

```
English          DogeshBangla      Bangla Meaning
─────────────────────────────────────────────────
if       →       যদি         →    যদি (if/when)
else     →       নাহলে       →    নাহলে (otherwise)
while    →       যতক্ষণ     →    যতক্ষণ (as long as)
print    →       দেখাও       →    দেখাও (show/display)
declare  →       ধরি        →    ধরো (assume/hold)
int      →       সংখ্যা      →    সংখ্যা (number)
float    →       দশমিক     →    দশমিক (decimal)
{ }      →       শুরু/শেষ  →    শুরু/শেষ (begin/end)
```

### A Student Can Now Read Code Like a Bengali Sentence:
```
যদি (bochhor > 18) শুরু
    দেখাও bochhor;
নাহলে শুরু
    দেখাও 0;
শেষ
```
Reads as: **"যদি বছর ১৮ এর বেশি হয়, তাহলে দেখাও, নাহলে ০ দেখাও"**

---

## SLIDE 4 — Real-World Relevance

### Who Benefits from DogeshBangla?

| Audience | Benefit |
|----------|---------|
| Grade 6–8 students | First exposure to coding without language barrier |
| Rural school teachers | Teach CS concepts in Bangla first |
| Non-English speakers | Access to computational thinking |
| CS Education researchers | Study of L1 language effects on programming learning |

### Comparable Successful Models:
- 🇩🇪 **Scratch (MIT)** — visual language for kids, no English required
- 🇮🇩 **Bahasa Python** — Indonesian keyword Python variant
- 🇰🇷 **Korean-syntax educational tools** — used in Korean middle schools

> DogeshBangla applies the same principle to **Bangladesh's 170M Bangla speakers**

---

## SLIDE 5 — Compiler Architecture

### 4-Phase Compilation Pipeline

```
┌──────────────┐
│  Source File  │  hello.db
│  (.db)        │
└──────┬───────┘
       │
       ▼
┌──────────────┐
│   PHASE 1    │  Lexer (lexer.cpp)
│   LEXER      │  → Breaks source into Tokens
│              │  → 25 token types
└──────┬───────┘
       │  Token Stream
       ▼
┌──────────────┐
│   PHASE 2    │  Parser (parser.cpp)
│   PARSER     │  → Builds Abstract Syntax Tree
│              │  → Recursive Descent
│              │  → Panic-Mode Error Recovery
└──────┬───────┘
       │  AST
       ▼
┌──────────────┐
│   PHASE 3    │  SemanticAnalyzer (semantic.cpp)
│   SEMANTIC   │  → Type checking
│   ANALYSIS   │  → Symbol Table
│              │  → Undeclared variable detection
└──────┬───────┘
       │  Validated AST
       ▼
┌──────────────┐
│   PHASE 4    │  CodeGenerator (codegen.cpp)
│   CODE GEN   │  → Emits Python source code
│              │  → Proper indentation
└──────┬───────┘
       │
       ▼
┌──────────────┐
│  Python File  │  hello.py  →  python hello.py
│  (.py)        │
└──────────────┘
```

---

## SLIDE 6 — Live Demo

### DogeshBangla Source Code:
```
// Factorial of 6
ধরি সংখ্যা n      = 6;
ধরি সংখ্যা result = 1;
ধরি সংখ্যা i      = 1;

যতক্ষণ (i <= n) শুরু
    result = result * i;
    i = i + 1;
শেষ

দেখাও result;
```

### Generated Python Output:
```python
# Generated by DogeshBangla Compiler v1.0
n = int(6)
result = int(1)
i = int(1)

while (i <= n):
    result = (result * i)
    i = (i + 1)

print(result)
```

### Running it:
```
$ ./dogeshbangla factorial.db
$ python factorial.py
720
```

---

## SLIDE 7 — Error Handling Demo

### DogeshBangla catches errors gracefully:

**Test: Undeclared variable**
```
দেখাও ajob_variable;
```
```
[SEMANTIC ERROR] Line 1, Col 8: Undeclared variable 'ajob_variable'.
```

**Test: Type mismatch**
```
ধরি সংখ্যা x = 3.14;
```
```
[SEMANTIC ERROR] Line 1, Col 7:
Type mismatch: cannot assign দশমিক (float) value
to সংখ্যা (int) variable 'x'.
```

**Test: Syntax error with recovery**
```
ধরি সংখ্যা x    // missing = and value
ধরি সংখ্যা y = 10;
দেখাও y;
```
```
[SYNTAX ERROR] Line 1, Col 15: Expected '=' after variable name (got 'ধরি')
[DogeshBangla] Continuing after error recovery...
→ y = 10 and print(y) still compiled correctly!
```

---

## SLIDE 8 — Language Features

### All Minimum Requirements Implemented ✓

| Requirement | DogeshBangla Feature | Status |
|-------------|---------------------|--------|
| 2 data types with type checking | `সংখ্যা` (int), `দশমিক` (float) | ✅ |
| Arithmetic with correct precedence | +, -, *, / with (, ) grouping | ✅ |
| Assignment statements | `x = expr;` | ✅ |
| IF-ELSE statement | `যদি...নাহলে` | ✅ |
| WHILE loop | `যতক্ষণ` | ✅ |
| Syntax error recovery | Panic-mode skip to `;` or `শেষ` | ✅ |
| No runtime crashes | Try-catch + error collection | ✅ |
| Valid executable target code | Clean Python output | ✅ |

---

## SLIDE 9 — Future Roadmap

### DogeshBangla v2.0 → v4.0

```
v1.0 (NOW)          v2.0                v3.0               v4.0
─────────────────────────────────────────────────────────────────
✅ Basic types      Functions           WebAssembly        Full Bangla
✅ Arithmetic       Arrays              Browser IDE        Unicode
✅ IF-ELSE          Strings             Standard lib       Mobile app
✅ WHILE            For loops           Online compiler    NCTB textbook
✅ Print            Input               Package manager    integration
✅ Error recovery   Nested functions    Community repo
```

### The Dream: NCTB Integration
> Partner with the **National Curriculum and Textbook Board** to include DogeshBangla in Grade 7 Computer Science textbooks across Bangladesh.

This would give **3+ million students per year** their first programming experience in their native language.

---

## SLIDE 10 — Conclusion

### DogeshBangla in One Sentence:

> **"DogeshBangla is a Bangla-first compiled programming language that lets Bangladeshi students learn computational thinking in the language they already think in — without the cognitive overhead of English syntax."**

### Key Takeaways:
1. ✅ **Real problem** — English barrier in CS education for Bangla speakers
2. ✅ **Working solution** — Full compiler: Lexer → Parser → Semantic → CodeGen
3. ✅ **Proven model** — Scratch, Scratch Jr., regional language CS tools
4. ✅ **Future potential** — NCTB integration, WebAssembly, Unicode Bangla
5. ✅ **All requirements met** — Types, Arithmetic, IF-ELSE, WHILE, Error recovery

### Thank You! প্রশ্ন আছে?
*(Any questions?)*

