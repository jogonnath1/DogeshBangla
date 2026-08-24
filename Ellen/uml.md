# DogeshBangla Compiler — UML Class Diagrams
## CSE-4114 Compiler Design and Construction Sessional

---

## 1. Overall System Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     DogeshBangla Compiler                       │
│                                                                 │
│  Source File (.db)                                              │
│       │                                                         │
│       ▼                                                         │
│  ┌─────────┐    tokens    ┌─────────┐    AST    ┌───────────┐  │
│  │  Lexer  │ ──────────► │ Parser  │ ─────────► │ Semantic  │  │
│  └─────────┘             └─────────┘            │ Analyzer  │  │
│       │                       │                 └───────────┘  │
│       │                       │                       │        │
│       ▼                       ▼                       ▼        │
│  ┌─────────────────────────────────────────────────────────┐   │
│  │                    ErrorHandler                         │   │
│  └─────────────────────────────────────────────────────────┘   │
│                                                 │              │
│                                                 ▼              │
│                                          ┌───────────┐         │
│                                          │ CodeGen   │         │
│                                          └───────────┘         │
│                                                 │              │
│                                                 ▼              │
│                                        Python Output (.py)     │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Token Class

```
┌──────────────────────────────┐
│           Token              │
├──────────────────────────────┤
│ + type    : TokenType        │
│ + value   : string           │
│ + line    : int              │
│ + col     : int              │
├──────────────────────────────┤
│ + is(TokenType) : bool       │
│ + isNot(TokenType) : bool    │
│ + isEOF() : bool             │
│ + isLiteral() : bool         │
│ + isType() : bool            │
│ + isRelOp() : bool           │
│ + toString() : string        │
└──────────────────────────────┘

<<enumeration>> TokenType
─────────────────────────
ধরি, সংখ্যা, দশমিক,
যদি, নাহলে, যতক্ষণ,
দেখাও, শুরু, শেষ,
INTEGER_LIT, FLOAT_LIT,
IDENTIFIER,
PLUS, MINUS, STAR, SLASH,
ASSIGN, EQ, NEQ, LT, GT, LEQ, GEQ,
LPAREN, RPAREN, SEMICOLON,
EOF_TOKEN, UNKNOWN
```

---

## 3. ErrorHandler Class

```
┌──────────────────────────────────────────┐
│               ErrorHandler               │
├──────────────────────────────────────────┤
│ + errors   : vector<CompilerError>       │
│ + hadError : bool                        │
├──────────────────────────────────────────┤
│ + lexicalError(line, col, msg) : void    │
│ + syntaxError(line, col, msg)  : void    │
│ + semanticError(line, col, msg): void    │
│ + printAll()                   : void    │
│ + hasErrors()                  : bool    │
│ + clear()                      : void    │
└──────────────────────────────────────────┘

┌──────────────────────────────────────────┐
│             CompilerError                │
├──────────────────────────────────────────┤
│ + severity : ErrorSeverity               │
│ + message  : string                      │
│ + line     : int                         │
│ + col      : int                         │
├──────────────────────────────────────────┤
│ + toString() : string                    │
└──────────────────────────────────────────┘

<<enumeration>> ErrorSeverity
─────────────────────────────
LEXICAL, SYNTAX, SEMANTIC
```

---

## 4. Lexer Class

```
┌─────────────────────────────────────────────────────┐
│                       Lexer                         │
├─────────────────────────────────────────────────────┤
│ - src      : string                                 │
│ - err      : ErrorHandler&                          │
│ - pos      : int                                    │
│ - line     : int                                    │
│ - col      : int                                    │
│ - KEYWORDS : map<string, TokenType>   [static]      │
├─────────────────────────────────────────────────────┤
│ + Lexer(source, errHandler)                         │
│ + tokenize() : vector<Token>                        │
│ - peek()     : char                                 │
│ - peekNext() : char                                 │
│ - advance()  : char                                 │
│ - isAtEnd()  : bool                                 │
│ - match(char): bool                                 │
│ - nextToken()                    : Token            │
│ - scanNumber()                   : Token            │
│ - scanIdentifierOrKeyword()      : Token            │
│ - skipWhitespaceAndComments()    : void             │
└─────────────────────────────────────────────────────┘

Lexer ──────────────────────────────> ErrorHandler
       uses (reports lexical errors)
```

