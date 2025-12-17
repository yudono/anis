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
#include "lib/gui/minigui.h"
#include "lib/gui/types.h"
#include "lib/register.h"

// Define global appState needed by minigui
AppState appState;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: sunda <file.sd>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
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

    // 2. Parse
    Parser parser(tokens);
    std::vector<std::shared_ptr<Stmt>> statements = parser.parse();

    // 3. Interpret
    Interpreter interpreter;
    
    // Bindings
    register_std_libs(interpreter);

    interpreter.interpret(statements);

    return 0;
}
