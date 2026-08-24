// =============================================================
//  DogeshBangla Compiler — semantic.h
//  Semantic analyzer + symbol table interface
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#pragma once
#include "ast.h"
#include "error.h"
#include <unordered_map>
#include <string>

// ------------------------------------------------------------------
// Symbol table entry
// ------------------------------------------------------------------
struct Symbol {
    std::string name;
    DataType    type;
    int         declLine;
    int         declCol;
};

// ------------------------------------------------------------------
// SemanticAnalyzer — walks the AST, checks types, fills symbol table
// ------------------------------------------------------------------
class SemanticAnalyzer {
public:
    SemanticAnalyzer(ErrorHandler& errHandler);

    // Run the full semantic pass on a program node
    void analyze(ProgramNode* program);

    // Public symbol table (for use by code generator)
    const std::unordered_map<std::string, Symbol>& getSymbolTable() const;

private:
    ErrorHandler& err;
    std::unordered_map<std::string, Symbol> symbolTable;

    // ── Visitor methods ───────────────────────────────────────────
    void  analyzeStatement(ASTNode* node);
    void  analyzeBlock(BlockNode* node);
    void  analyzeDeclaration(DeclarationNode* node);
    void  analyzeAssignment(AssignmentNode* node);
    void  analyzeIf(IfNode* node);
    void  analyzeWhile(WhileNode* node);
    void  analyzePrint(PrintNode* node);

    // Expression analysis returns the resolved DataType
    DataType analyzeExpr(ASTNode* node);
    DataType analyzeBinaryOp(BinaryOpNode* node);
    DataType analyzeUnaryMinus(UnaryMinusNode* node);
    DataType analyzeIdentifier(IdentifierNode* node);

    // ── Helpers ───────────────────────────────────────────────────
    bool  isDeclared(const std::string& name) const;
    void  declare(const std::string& name, DataType t, int l, int c);
    DataType lookupType(const std::string& name) const;
};
