// =============================================================
//  DogeshBangla Compiler — parser.h
//  Recursive-descent parser interface
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#pragma once
#include "token.h"
#include "ast.h"
#include "error.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(std::vector<Token> tokens, ErrorHandler& errHandler);

    // Parse all tokens and return the root ProgramNode
    std::unique_ptr<ProgramNode> parse();

private:
    std::vector<Token> tokens;
    ErrorHandler&      err;
    int                current;   // index of current token

    // ── Token navigation ─────────────────────────────────────────
    Token& peek();
    Token& previous();
    Token& advance();
    bool   isAtEnd();
    bool   check(TokenType t);
    bool   match(TokenType t);
    Token  consume(TokenType t, const std::string& errMsg);

    // ── Panic-mode error recovery ─────────────────────────────────
    // Skip tokens until a synchronization point (';' or 'shesh')
    void synchronize();

    // ── Grammar rules (each = one function) ──────────────────────
    ASTNodePtr parseStatement();
    ASTNodePtr parseDeclaration();
    ASTNodePtr parseAssignment();
    ASTNodePtr parseIf();
    ASTNodePtr parseWhile();
    ASTNodePtr parsePrint();
    std::unique_ptr<BlockNode> parseBlock();

    // ── Expression rules (respecting precedence) ──────────────────
    ASTNodePtr parseExpression();   // lowest precedence entry point
    ASTNodePtr parseComparison();   // ==, !=, <, >, <=, >=
    ASTNodePtr parseAddSub();       // +, -
    ASTNodePtr parseMulDiv();       // *, /
    ASTNodePtr parseUnary();        // unary minus
    ASTNodePtr parsePrimary();      // literals, identifiers, grouped expr
};
