// =============================================================
//  DogeshBangla Compiler — lexer.h
//  Lexer interface — breaks source text into tokens
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#pragma once
#include "token.h"
#include "error.h"
#include <string>
#include <vector>
#include <unordered_map>

class Lexer {
public:
    Lexer(const std::string& source, ErrorHandler& errHandler);

    // Tokenize entire source and return all tokens
    std::vector<Token> tokenize();

private:
    std::string     src;
    ErrorHandler&   err;
    int             pos;    // current character index
    int             line;   // current line number (1-based)
    int             col;    // current column number (1-based)

    // Keyword lookup table
    static const std::unordered_map<std::string, TokenType> KEYWORDS;

    // ── Helpers ──────────────────────────────────────────────────
    char peek()          const;
    char peekNext()      const;
    char advance();
    bool isAtEnd()       const;
    bool match(char expected);

    // ── Scanning methods ─────────────────────────────────────────
    Token nextToken();
    Token scanNumber();
    Token scanString();
    Token scanIdentifierOrKeyword();
    void  skipWhitespaceAndComments();
};
