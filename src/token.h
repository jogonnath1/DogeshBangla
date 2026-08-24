// =============================================================
//  DogeshBangla Compiler — token.h
//  Token type definitions and Token structure
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#pragma once
#include <string>

// ------------------------------------------------------------------
// All token types recognized by the DogeshBangla lexer
// ------------------------------------------------------------------
enum class TokenType {
    // ── Keywords ────────────────────────────────────────────────
    DHORO,       // dhoro    → variable declaration
    SANKHYA,     // sankhya  → integer type
    DOSHOMIK,    // doshomik → float type
    JODI,        // jodi     → if
    NAHOLE,      // nahole   → else
    JOTOKHON,    // jotokhon → while
    DEKHAO,      // dekhao   → print
    SHURU,       // shuru    → begin block  {
    SHESH,       // shesh    → end block    }

    // ── Literals ────────────────────────────────────────────────
    INTEGER_LIT,  // e.g.  42
    FLOAT_LIT,    // e.g.  3.14
    STRING_LIT,   // e.g.  "hello"

    // ── Identifier ──────────────────────────────────────────────
    IDENTIFIER,   // user-defined variable names

    // ── Arithmetic Operators ────────────────────────────────────
    PLUS,         // +
    MINUS,        // -
    STAR,         // *
    SLASH,        // /

    // ── Comparison / Relational Operators ───────────────────────
    ASSIGN,       // =
    EQ,           // ==
    NEQ,          // !=
    LT,           // <
    GT,           // >
    LEQ,          // <=
    GEQ,          // >=

    // ── Punctuation ─────────────────────────────────────────────
    LPAREN,       // (
    RPAREN,       // )
    SEMICOLON,    // ;

    // ── Special ─────────────────────────────────────────────────
    EOF_TOKEN,    // end of file
    UNKNOWN       // unrecognized character (triggers lexical error)
};

// ------------------------------------------------------------------
// Convert TokenType to human-readable string (for debugging/errors)
// ------------------------------------------------------------------
inline std::string tokenTypeToString(TokenType t) {
    switch (t) {
        case TokenType::DHORO:       return "DHORO";
        case TokenType::SANKHYA:     return "SANKHYA";
        case TokenType::DOSHOMIK:    return "DOSHOMIK";
        case TokenType::JODI:        return "JODI";
        case TokenType::NAHOLE:      return "NAHOLE";
        case TokenType::JOTOKHON:    return "JOTOKHON";
        case TokenType::DEKHAO:      return "DEKHAO";
        case TokenType::SHURU:       return "SHURU";
        case TokenType::SHESH:       return "SHESH";
        case TokenType::INTEGER_LIT: return "INTEGER_LIT";
        case TokenType::FLOAT_LIT:   return "FLOAT_LIT";
        case TokenType::STRING_LIT:  return "STRING_LIT";
        case TokenType::IDENTIFIER:  return "IDENTIFIER";
        case TokenType::PLUS:        return "PLUS";
        case TokenType::MINUS:       return "MINUS";
        case TokenType::STAR:        return "STAR";
        case TokenType::SLASH:       return "SLASH";
        case TokenType::ASSIGN:      return "ASSIGN";
        case TokenType::EQ:          return "EQ";
        case TokenType::NEQ:         return "NEQ";
        case TokenType::LT:          return "LT";
        case TokenType::GT:          return "GT";
        case TokenType::LEQ:         return "LEQ";
        case TokenType::GEQ:         return "GEQ";
        case TokenType::LPAREN:      return "LPAREN";
        case TokenType::RPAREN:      return "RPAREN";
        case TokenType::SEMICOLON:   return "SEMICOLON";
        case TokenType::EOF_TOKEN:   return "EOF";
        case TokenType::UNKNOWN:     return "UNKNOWN";
        default:                     return "???";
    }
}

// ------------------------------------------------------------------
// Token — one unit of source text produced by the lexer
// ------------------------------------------------------------------
struct Token {
    TokenType   type;   // what kind of token this is
    std::string value;  // the raw source text
    int         line;   // 1-based source line number
    int         col;    // 1-based column number

    Token(TokenType t, std::string v, int l, int c)
        : type(t), value(std::move(v)), line(l), col(c) {}

    // Convenience: is this token a specific keyword/operator?
    bool is(TokenType t)        const { return type == t; }
    bool isNot(TokenType t)     const { return type != t; }
    bool isEOF()                const { return type == TokenType::EOF_TOKEN; }
    bool isLiteral()            const {
        return type == TokenType::INTEGER_LIT
            || type == TokenType::FLOAT_LIT;
    }
    bool isType()               const {
        return type == TokenType::SANKHYA
            || type == TokenType::DOSHOMIK;
    }
    bool isRelOp()              const {
        return type == TokenType::EQ   || type == TokenType::NEQ
            || type == TokenType::LT   || type == TokenType::GT
            || type == TokenType::LEQ  || type == TokenType::GEQ;
    }

    std::string toString() const {
        return "[" + tokenTypeToString(type) + " '" + value
             + "' L" + std::to_string(line)
             + ":C" + std::to_string(col) + "]";
    }
};
