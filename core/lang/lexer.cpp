#include "lexer.h"
#include <cctype>
#include <iostream>

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (pos < src.size()) {
        char c = peek();
        if (isspace(c)) {
            advance();
        } else if (isalpha(c) || c == '_') {
            std::string ident;
            while (isalnum(peek()) || peek() == '_') ident += advance();
            
            if (ident == "var") tokens.push_back({TOK_VAR, ident});
            else if (ident == "if") tokens.push_back({TOK_IF, ident});
            else if (ident == "for") tokens.push_back({TOK_FOR, ident});
            else if (ident == "function") tokens.push_back({TOK_FUNCTION, ident});
            else if (ident == "return") tokens.push_back({TOK_RETURN, ident});
            else if (ident == "switch") tokens.push_back({TOK_SWITCH, ident}); // Added
            else if (ident == "case") tokens.push_back({TOK_CASE, ident});
            else if (ident == "default") tokens.push_back({TOK_DEFAULT, ident});
            else if (ident == "const") tokens.push_back({TOK_CONST, ident}); // Added
            else if (ident == "import") tokens.push_back({TOK_IMPORT, ident});
            else if (ident == "form" || ident == "from") tokens.push_back({TOK_FROM, ident}); 
            else tokens.push_back({TOK_IDENTIFIER, ident});
        } else if (isdigit(c)) {
            // ...
            std::string num;
            while (isdigit(peek())) num += advance();
            tokens.push_back({TOK_NUMBER, num});
        } else if (c == '"') {
             // ...
             advance(); // skip opening
            std::string str;
            while (peek() != '"' && pos < src.size()) {
                 str += advance();
            }
            advance(); // skip closing
            tokens.push_back({TOK_STRING, str});
        } else if (c == '`') {
             // ...
             advance(); // skip opening
            std::string str;
            while (peek() != '`' && pos < src.size()) {
                 str += advance();
            }
            advance(); // skip closing
            tokens.push_back({TOK_STRING, str});
        } else {
            advance();
            if (c == '(') tokens.push_back({TOK_LPAREN, "("});
            else if (c == ')') tokens.push_back({TOK_RPAREN, ")"});
            else if (c == '{') tokens.push_back({TOK_LBRACE, "{"});
            else if (c == '}') tokens.push_back({TOK_RBRACE, "}"});
            else if (c == '[') tokens.push_back({TOK_LBRACKET, "["});
            else if (c == ']') tokens.push_back({TOK_RBRACKET, "]"});
            else if (c == ';') tokens.push_back({TOK_SEMICOLON, ";"});
            else if (c == ':') tokens.push_back({TOK_COLON, ":"}); // Added
            else if (c == ',') tokens.push_back({TOK_COMMA, ","});
            else if (c == '.') tokens.push_back({TOK_DOT, "."});
            else if (c == '+') {
                 if (peek() == '=') { advance(); tokens.push_back({TOK_PLUS_EQUAL, "+="}); }
                 else tokens.push_back({TOK_PLUS, "+"});
            }
            else if (c == '-') tokens.push_back({TOK_MINUS, "-"});
            else if (c == '*') tokens.push_back({TOK_STAR, "*"});
            else if (c == '/') {
                 if (peek() == '/') {
                      // Comment
                      while (peek() != '\n' && pos < src.size()) advance();
                 } else {
                      tokens.push_back({TOK_SLASH, "/"});
                 }
            }
            else if (c == '<') tokens.push_back({TOK_LT, "<"});
            else if (c == '>') tokens.push_back({TOK_GT, ">"});
            else if (c == '=') {
                if (peek() == '=') { advance(); tokens.push_back({TOK_EQEQ, "=="}); }
                else if (peek() == '>') { advance(); tokens.push_back({TOK_ARROW, "=>"}); } // Added
                else tokens.push_back({TOK_EQ, "="});
            }
        }
    }
    tokens.push_back({TOK_EOF, ""});
    return tokens;
}

char Lexer::peek() {
    if (pos >= src.size()) return 0;
    return src[pos];
}
char Lexer::advance() {
    if (pos >= src.size()) return 0;
    return src[pos++];
}
