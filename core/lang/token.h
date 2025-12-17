#ifndef SUNDA_TOKEN_H
#define SUNDA_TOKEN_H

#include <string>
#include <iostream>

enum TokenType {
    TOK_EOF,
    TOK_VAR, TOK_IF, TOK_FOR, TOK_FUNCTION, TOK_IMPORT, TOK_FROM, TOK_RETURN,
    TOK_SWITCH, TOK_CASE, TOK_DEFAULT, // Added
    TOK_CONST, TOK_LBRACKET, TOK_RBRACKET,
    TOK_IDENTIFIER, TOK_STRING, TOK_NUMBER,
    TOK_LPAREN, TOK_RPAREN, TOK_LBRACE, TOK_RBRACE,
    TOK_EQ, TOK_EQEQ, TOK_SEMICOLON, TOK_COLON, TOK_COMMA, TOK_DOT,
    TOK_PLUS, TOK_MINUS, TOK_STAR, TOK_SLASH, TOK_PLUS_EQUAL,
    TOK_LT, TOK_GT,
    TOK_ARROW // =>
};

struct Token {
    TokenType type;
    std::string text;
    // int line; // nice to have later
};

#endif
