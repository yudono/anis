#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
// Lang
#include "core/lang/lexer.h"
#include "core/lang/parser.h"
#include "core/lang/interpreter.h"
#include "core/debugger.h"
#include "lib/gui/minigui.h"
#include "lib/gui/layout.h"
#include "lib/register.h"

// Define global appState needed by minigui
AppState appState;

// Define static member of Debugger
bool Debugger::isReplMode = false;

void printHelp() {
    std::cout << COLOR_CYAN << "Sunda Programming Language" << COLOR_RESET << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_GREEN << "USAGE:" << COLOR_RESET << std::endl;
    std::cout << "  sunda                        Enter REPL mode" << std::endl;
    std::cout << "  sunda <file.sd>              Run a Sunda script" << std::endl;
    std::cout << "  sunda --help                 Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_GREEN << "EXAMPLES:" << COLOR_RESET << std::endl;
    std::cout << "  sunda                        Start interactive shell" << std::endl;
    std::cout << "  sunda examples/hello.sd      Run hello.sd" << std::endl;
    std::cout << "  sunda myapp/main.sd          Run GUI application" << std::endl;
    std::cout << std::endl;
    std::cout << COLOR_GREEN << "FEATURES:" << COLOR_RESET << std::endl;
    std::cout << "  • Modern JavaScript-like syntax" << std::endl;
    std::cout << "  • Arrow functions: (a, b) => { ... }" << std::endl;
    std::cout << "  • Object spread: { ...obj, key: value }" << std::endl;
    std::cout << "  • Ternary operator: condition ? true : false" << std::endl;
    std::cout << "  • JSX-like GUI components" << std::endl;
    std::cout << "  • Built-in GUI library" << std::endl;
    std::cout << std::endl;
}

void runREPL() {
    Debugger::isReplMode = true;
    Interpreter interpreter;
    register_std_libs(interpreter);

    std::cout << COLOR_CYAN << "Sunda REPL (v1.0.0)" << COLOR_RESET << std::endl;
    std::cout << "Type 'exit' to quit." << std::endl;

    std::string line;
    while (true) {
        std::cout << COLOR_GREEN << "sunda> " << COLOR_RESET;
        if (!std::getline(std::cin, line) || line == "exit") break;
        if (line.empty()) continue;

        try {
            // 1. Lex
            Lexer lexer(line);
            std::vector<Token> tokens = lexer.tokenize();

            // 2. Parse
            Parser parser(tokens);
            std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

            // 3. Interpret
            interpreter.sourceCode = line;
            interpreter.hasLastExpressionValue = false; // Reset before run
            interpreter.interpret(statements);
            
            if (interpreter.hasLastExpressionValue) {
                std::cout << COLOR_BLUE << "=> " << COLOR_RESET << interpreter.lastExpressionValue.toString() << std::endl;
            }
        } catch (const std::exception& e) {
            // Error already printed by Debugger if it didn't exit
        } catch (...) {
            std::cerr << "Unknown error occurred" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        runREPL();
        return 0;
    }
    
    std::string arg = argv[1];
    if (arg == "--help" || arg == "-h") {
        printHelp();
        return 0;
    }
    
    // Check flags
    bool dumpTokens = false;
    std::string filePath = arg;
    
    if (argc > 2) {
        std::string flag = argv[1];
        if (flag == "--dump-tokens") {
            dumpTokens = true;
            filePath = argv[2];
        } else {
             filePath = argv[1];
             if (std::string(argv[2]) == "--dump-tokens") dumpTokens = true;
        }
    }
    
    // Set global base path for relative resource loading
    std::string scriptPath = filePath;
    size_t lastSlash = scriptPath.find_last_of("/\\");
    if (lastSlash != std::string::npos) {
        g_basePath = scriptPath.substr(0, lastSlash + 1);  // Include the slash
    }

    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // 1. Lex
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();
    
    if (dumpTokens) {
        std::cout << "TOKENS:" << std::endl;
        for (const auto& t : tokens) {
            std::cout << "Line " << t.line << ": " << t.type << " '" << t.text << "'" << std::endl;
        }
        return 0; // Exit after dump
    }

    // 2. Parse
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    // 3. Interpret
    Interpreter interpreter;
    interpreter.sourceCode = source; // Pass source for debugging
    interpreter.currentFile = filePath; // Pass file path for debugging
    
    // Register Libs
    register_std_libs(interpreter);

    interpreter.interpret(statements);

    return 0;
}
