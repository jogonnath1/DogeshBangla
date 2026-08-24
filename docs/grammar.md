# DogeshBangla — Complete Language Grammar (BNF)
## CSE-4114 Compiler Design and Construction Sessional

---

## Grammar Notation

- `::=` — is defined as
- `|` — or (alternative)
- `[ ]` — optional (zero or one occurrence)
- `{ }` — repetition (zero or more occurrences)
- `" "` — terminal (literal keyword/symbol)

---

## 1. Program Structure

```bnf
<program>        ::= { <statement> }

<statement>      ::= <declaration_stmt>
                   | <assignment_stmt>
                   | <if_stmt>
                   | <while_stmt>
                   | <print_stmt>
                   | <block>
```

---

## 2. Statement Rules

### 2.1 Variable Declaration
```bnf
<declaration_stmt> ::= "ধরি" <type> <identifier> "=" <expression> ";"

<type>             ::= "সংখ্যা"
                     | "দশমিক"
```

### 2.2 Assignment Statement
```bnf
<assignment_stmt>  ::= <identifier> "=" <expression> ";"
```

### 2.3 If-Else Statement
```bnf
<if_stmt>          ::= "যদি" "(" <expression> ")" <block>
                     | "যদি" "(" <expression> ")" <block> "নাহলে" <block>
```

### 2.4 While Statement
```bnf
<while_stmt>       ::= "যতক্ষণ" "(" <expression> ")" <block>
```

### 2.5 Print Statement
```bnf
<print_stmt>       ::= "দেখাও" <expression> ";"
```

### 2.6 Block
```bnf
<block>            ::= "শুরু" { <statement> } "শেষ"
```

---

## 3. Expression Rules (Precedence Low to High)

```bnf
<expression>       ::= <comparison>

<comparison>       ::= <add_sub>
                     | <comparison> <relop> <add_sub>

<add_sub>          ::= <mul_div>
                     | <add_sub> "+" <mul_div>
                     | <add_sub> "-" <mul_div>

<mul_div>          ::= <unary>
                     | <mul_div> "*" <unary>
                     | <mul_div> "/" <unary>

<unary>            ::= <primary>
                     | "-" <unary>

<primary>          ::= <integer_literal>
                     | <float_literal>
                     | <identifier>
                     | "(" <expression> ")"
```

---

## 4. Operators

### 4.1 Relational Operators
```bnf
<relop>            ::= "=="
                     | "!="
                     | "<"
                     | ">"
                     | "<="
                     | ">="
```

### 4.2 Arithmetic Operators (in order of precedence)

| Level | Operator | Associativity |
|-------|----------|---------------|
| 1 (lowest) | +, - | Left |
| 2 | *, / | Left |
| 3 (highest) | Unary - | Right |

---

## 5. Lexical Rules

### 5.1 Literals
```bnf
<integer_literal>      ::= <digit> { <digit> }
<float_literal>        ::= <digit> { <digit> } "." <digit> { <digit> }
<digit>                ::= "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9"
```

### 5.2 Identifiers
```bnf
<identifier>           ::= <letter_or_underscore> { <letter_or_underscore> | <digit> }
<letter_or_underscore> ::= "a".."z" | "A".."Z" | "_"
```

### 5.3 Keywords (Reserved)

| Keyword     | Role         |
|-------------|--------------|
| `ধরি`     | Declaration  |
| `সংখ্যা`   | Integer type |
| `দশমিক`  | Float type   |
| `যদি`      | If           |
| `নাহলে`    | Else         |
| `যতক্ষণ`  | While        |
| `দেখাও`    | Print        |
| `শুরু`     | Block start  |
| `শেষ`     | Block end    |

### 5.4 Comments
```bnf
<comment>  ::= "//" { any character except newline } newline
```

---

## 6. Type System Rules

| Operation | Left Type | Right Type | Result Type |
|-----------|-----------|------------|-------------|
| +, -, * | সংখ্যা | সংখ্যা | সংখ্যা |
| +, -, * | দশমিক | any | দশমিক |
| / | any | any | দশমিক |
| Relational | any | any | সংখ্যা (0 or 1) |

**Type Mismatch Rules:**
- Assigning `দশমিক` value to `সংখ্যা` variable  → SEMANTIC ERROR
- Using undeclared variable                          → SEMANTIC ERROR
- Re-declaring an existing variable                  → SEMANTIC ERROR

---

## 7. Error Recovery Rule

On a syntax error, the parser uses panic-mode recovery:
> Skip tokens until a synchronization token is found: `;` or `শেষ`

```bnf
<sync_token> ::= ";" | "শেষ"
```

