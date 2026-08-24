// =============================================================
//  DogeshBangla Compiler — lexer.cpp
//  Full lexer implementation
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#include "lexer.h"
#include <cctype>
#include <stdexcept>

// ------------------------------------------------------------------
// Keyword table — maps source word → TokenType
// ------------------------------------------------------------------
const std::unordered_map<std::string, TokenType> Lexer::KEYWORDS = {
    {"ধরি",    TokenType::DHORO},
    {"সংখ্যা",  TokenType::SANKHYA},
    {"দশমিক", TokenType::DOSHOMIK},
    {"যদি",     TokenType::JODI},
    {"নাহলে",   TokenType::NAHOLE},
    {"যতক্ষণ", TokenType::JOTOKHON},
    {"দেখাও",   TokenType::DEKHAO},
    {"শুরু",    TokenType::SHURU},
    {"শেষ",    TokenType::SHESH},
};

// Helper for UTF-8 support
static bool isAlphaOrUTF8(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return std::isalpha(uc) || uc >= 128;
}

static bool isAlnumOrUTF8(char c) {
    unsigned char uc = static_cast<unsigned char>(c);
    return std::isalnum(uc) || uc >= 128;
}

// ------------------------------------------------------------------
// Constructor
// ------------------------------------------------------------------
Lexer::Lexer(const std::string& source, ErrorHandler& errHandler)
    : src(source), err(errHandler), pos(0), line(1), col(1) {}

// ------------------------------------------------------------------
// tokenize() — scan the entire source, return all tokens
// ------------------------------------------------------------------
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (!isAtEnd()) {
        skipWhitespaceAndComments();
        if (isAtEnd()) break;
        tokens.push_back(nextToken());
    }
    tokens.emplace_back(TokenType::EOF_TOKEN, "", line, col);
    return tokens;
}

// ------------------------------------------------------------------
// Helpers
// ------------------------------------------------------------------
char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return src[pos];
}

char Lexer::peekNext() const {
    if (pos + 1 >= (int)src.size()) return '\0';
    return src[pos + 1];
}

char Lexer::advance() {
    char c = src[pos++];
    if (c == '\n') { line++; col = 1; }
    else            { col++; }
    return c;
}

bool Lexer::isAtEnd() const {
    return pos >= (int)src.size();
}

bool Lexer::match(char expected) {
    if (isAtEnd() || src[pos] != expected) return false;
    advance();
    return true;
}

// ------------------------------------------------------------------
// skipWhitespaceAndComments — skip spaces, tabs, newlines, // comments
// ------------------------------------------------------------------
void Lexer::skipWhitespaceAndComments() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advance();
        } else if (c == '/' && peekNext() == '/') {
            // Single-line comment: skip to end of line
            while (!isAtEnd() && peek() != '\n') advance();
        } else {
            break;
        }
    }
}

// ------------------------------------------------------------------
// nextToken — scan and return ONE token from current position
// ------------------------------------------------------------------
Token Lexer::nextToken() {
    int startLine = line;
    int startCol  = col;
    char c = advance();

    // ── Single-character tokens ───────────────────────────────────
    switch (c) {
        case '+': return {TokenType::PLUS,      "+", startLine, startCol};
        case '-': return {TokenType::MINUS,     "-", startLine, startCol};
        case '*': return {TokenType::STAR,      "*", startLine, startCol};
        case '/': return {TokenType::SLASH,     "/", startLine, startCol};
        case '(': return {TokenType::LPAREN,    "(", startLine, startCol};
        case ')': return {TokenType::RPAREN,    ")", startLine, startCol};
        case ';': return {TokenType::SEMICOLON, ";", startLine, startCol};

        // ── One-or-two character tokens ───────────────────────────
        case '=':
            if (match('=')) return {TokenType::EQ,     "==", startLine, startCol};
            return             {TokenType::ASSIGN,     "=",  startLine, startCol};
        case '!':
            if (match('=')) return {TokenType::NEQ,    "!=", startLine, startCol};
            err.lexicalError(startLine, startCol, "Unexpected '!'; did you mean '!='?");
            return {TokenType::UNKNOWN, "!", startLine, startCol};
        case '<':
            if (match('=')) return {TokenType::LEQ,    "<=", startLine, startCol};
            return             {TokenType::LT,         "<",  startLine, startCol};
        case '>':
            if (match('=')) return {TokenType::GEQ,    ">=", startLine, startCol};
            return             {TokenType::GT,         ">",  startLine, startCol};

        default: break;
    }

    // ── Numbers ───────────────────────────────────────────────────
    if (std::isdigit(c)) {
        pos--;
        if (c == '\n') { line--; col = startCol; }
        else            { col = startCol; }
        return scanNumber();
    }

    // ── String literals ───────────────────────────────────────────
    if (c == '"') {
        pos--; col = startCol;
        return scanString();
    }

    // ── Identifiers / Keywords ────────────────────────────────────
    if (isAlphaOrUTF8(c) || c == '_') {
        pos--;
        col = startCol;
        return scanIdentifierOrKeyword();
    }

    // ── Unknown character ─────────────────────────────────────────
    std::string msg = "Unknown character '";
    msg += c;
    msg += "'";
    err.lexicalError(startLine, startCol, msg);
    return {TokenType::UNKNOWN, std::string(1, c), startLine, startCol};
}

// ------------------------------------------------------------------
// scanNumber — scan an integer or float literal
// ------------------------------------------------------------------
Token Lexer::scanNumber() {
    int startLine = line;
    int startCol  = col;
    std::string num;
    bool isFloat = false;

    while (!isAtEnd() && std::isdigit(peek())) {
        num += advance();
    }

    // Check for decimal point
    if (!isAtEnd() && peek() == '.' && std::isdigit(peekNext())) {
        isFloat = true;
        num += advance(); // consume '.'
        while (!isAtEnd() && std::isdigit(peek())) {
            num += advance();
        }
    }

    if (isFloat)
        return {TokenType::FLOAT_LIT, num, startLine, startCol};
    else
        return {TokenType::INTEGER_LIT, num, startLine, startCol};
}

// ------------------------------------------------------------------
// scanIdentifierOrKeyword — scan a word and classify it
// ------------------------------------------------------------------
Token Lexer::scanIdentifierOrKeyword() {
    int startLine = line;
    int startCol  = col;
    std::string word;

    while (!isAtEnd() && (isAlnumOrUTF8(peek()) || peek() == '_')) {
        word += advance();
    }

    // Look up in keyword table
    auto it = KEYWORDS.find(word);
    if (it != KEYWORDS.end()) {
        return {it->second, word, startLine, startCol};
    }
    return {TokenType::IDENTIFIER, word, startLine, startCol};
}

// ------------------------------------------------------------------
// scanString — scan a double-quoted string literal  "hello world"
// ------------------------------------------------------------------
Token Lexer::scanString() {
    int startLine = line;
    int startCol  = col;
    advance(); // consume opening '"'
    std::string str;
    while (!isAtEnd() && peek() != '"') {
        if (peek() == '\n') {
            err.lexicalError(startLine, startCol,
                "Unterminated string: missing closing quote");
            return {TokenType::UNKNOWN, str, startLine, startCol};
        }
        str += advance();
    }
    if (isAtEnd()) {
        err.lexicalError(startLine, startCol,
            "Unterminated string: missing closing quote");
        return {TokenType::UNKNOWN, str, startLine, startCol};
    }
    advance(); // consume closing '"'
    return {TokenType::STRING_LIT, str, startLine, startCol};
}