---

## 5. AST Node Hierarchy

```
                        ┌───────────┐
                        │  ASTNode  │  (abstract)
                        │───────────│
                        │ line: int │
                        │ col : int │
                        │───────────│
                        │nodeType() │
                        └─────┬─────┘
                              │
              ┌───────────────┼───────────────────┐
              │               │                   │
    ┌─────────────┐   ┌────────────────┐  ┌──────────────┐
    │ ProgramNode │   │  Expression    │  │  Statement   │
    │─────────────│   │    Nodes       │  │    Nodes     │
    │ statements  │   └───────┬────────┘  └──────┬───────┘
    └─────────────┘           │                  │
                    ┌─────────┼──────────┐        │
                    │         │          │        │
             ┌──────────┐ ┌───────┐ ┌──────────┐ │
             │IntLiteral│ │Float  │ │Identifier│ │
             │──────────│ │Literal│ │──────────│ │
             │value:int │ │───────│ │name      │ │
             └──────────┘ │value  │ │resolved  │ │
                          │:double│ │Type      │ │
                          └───────┘ └──────────┘ │
                                                 │
                    ┌────────────────────┬────────┼────────────┬────────────┐
                    │                    │        │            │            │
             ┌────────────┐  ┌────────────┐ ┌─────────┐ ┌─────────┐ ┌──────────┐
             │BinaryOpNode│  │UnaryMinus  │ │Declarat-│ │Assignm- │ │ IfNode   │
             │────────────│  │────────────│ │ionNode  │ │entNode  │ │──────────│
             │op:TokenType│  │operand:Node│ │─────────│ │─────────│ │condition │
             │left:Node   │  │resolvedType│ │type:Data│ │name     │ │thenBlock │
             │right:Node  │  └────────────┘ │Type     │ │value    │ │elseBlock │
             │resolvedType│                 │name     │ └─────────┘ └──────────┘
             └────────────┘                 │init:Node│
                                            └─────────┘

    ┌────────────┐  ┌───────────┐  ┌───────────────────┐
    │ WhileNode  │  │ PrintNode │  │    BlockNode       │
    │────────────│  │───────────│  │───────────────────│
    │ condition  │  │ expr:Node │  │statements:         │
    │ body:Block │  └───────────┘  │  vector<ASTNode>  │
    └────────────┘                 └───────────────────┘
```

---

## 6. Parser Class

```
┌──────────────────────────────────────────────────────┐
│                       Parser                         │
├──────────────────────────────────────────────────────┤
│ - tokens  : vector<Token>                            │
│ - err     : ErrorHandler&                            │
│ - current : int                                      │
├──────────────────────────────────────────────────────┤
│ + Parser(tokens, errHandler)                         │
│ + parse() : unique_ptr<ProgramNode>                  │
│ - peek()                   : Token&                  │
│ - previous()               : Token&                  │
│ - advance()                : Token&                  │
│ - isAtEnd()                : bool                    │
│ - check(TokenType)         : bool                    │
│ - match(TokenType)         : bool                    │
│ - consume(type, msg)       : Token                   │
│ - synchronize()            : void    [error recovery]│
│ - parseStatement()         : ASTNodePtr              │
│ - parseDeclaration()       : ASTNodePtr              │
│ - parseAssignment()        : ASTNodePtr              │
│ - parseIf()                : ASTNodePtr              │
│ - parseWhile()             : ASTNodePtr              │
│ - parsePrint()             : ASTNodePtr              │
│ - parseBlock()             : unique_ptr<BlockNode>   │
│ - parseExpression()        : ASTNodePtr              │
│ - parseComparison()        : ASTNodePtr              │
│ - parseAddSub()            : ASTNodePtr              │
│ - parseMulDiv()            : ASTNodePtr              │
│ - parseUnary()             : ASTNodePtr              │
│ - parsePrimary()           : ASTNodePtr              │
└──────────────────────────────────────────────────────┘

Parser ─────────────────────────> ErrorHandler
Parser ─── produces ────────────> ProgramNode (AST Root)
```

