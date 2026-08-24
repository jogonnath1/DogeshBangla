// =============================================================
//  DogeshBangla Compiler — ast.h
//  Abstract Syntax Tree node definitions
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#pragma once
#include "token.h"
#include <string>
#include <vector>
#include <memory>

// ------------------------------------------------------------------
// Data type enum (mirrors DogeshBangla type keywords)
// ------------------------------------------------------------------
enum class DataType {
    SANKHYA,   // integer
    DOSHOMIK,  // float
    BORNA,     // string
    UNKNOWN    // not yet resolved / error state
};

inline std::string dataTypeToString(DataType dt) {
    switch (dt) {
        case DataType::SANKHYA:  return "sankhya (int)";
        case DataType::DOSHOMIK: return "doshomik (float)";
        case DataType::BORNA:    return "borna (string)";
        default:                 return "unknown";
    }
}

// ------------------------------------------------------------------
// Forward declarations
// ------------------------------------------------------------------
struct ASTNode;
using ASTNodePtr = std::unique_ptr<ASTNode>;

// ------------------------------------------------------------------
// Base AST node
// ------------------------------------------------------------------
struct ASTNode {
    int line = 0, col = 0;
    virtual ~ASTNode() = default;
    virtual std::string nodeType() const = 0;
};

// ══════════════════════════════════════════════════════════════════
//  Expression nodes
// ══════════════════════════════════════════════════════════════════

// Integer literal  e.g. 42
struct IntLiteralNode : ASTNode {
    int value;
    IntLiteralNode(int v, int l, int c) : value(v) { line=l; col=c; }
    std::string nodeType() const override { return "IntLiteral"; }
};

// Float literal  e.g. 3.14
struct FloatLiteralNode : ASTNode {
    double value;
    FloatLiteralNode(double v, int l, int c) : value(v) { line=l; col=c; }
    std::string nodeType() const override { return "FloatLiteral"; }
};

// String literal  e.g. "hello world"
struct StringLiteralNode : ASTNode {
    std::string value;
    StringLiteralNode(std::string v, int l, int c) : value(std::move(v)) { line=l; col=c; }
    std::string nodeType() const override { return "StringLiteral"; }
};

// Variable reference  e.g. x
struct IdentifierNode : ASTNode {
    std::string name;
    DataType    resolvedType = DataType::UNKNOWN;  // filled by semantic pass
    IdentifierNode(std::string n, int l, int c) : name(std::move(n)) { line=l; col=c; }
    std::string nodeType() const override { return "Identifier"; }
};

// Binary operation  e.g. x + 3, a * b, x == 5
struct BinaryOpNode : ASTNode {
    TokenType   op;   // PLUS, MINUS, STAR, SLASH, EQ, NEQ, LT, GT, LEQ, GEQ
    ASTNodePtr  left;
    ASTNodePtr  right;
    DataType    resolvedType = DataType::UNKNOWN;  // filled by semantic pass

    BinaryOpNode(TokenType o, ASTNodePtr l, ASTNodePtr r, int ln, int c)
        : op(o), left(std::move(l)), right(std::move(r)) { line=ln; col=c; }
    std::string nodeType() const override { return "BinaryOp"; }
};

// Unary minus  e.g. -x
struct UnaryMinusNode : ASTNode {
    ASTNodePtr  operand;
    DataType    resolvedType = DataType::UNKNOWN;

    UnaryMinusNode(ASTNodePtr o, int l, int c) : operand(std::move(o)) { line=l; col=c; }
    std::string nodeType() const override { return "UnaryMinus"; }
};

// ══════════════════════════════════════════════════════════════════
//  Statement nodes
// ══════════════════════════════════════════════════════════════════

// Block  shuru ... shesh
struct BlockNode : ASTNode {
    std::vector<ASTNodePtr> statements;
    BlockNode(int l, int c) { line=l; col=c; }
    std::string nodeType() const override { return "Block"; }
};

// Variable declaration  dhoro sankhya x = expr ;
struct DeclarationNode : ASTNode {
    DataType    type;
    std::string name;
    ASTNodePtr  initializer;  // can be nullptr for future extension

    DeclarationNode(DataType t, std::string n, ASTNodePtr init, int l, int c)
        : type(t), name(std::move(n)), initializer(std::move(init)) { line=l; col=c; }
    std::string nodeType() const override { return "Declaration"; }
};

// Assignment  x = expr ;
struct AssignmentNode : ASTNode {
    std::string name;
    ASTNodePtr  value;

    AssignmentNode(std::string n, ASTNodePtr v, int l, int c)
        : name(std::move(n)), value(std::move(v)) { line=l; col=c; }
    std::string nodeType() const override { return "Assignment"; }
};

// If-Else  jodi (cond) block  [nahole block]
struct IfNode : ASTNode {
    ASTNodePtr condition;
    ASTNodePtr thenBlock;
    ASTNodePtr elseBlock;   // nullptr if no nahole

    IfNode(ASTNodePtr cond, ASTNodePtr thenB, ASTNodePtr elseB, int l, int c)
        : condition(std::move(cond)), thenBlock(std::move(thenB)),
          elseBlock(std::move(elseB)) { line=l; col=c; }
    std::string nodeType() const override { return "If"; }
};

// While  jotokhon (cond) block
struct WhileNode : ASTNode {
    ASTNodePtr condition;
    ASTNodePtr body;

    WhileNode(ASTNodePtr cond, ASTNodePtr body, int l, int c)
        : condition(std::move(cond)), body(std::move(body)) { line=l; col=c; }
    std::string nodeType() const override { return "While"; }
};

// Print  dekhao expr ;
struct PrintNode : ASTNode {
    ASTNodePtr expr;
    PrintNode(ASTNodePtr e, int l, int c) : expr(std::move(e)) { line=l; col=c; }
    std::string nodeType() const override { return "Print"; }
};

// Program — root node
struct ProgramNode : ASTNode {
    std::vector<ASTNodePtr> statements;
    ProgramNode() { line=1; col=1; }
    std::string nodeType() const override { return "Program"; }
};
