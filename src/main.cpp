// =============================================================
//  DogeshBangla Compiler — main.cpp
//  Entry point — orchestrates all compiler phases
//  CSE-4114 Compiler Design and Construction Sessional
// =============================================================
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

#include "token.h"
#include "error.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "codegen.h"

// ------------------------------------------------------------------
// Read entire file into string
// ------------------------------------------------------------------
static std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "[DogeshBangla] Error: Cannot open file '" << path << "'\n";
        std::exit(1);
    }
    std::ostringstream ss;
    ss << file.rdbuf();
    return ss.str();
}

// ------------------------------------------------------------------
// Write string to file
// ------------------------------------------------------------------
static void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file.is_open()) {
        std::cerr << "[DogeshBangla] Error: Cannot write to '" << path << "'\n";
        std::exit(1);
    }
    file << content;
}

// ------------------------------------------------------------------
// Derive output .py path from input .db path
// ------------------------------------------------------------------
static std::string deriveOutputPath(const std::string& inputPath) {
    // Find directory prefix of input file
    size_t lastSlash = inputPath.find_last_of("/\\");
    std::string dir = (lastSlash == std::string::npos) ? "" : inputPath.substr(0, lastSlash + 1);
    std::string baseName = (lastSlash == std::string::npos) ? inputPath : inputPath.substr(lastSlash + 1);
    size_t lastDot = baseName.find_last_of('.');
    std::string stem = (lastDot == std::string::npos) ? baseName : baseName.substr(0, lastDot);
    return dir + stem + ".py";
}

// ------------------------------------------------------------------
// Print compiler banner
// ------------------------------------------------------------------
static void printBanner() {
    std::cout << "\n";
    std::cout << "  ================================================\n";
    std::cout << "   DogeshBangla Compiler  v1.0\n";
    std::cout << "   CSE-4114 | Compiler Design Sessional\n";
    std::cout << "   Language : DogeshBangla (.dbs)\n";
    std::cout << "   Features : Part 1 & Part 2 Fully Implemented\n";
    std::cout << "   Target   : Python (.py)\n";
    std::cout << "  ================================================\n\n";
    std::cout << "\n";
}

// ------------------------------------------------------------------
// Print usage
// ------------------------------------------------------------------
static void printUsage(const char* prog) {
    std::cout << "Usage: " << prog << " <source.db> [--tokens] [--ast]\n";
    std::cout << "  <source.db>  : DogeshBangla source file\n";
    std::cout << "  --tokens     : (optional) dump all tokens to stdout\n";
    std::cout << "  --no-codegen : (optional) run only lex+parse+semantic, no output\n";
}

// ------------------------------------------------------------------
// main() — compiler pipeline
// ------------------------------------------------------------------
int main(int argc, char* argv[]) {
    printBanner();

    if (argc < 2) {
        printUsage(argv[0]);
        return 1;
    }

    std::string inputPath  = argv[1];
    bool dumpTokens        = false;
    bool noCodegen         = false;

    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--tokens")     dumpTokens = true;
        if (arg == "--no-codegen") noCodegen  = true;
    }

    // Validate file extension
    if (inputPath.size() < 4 ||
        inputPath.substr(inputPath.size() - 4) != ".dbs") {
        std::cerr << "[DogeshBangla] Warning: File should have .dbs extension.\n";
    }

    ErrorHandler err;

    // ─────────────────────────────────────────────────────────────
    //  PHASE 1: LEXICAL ANALYSIS
    // ─────────────────────────────────────────────────────────────
    std::cout << "[Phase 1] Lexical Analysis...\n";
    std::string source = readFile(inputPath);
    Lexer lexer(source, err);
    std::vector<Token> tokens = lexer.tokenize();

    if (dumpTokens) {
        std::cout << "\n── Token Stream ──────────────────────────────\n";
        for (const auto& tok : tokens)
            std::cout << "  " << tok.toString() << "\n";
        std::cout << "──────────────────────────────────────────────\n\n";
    }

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[DogeshBangla] Lexical errors found. Aborting.\n";
        return 1;
    }
    std::cout << "  -> " << tokens.size() << " tokens found.\n";

    // ─────────────────────────────────────────────────────────────
    //  PHASE 2: PARSING
    // ─────────────────────────────────────────────────────────────
    std::cout << "[Phase 2] Parsing...\n";
    Parser parser(std::move(tokens), err);
    auto program = parser.parse();

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[DogeshBangla] Syntax errors found. Aborting.\n";
        return 1;
    }
    std::cout << "  -> AST built with " << program->statements.size() << " top-level statement(s).\n";

    // ─────────────────────────────────────────────────────────────
    //  PHASE 3: SEMANTIC ANALYSIS
    // ─────────────────────────────────────────────────────────────
    std::cout << "[Phase 3] Semantic Analysis...\n";
    SemanticAnalyzer semantic(err);
    semantic.analyze(program.get());

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[DogeshBangla] Semantic errors found. Aborting.\n";
        return 1;
    }
    std::cout << "  -> Type checking passed. " << semantic.getSymbolTable().size() << " variable(s) in symbol table.\n";

    if (noCodegen) {
        std::cout << "\n[DogeshBangla] Compilation successful (no output requested).\n";
        return 0;
    }

    // ─────────────────────────────────────────────────────────────
    //  PHASE 4: CODE GENERATION
    // ─────────────────────────────────────────────────────────────
    std::cout << "[Phase 4] Generating Python code...\n";
    CodeGenerator codegen(err);
    std::string pyCode = codegen.generate(program.get());

    if (err.hasErrors()) {
        err.printAll();
        std::cerr << "[DogeshBangla] Code generation errors found.\n";
        return 1;
    }

    std::string outputPath = deriveOutputPath(inputPath);
    writeFile(outputPath, pyCode);

    std::cout << "  -> Python output written to: " << outputPath << "\n\n";

    // ─────────────────────────────────────────────────────────────
    //  DONE
    // ─────────────────────────────────────────────────────────────
    std::cout << "\n========================================\n";
    std::cout << " Compilation successful!\n";
    std::cout << " Run your program: python " << outputPath << "\n";
    std::cout << "========================================\n\n";

    return 0;
}
