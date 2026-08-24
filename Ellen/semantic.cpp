// =============================================================
//  DogeshBangla Compiler — semantic.cpp
//  Full semantic analysis implementation
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#include "semantic.h"
#include <stdexcept>

SemanticAnalyzer::SemanticAnalyzer(ErrorHandler& errHandler)
    : err(errHandler) {}

const std::unordered_map<std::string, Symbol>& SemanticAnalyzer::getSymbolTable() const {
    return symbolTable;
}

// ------------------------------------------------------------------
// analyze() — entry point
// ------------------------------------------------------------------
void SemanticAnalyzer::analyze(ProgramNode* program) {
    for (auto& stmt : program->statements) {
        analyzeStatement(stmt.get());
    }
}

// ------------------------------------------------------------------
// analyzeStatement — dispatch to correct handler
// ------------------------------------------------------------------
void SemanticAnalyzer::analyzeStatement(ASTNode* node) {
    if (!node) return;

    if (auto* d = dynamic_cast<DeclarationNode*>(node))  { analyzeDeclaration(d); return; }
    if (auto* a = dynamic_cast<AssignmentNode*>(node))   { analyzeAssignment(a);  return; }
    if (auto* i = dynamic_cast<IfNode*>(node))           { analyzeIf(i);          return; }
    if (auto* w = dynamic_cast<WhileNode*>(node))        { analyzeWhile(w);       return; }
    if (auto* p = dynamic_cast<PrintNode*>(node))        { analyzePrint(p);       return; }
    if (auto* b = dynamic_cast<BlockNode*>(node))        { analyzeBlock(b);       return; }

    err.semanticError(node->line, node->col,
        "Unknown AST node type in semantic analysis: " + node->nodeType());
}

// ------------------------------------------------------------------
// analyzeBlock — analyze each statement inside a block
// ------------------------------------------------------------------
void SemanticAnalyzer::analyzeBlock(BlockNode* node) {
    for (auto& stmt : node->statements) {
        analyzeStatement(stmt.get());
    }
}

// ------------------------------------------------------------------
// analyzeDeclaration — dhoro sankhya x = expr;
// ------------------------------------------------------------------
void SemanticAnalyzer::analyzeDeclaration(DeclarationNode* node) {
    // Check for redeclaration
    if (isDeclared(node->name)) {
        err.semanticError(node->line, node->col,
            "Variable '" + node->name + "' is already declared.");
        return;
    }

    // Analyze initializer
    DataType initType = DataType::UNKNOWN;
    if (node->initializer) {
        initType = analyzeExpr(node->initializer.get());
    }

    // Type mismatch check: sankhya cannot hold doshomik literal
    if (node->type == DataType::SANKHYA && initType == DataType::DOSHOMIK) {
        err.semanticError(node->line, node->col,
            "Type mismatch: cannot assign doshomik (float) value to "
            "sankhya (int) variable '" + node->name + "'.");
        return;
    }

    declare(node->name, node->type, node->line, node->col);
}

// ------------------------------------------------------------------
// analyzeAssignment — x = expr;
// ------------------------------------------------------------------
void SemanticAnalyzer::analyzeAssignment(AssignmentNode* node) {
    if (!isDeclared(node->name)) {
        err.semanticError(node->line, node->col,
            "Undeclared variable '" + node->name + "'.");
        return;
    }

    DataType varType  = lookupType(node->name);
    DataType exprType = analyzeExpr(node->value.get());

    // Type mismatch: assigning float into int variable
    if (varType == DataType::SANKHYA && exprType == DataType::DOSHOMIK) {
        err.semanticError(node->line, node->col,
            "Type mismatch: cannot assign doshomik (float) expression to "
            "sankhya (int) variable '" + node->name + "'.");
    }
}

// ------------------------------------------------------------------
// analyzeIf — jodi (cond) block [nahole block]
// ------------------------------------------------------------------
void SemanticAnalyzer::analyzeIf(IfNode* node) {
    analyzeExpr(node->condition.get());

    if (auto* b = dynamic_cast<BlockNode*>(node->thenBlock.get()))
        analyzeBlock(b);

    if (node->elseBlock) {
        if (auto* b = dynamic_cast<BlockNode*>(node->elseBlock.get()))
            analyzeBlock(b);
    }
}

