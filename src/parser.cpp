// =============================================================
//  DogeshBangla Compiler — parser.cpp
//  Full recursive-descent parser implementation
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#include "parser.h"
#include <stdexcept>

// ------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------
Parser::Parser(std::vector<Token> toks, ErrorHandler& errHandler)
    : tokens(std::move(toks)), err(errHandler), current(0) {}

// ------------------------------------------------------------------
// parse() — entry point, parse entire program
// ------------------------------------------------------------------
std::unique_ptr<ProgramNode> Parser::parse() {
    auto program = std::make_unique<ProgramNode>();
    while (!isAtEnd()) {
        try {
            auto stmt = parseStatement();
            if (stmt) program->statements.push_back(std::move(stmt));
        } catch (...) {
            // Panic-mode recovery: skip to synchronization point
            synchronize();
        }
    }
    return program;
}

// ------------------------------------------------------------------
// Token navigation helpers
// ------------------------------------------------------------------
Token& Parser::peek()     { return tokens[current]; }
Token& Parser::previous() { return tokens[current - 1]; }
bool   Parser::isAtEnd()  { return peek().type == TokenType::EOF_TOKEN; }

Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool Parser::check(TokenType t) {
    if (isAtEnd()) return false;
    return peek().type == t;
}

bool Parser::match(TokenType t) {
    if (check(t)) { advance(); return true; }
    return false;
}

Token Parser::consume(TokenType t, const std::string& msg) {
    if (check(t)) return advance();
    Token& tok = peek();
    err.syntaxError(tok.line, tok.col, msg + " (got '" + tok.value + "')");
    throw std::runtime_error("syntax error");
}

// ------------------------------------------------------------------
// synchronize() — Panic-mode recovery
// Skip tokens until ';' or 'shesh', then continue parsing
// ------------------------------------------------------------------
void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        // Stop at a statement boundary
        if (previous().type == TokenType::SEMICOLON) return;
        if (previous().type == TokenType::SHESH)     return;

        switch (peek().type) {
            case TokenType::DHORO:
            case TokenType::JODI:
            case TokenType::JOTOKHON:
            case TokenType::DEKHAO:
            case TokenType::SHURU:
            case TokenType::SHESH:
                return;
            default:
                break;
        }
        advance();
    }
}

// ------------------------------------------------------------------
// parseStatement — decide which kind of statement to parse
// ------------------------------------------------------------------
ASTNodePtr Parser::parseStatement() {
    if (check(TokenType::DHORO))    return parseDeclaration();
    if (check(TokenType::JODI))     return parseIf();
    if (check(TokenType::JOTOKHON)) return parseWhile();
    if (check(TokenType::DEKHAO))   return parsePrint();
    if (check(TokenType::SHURU))    return parseBlock();

    // Must be an assignment: identifier = expr ;
    if (check(TokenType::IDENTIFIER)) return parseAssignment();

    Token& tok = peek();
    err.syntaxError(tok.line, tok.col,
        "Unexpected token '" + tok.value + "'. Expected a statement.");
    throw std::runtime_error("syntax error");
}

// ------------------------------------------------------------------
// parseDeclaration — dhoro <type> <id> = <expr> ;
// ------------------------------------------------------------------
ASTNodePtr Parser::parseDeclaration() {
    Token dhoroTok = consume(TokenType::DHORO, "Expected 'dhoro'");

    // Type keyword
    DataType dtype = DataType::UNKNOWN;
    if (match(TokenType::SANKHYA))       dtype = DataType::SANKHYA;
    else if (match(TokenType::DOSHOMIK)) dtype = DataType::DOSHOMIK;
    else {
        Token& t = peek();
        err.syntaxError(t.line, t.col,
            "Expected type keyword 'sankhya' or 'doshomik' after 'dhoro'");
        throw std::runtime_error("syntax error");
    }

    Token nameTok = consume(TokenType::IDENTIFIER,
                            "Expected variable name after type");
    consume(TokenType::ASSIGN, "Expected '=' after variable name");

    auto initExpr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after declaration");

    return std::make_unique<DeclarationNode>(
        dtype, nameTok.value, std::move(initExpr),
        dhoroTok.line, dhoroTok.col);
}

// ------------------------------------------------------------------
// parseAssignment — <id> = <expr> ;
// ------------------------------------------------------------------
ASTNodePtr Parser::parseAssignment() {
    Token nameTok = consume(TokenType::IDENTIFIER, "Expected variable name");
    consume(TokenType::ASSIGN, "Expected '=' in assignment");
    auto val = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after assignment");
    return std::make_unique<AssignmentNode>(
        nameTok.value, std::move(val), nameTok.line, nameTok.col);
}

// ------------------------------------------------------------------
// parseIf — jodi ( <cond> ) <block> [ nahole <block> ]
// ------------------------------------------------------------------
ASTNodePtr Parser::parseIf() {
    Token jodiTok = consume(TokenType::JODI, "Expected 'jodi'");
    consume(TokenType::LPAREN, "Expected '(' after 'jodi'");
    auto cond = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    auto thenBlock = parseBlock();

    ASTNodePtr elseBlock = nullptr;
    if (match(TokenType::NAHOLE)) {
        elseBlock = parseBlock();
    }

    return std::make_unique<IfNode>(
        std::move(cond), std::move(thenBlock), std::move(elseBlock),
        jodiTok.line, jodiTok.col);
}

