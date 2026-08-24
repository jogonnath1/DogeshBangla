// =============================================================
//  DogeshBangla Compiler — codegen.h
//  Python code generator interface
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#pragma once
#include "ast.h"
#include "error.h"
#include <string>
#include <sstream>

class CodeGenerator {
public:
    CodeGenerator(ErrorHandler& errHandler);

    // Generate Python source code from AST, return as string
    std::string generate(ProgramNode* program);

private:
    ErrorHandler&   err;
    std::ostringstream out;
    int             indentLevel;

    // ── Emit helpers ──────────────────────────────────────────────
    void emit(const std::string& code);
    void emitLine(const std::string& line);
    void indent();
    void dedent();
    std::string getIndent() const;

    // ── Code generation visitors ──────────────────────────────────
    void genStatement(ASTNode* node);
    void genBlock(BlockNode* node);
    void genDeclaration(DeclarationNode* node);
    void genAssignment(AssignmentNode* node);
    void genIf(IfNode* node);
    void genWhile(WhileNode* node);
    void genPrint(PrintNode* node);

    // Expression generation — returns Python expression string
    std::string genExpr(ASTNode* node);
    std::string genBinaryOp(BinaryOpNode* node);
    std::string genUnaryMinus(UnaryMinusNode* node);

    // Operator token → Python operator string
    std::string opToString(TokenType op);
};