// ------------------------------------------------------------------
// analyzeWhile — jotokhon (cond) block
// ------------------------------------------------------------------
void SemanticAnalyzer::analyzeWhile(WhileNode* node) {
    analyzeExpr(node->condition.get());

    if (auto* b = dynamic_cast<BlockNode*>(node->body.get()))
        analyzeBlock(b);
}

// ------------------------------------------------------------------
// analyzePrint — dekhao expr;
// ------------------------------------------------------------------
void SemanticAnalyzer::analyzePrint(PrintNode* node) {
    analyzeExpr(node->expr.get());
}

// ══════════════════════════════════════════════════════════════════
//  Expression analysis
// ══════════════════════════════════════════════════════════════════

DataType SemanticAnalyzer::analyzeExpr(ASTNode* node) {
    if (!node) return DataType::UNKNOWN;

    if (dynamic_cast<IntLiteralNode*>(node))
        return DataType::SANKHYA;

    if (dynamic_cast<FloatLiteralNode*>(node))
        return DataType::DOSHOMIK;

    if (dynamic_cast<StringLiteralNode*>(node))
        return DataType::BORNA;

    if (auto* n = dynamic_cast<IdentifierNode*>(node))
        return analyzeIdentifier(n);

    if (auto* n = dynamic_cast<BinaryOpNode*>(node))
        return analyzeBinaryOp(n);

    if (auto* n = dynamic_cast<UnaryMinusNode*>(node))
        return analyzeUnaryMinus(n);

    err.semanticError(node->line, node->col,
        "Unknown expression node type: " + node->nodeType());
    return DataType::UNKNOWN;
}

DataType SemanticAnalyzer::analyzeIdentifier(IdentifierNode* node) {
    if (!isDeclared(node->name)) {
        err.semanticError(node->line, node->col,
            "Undeclared variable '" + node->name + "'.");
        return DataType::UNKNOWN;
    }
    DataType t = lookupType(node->name);
    node->resolvedType = t;
    return t;
}

DataType SemanticAnalyzer::analyzeBinaryOp(BinaryOpNode* node) {
    DataType leftType  = analyzeExpr(node->left.get());
    DataType rightType = analyzeExpr(node->right.get());

    // If either operand is float, result is float
    DataType resultType = (leftType == DataType::DOSHOMIK ||
                           rightType == DataType::DOSHOMIK)
                          ? DataType::DOSHOMIK
                          : DataType::SANKHYA;

    // Division always promotes to float in DogeshBangla
    if (node->op == TokenType::SLASH)
        resultType = DataType::DOSHOMIK;

    // Relational operators return int (0 or 1)
    // Relational operators produce a boolean integer result (0 or 1)
    bool isRelOp = (node->op == TokenType::EQ  || node->op == TokenType::NEQ ||
                    node->op == TokenType::LT  || node->op == TokenType::GT  ||
                    node->op == TokenType::LEQ || node->op == TokenType::GEQ);
    if (isRelOp)
        resultType = DataType::SANKHYA;

    node->resolvedType = resultType;
    return resultType;
}

DataType SemanticAnalyzer::analyzeUnaryMinus(UnaryMinusNode* node) {
    DataType t = analyzeExpr(node->operand.get());
    node->resolvedType = t;
    return t;
}

// ══════════════════════════════════════════════════════════════════
//  Symbol table helpers
// ══════════════════════════════════════════════════════════════════

bool SemanticAnalyzer::isDeclared(const std::string& name) const {
    return symbolTable.find(name) != symbolTable.end();
}

void SemanticAnalyzer::declare(const std::string& name, DataType t, int l, int c) {
    symbolTable[name] = {name, t, l, c};
}

DataType SemanticAnalyzer::lookupType(const std::string& name) const {
    auto it = symbolTable.find(name);
    if (it != symbolTable.end()) return it->second.type;
    return DataType::UNKNOWN;
}
