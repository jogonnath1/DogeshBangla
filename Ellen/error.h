// =============================================================
//  DogeshBangla Compiler — error.h
//  Error handler — collects and reports all compiler errors
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <sstream>

// ------------------------------------------------------------------
// Error severity levels
// ------------------------------------------------------------------
enum class ErrorSeverity { LEXICAL, SYNTAX, SEMANTIC };

// ------------------------------------------------------------------
// A single compiler error record
// ------------------------------------------------------------------
struct CompilerError {
    ErrorSeverity severity;
    std::string   message;
    int           line;
    int           col;

    CompilerError(ErrorSeverity s, std::string m, int l, int c)
        : severity(s), message(std::move(m)), line(l), col(c) {}

    std::string toString() const {
        std::string tag;
        switch (severity) {
            case ErrorSeverity::LEXICAL:   tag = "[LEXICAL ERROR]";   break;
            case ErrorSeverity::SYNTAX:    tag = "[SYNTAX ERROR]";    break;
            case ErrorSeverity::SEMANTIC:  tag = "[SEMANTIC ERROR]";  break;
        }
        return tag + " Line " + std::to_string(line)
             + ", Col " + std::to_string(col)
             + ": " + message;
    }
};

// ------------------------------------------------------------------
// ErrorHandler — collects errors, prints them, never throws
// ------------------------------------------------------------------
class ErrorHandler {
public:
    std::vector<CompilerError> errors;
    bool hadError = false;

    void lexicalError(int line, int col, const std::string& msg) {
        errors.emplace_back(ErrorSeverity::LEXICAL, msg, line, col);
        hadError = true;
    }

    void syntaxError(int line, int col, const std::string& msg) {
        errors.emplace_back(ErrorSeverity::SYNTAX, msg, line, col);
        hadError = true;
    }

    void semanticError(int line, int col, const std::string& msg) {
        errors.emplace_back(ErrorSeverity::SEMANTIC, msg, line, col);
        hadError = true;
    }

    // Print all collected errors to stderr
    void printAll() const {
        if (errors.empty()) {
            std::cout << "[DogeshBangla] Compilation successful. No errors found.\n";
            return;
        }
        std::cerr << "\n========== DogeshBangla Compiler Errors ==========\n";
        for (const auto& e : errors)
            std::cerr << e.toString() << "\n";
        std::cerr << "==================================================\n";
        std::cerr << "Total errors: " << errors.size() << "\n\n";
    }

    bool hasErrors() const { return hadError; }
    void clear() { errors.clear(); hadError = false; }
};