---

## 7. SemanticAnalyzer Class

```
┌──────────────────────────────────────────────────────┐
│                  SemanticAnalyzer                    │
├──────────────────────────────────────────────────────┤
│ - err         : ErrorHandler&                        │
│ - symbolTable : map<string, Symbol>                  │
├──────────────────────────────────────────────────────┤
│ + SemanticAnalyzer(errHandler)                       │
│ + analyze(ProgramNode*)      : void                  │
│ + getSymbolTable()           : map<string,Symbol>&   │
│ - analyzeStatement(ASTNode*) : void                  │
│ - analyzeBlock(BlockNode*)   : void                  │
│ - analyzeDeclaration(...)    : void                  │
│ - analyzeAssignment(...)     : void                  │
│ - analyzeIf(IfNode*)         : void                  │
│ - analyzeWhile(WhileNode*)   : void                  │
│ - analyzePrint(PrintNode*)   : void                  │
│ - analyzeExpr(ASTNode*)      : DataType              │
│ - analyzeBinaryOp(...)       : DataType              │
│ - analyzeUnaryMinus(...)     : DataType              │
│ - analyzeIdentifier(...)     : DataType              │
│ - isDeclared(name)           : bool                  │
│ - declare(name, type, l, c)  : void                  │
│ - lookupType(name)           : DataType              │
└──────────────────────────────────────────────────────┘

┌────────────────────────┐
│        Symbol          │
├────────────────────────┤
│ + name     : string    │
│ + type     : DataType  │
│ + declLine : int       │
│ + declCol  : int       │
└────────────────────────┘

SemanticAnalyzer ──── uses ────> ErrorHandler
SemanticAnalyzer ──── reads ───> ProgramNode (AST)
SemanticAnalyzer ──── builds ──> SymbolTable (map<string, Symbol>)
```

---

## 8. CodeGenerator Class

```
┌──────────────────────────────────────────────────────┐
│                   CodeGenerator                      │
├──────────────────────────────────────────────────────┤
│ - err         : ErrorHandler&                        │
│ - out         : ostringstream                        │
│ - indentLevel : int                                  │
├──────────────────────────────────────────────────────┤
│ + CodeGenerator(errHandler)                          │
│ + generate(ProgramNode*) : string                    │
│ - emit(code)             : void                      │
│ - emitLine(line)         : void                      │
│ - indent()               : void                      │
│ - dedent()               : void                      │
│ - getIndent()            : string                    │
│ - genStatement(ASTNode*) : void                      │
│ - genBlock(BlockNode*)   : void                      │
│ - genDeclaration(...)    : void                      │
│ - genAssignment(...)     : void                      │
│ - genIf(IfNode*)         : void                      │
│ - genWhile(WhileNode*)   : void                      │
│ - genPrint(PrintNode*)   : void                      │
│ - genExpr(ASTNode*)      : string                    │
│ - genBinaryOp(...)       : string                    │
│ - genUnaryMinus(...)     : string                    │
│ - opToString(TokenType)  : string                    │
└──────────────────────────────────────────────────────┘

CodeGenerator ──── reads ────> ProgramNode (AST)
CodeGenerator ──── uses  ────> ErrorHandler
CodeGenerator ──── emits ────> Python source code (string)
```

---

## 9. Component Relationships Summary

```
        ┌──────────┐
        │  main()  │
        └────┬─────┘
             │ creates and calls in sequence
             ▼
    ┌────────────────┐
    │  ErrorHandler  │◄───────────────────────────────┐
    └────────────────┘                                │
             │ shared by all phases                   │
    ┌────────┴──────────────────────────────────┐     │
    │                                           │     │
    ▼                                           ▼     │
┌──────┐  tokens   ┌────────┐  AST  ┌──────────┐     │
│Lexer │──────────►│Parser  │──────►│Semantic  │─────┘
└──────┘           └────────┘       │Analyzer  │
                                    └─────┬────┘
                                          │ validated AST
                                          ▼
                                    ┌──────────┐
                                    │CodeGen   │──► Python code
                                    └──────────┘
```