// ------------------------------------------------------------------
// parseWhile — jotokhon ( <cond> ) <block>
// (PART 2 FEATURE: Looping control statements)
// ------------------------------------------------------------------
ASTNodePtr Parser::parseWhile() {
    Token tok = consume(TokenType::JOTOKHON, "Expected 'jotokhon'");
    consume(TokenType::LPAREN, "Expected '(' after 'jotokhon'");
    auto cond = parseExpression();
    consume(TokenType::RPAREN, "Expected ')' after condition");
    auto body = parseBlock();

    return std::make_unique<WhileNode>(
        std::move(cond), std::move(body), tok.line, tok.col);
}

// ------------------------------------------------------------------
// parsePrint — dekhao <expr> ;
// ------------------------------------------------------------------
ASTNodePtr Parser::parsePrint() {
    Token tok = consume(TokenType::DEKHAO, "Expected 'dekhao'");
    auto expr = parseExpression();
    consume(TokenType::SEMICOLON, "Expected ';' after 'dekhao' expression");
    return std::make_unique<PrintNode>(std::move(expr), tok.line, tok.col);
}

// ------------------------------------------------------------------
// parseBlock — shuru <stmt>* shesh
// ------------------------------------------------------------------
std::unique_ptr<BlockNode> Parser::parseBlock() {
    Token shuruTok = consume(TokenType::SHURU, "Expected 'shuru' to begin block");
    auto block = std::make_unique<BlockNode>(shuruTok.line, shuruTok.col);

    while (!isAtEnd() && !check(TokenType::SHESH)) {
        try {
            auto stmt = parseStatement();
            if (stmt) block->statements.push_back(std::move(stmt));
        } catch (...) {
            synchronize();
        }
    }

    consume(TokenType::SHESH, "Expected 'shesh' to end block");
    return block;
}

// ══════════════════════════════════════════════════════════════════
//  Expression parsing — respects operator precedence
//
//  Precedence (low → high):
//    comparison  ==  !=  <  >  <=  >=
//    addition    +  -
//    multiply    *  /
//    unary       -expr
//    primary     literals, identifiers, (expr)
// ══════════════════════════════════════════════════════════════════

ASTNodePtr Parser::parseExpression() {
    return parseComparison();
}

ASTNodePtr Parser::parseComparison() {
    auto left = parseAddSub();

    while (peek().isRelOp()) {
        Token op = advance();
        auto right = parseAddSub();
        left = std::make_unique<BinaryOpNode>(
            op.type, std::move(left), std::move(right), op.line, op.col);
    }
    return left;
}

ASTNodePtr Parser::parseAddSub() {
    auto left = parseMulDiv();

    while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
        Token op = advance();
        auto right = parseMulDiv();
        left = std::make_unique<BinaryOpNode>(
            op.type, std::move(left), std::move(right), op.line, op.col);
    }
    return left;
}

ASTNodePtr Parser::parseMulDiv() {
    auto left = parseUnary();

    while (check(TokenType::STAR) || check(TokenType::SLASH)) {
        Token op = advance();
        auto right = parseUnary();
        left = std::make_unique<BinaryOpNode>(
            op.type, std::move(left), std::move(right), op.line, op.col);
    }
    return left;
}

ASTNodePtr Parser::parseUnary() {
    if (check(TokenType::MINUS)) {
        Token op = advance();
        auto operand = parseUnary();
        return std::make_unique<UnaryMinusNode>(
            std::move(operand), op.line, op.col);
    }
    return parsePrimary();
}

ASTNodePtr Parser::parsePrimary() {
    // String literal
    if (check(TokenType::STRING_LIT)) {
        Token t = advance();
        return std::make_unique<StringLiteralNode>(t.value, t.line, t.col);
    }

    // Integer literal
    if (check(TokenType::INTEGER_LIT)) {
        Token t = advance();
        int val = 0;
        try { val = std::stoi(t.value); }
        catch (...) {
            err.semanticError(t.line, t.col, "Integer literal overflow: " + t.value);
        }
        return std::make_unique<IntLiteralNode>(val, t.line, t.col);
    }

    // Float literal
    if (check(TokenType::FLOAT_LIT)) {
        Token t = advance();
        double val = 0.0;
        try { val = std::stod(t.value); }
        catch (...) {
            err.semanticError(t.line, t.col, "Float literal overflow: " + t.value);
        }
        return std::make_unique<FloatLiteralNode>(val, t.line, t.col);
    }

    // Identifier
    if (check(TokenType::IDENTIFIER)) {
        Token t = advance();
        return std::make_unique<IdentifierNode>(t.value, t.line, t.col);
    }

    // Grouped expression: ( expr )
    if (check(TokenType::LPAREN)) {
        advance(); // consume '('
        auto expr = parseExpression();
        consume(TokenType::RPAREN, "Expected ')' after expression");
        return expr;
    }

    Token& tok = peek();
    err.syntaxError(tok.line, tok.col,
        "Expected an expression but got '" + tok.value + "'");
    throw std::runtime_error("syntax error");
}
