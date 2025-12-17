#include "parser.h"
#include <iostream>

std::vector<std::shared_ptr<Stmt>> Parser::parse() {
    std::vector<std::shared_ptr<Stmt>> statements;
    while (!isAtEnd()) {
        try {
            statements.push_back(declaration());
        } catch (...) {
            advance(); // synchronize
        }
    }
    return statements;
}

std::shared_ptr<Stmt> Parser::declaration() {
    if (match(TOK_VAR)) {
        Token name = consume(TOK_IDENTIFIER, "Expect variable name.");
        std::shared_ptr<Expr> init = nullptr;
        if (match(TOK_EQ)) {
            init = expression();
        }
        return std::make_shared<VarDeclStmt>(name.text, init);
    }
    // const handling (treat as var or destructuring)
    if (match(TOK_CONST)) {
        if (match(TOK_LBRACKET)) {
            // Destructuring: const [a, b] = ...
            std::vector<std::string> names;
            if (!check(TOK_RBRACKET)) {
                do {
                    Token t = consume(TOK_IDENTIFIER, "Expect variable name in destructuring.");
                    names.push_back(t.text);
                } while (match(TOK_COMMA));
            }
            consume(TOK_RBRACKET, "Expect ']' after destructuring list.");
            consume(TOK_EQ, "Expect '=' after destructuring declaration.");
            std::shared_ptr<Expr> init = expression();
            return std::make_shared<DestructureStmt>(names, init);
        } else {
             // Normal const var
             Token name = consume(TOK_IDENTIFIER, "Expect variable name.");
             std::shared_ptr<Expr> init = nullptr;
             if (match(TOK_EQ)) {
                 init = expression();
             }
             return std::make_shared<VarDeclStmt>(name.text, init);
        }
    }
    if (match(TOK_FUNCTION)) {
        // Named function declaration: function Name(a, b) { ... }
        Token name = consume(TOK_IDENTIFIER, "Expect function name.");
        consume(TOK_LPAREN, "Expect '(' after function name.");
        std::vector<std::string> params;
        if (!check(TOK_RPAREN)) {
            do {
                if (match(TOK_IDENTIFIER)) {
                    params.push_back(previous().text);
                }
            } while (match(TOK_COMMA));
        }
        consume(TOK_RPAREN, "Expect ')' after parens.");
        consume(TOK_LBRACE, "Expect '{' before function body.");
        std::shared_ptr<BlockStmt> body = std::make_shared<BlockStmt>();
        while (!check(TOK_RBRACE) && !isAtEnd()) {
            body->statements.push_back(declaration());
        }
        consume(TOK_RBRACE, "Expect '}' after body.");
        return std::make_shared<FuncDeclStmt>(name.text, params, body);
    }
    if (match(TOK_IMPORT)) {
        if (match(TOK_STRING)) {
            std::string mod = previous().text;
            return std::make_shared<ImportStmt>(mod);
        }
        if (match(TOK_LBRACE)) {
            std::vector<std::string> syms;
            if (!check(TOK_RBRACE)) {
                do {
                    if (match(TOK_IDENTIFIER)) syms.push_back(previous().text);
                } while (match(TOK_COMMA));
            }
            consume(TOK_RBRACE, "Expect '}' after import list.");
            if (match(TOK_FROM) || (check(TOK_IDENTIFIER) && peek().text == "from" && advance().type == TOK_IDENTIFIER)) {
                 // matched from
            }
            Token modToken = consume(TOK_STRING, "Expect module string after 'from'.");
            return std::make_shared<ImportStmt>(modToken.text, syms);
        }
    }

    return statement();
}

std::shared_ptr<Stmt> Parser::statement() {
    if (match(TOK_RETURN)) {
        std::shared_ptr<Expr> value = nullptr;
        if (!check(TOK_SEMICOLON) && !check(TOK_RBRACE)) { // rudimentary check
            value = expression();
        }
        // consume semicolon if present
        match(TOK_SEMICOLON);
        return std::make_shared<ReturnStmt>(value);
    }
    if (match(TOK_IF)) {
        consume(TOK_LPAREN, "Expect '(' after if.");
        std::shared_ptr<Expr> condition = expression();
        consume(TOK_RPAREN, "Expect ')' after condition.");
        std::shared_ptr<Stmt> thenBranch = statement();
        return std::make_shared<IfStmt>(condition, thenBranch);
    }
    if (match(TOK_SWITCH)) {
        consume(TOK_LPAREN, "Expect '(' after switch.");
        std::shared_ptr<Expr> condition = expression();
        consume(TOK_RPAREN, "Expect ')' after condition.");
        consume(TOK_LBRACE, "Expect '{' before switch cases.");
        
        std::vector<Case> cases;
        while (!check(TOK_RBRACE) && !isAtEnd()) {
            if (match(TOK_CASE)) {
                 std::shared_ptr<Expr> value = expression();
                 consume(TOK_COLON, "Expect ':' after case value.");
                 // Parse statements until next case/default/brace?
                 // Simplification: Require block or single statement
                 // Or loop declarations until case/default/rbrace
                 std::shared_ptr<BlockStmt> block = std::make_shared<BlockStmt>();
                 while (!check(TOK_CASE) && !check(TOK_DEFAULT) && !check(TOK_RBRACE) && !isAtEnd()) {
                     block->statements.push_back(declaration());
                 }
                 cases.push_back({value, block});
            } else if (match(TOK_DEFAULT)) {
                 consume(TOK_COLON, "Expect ':' after default.");
                 std::shared_ptr<BlockStmt> block = std::make_shared<BlockStmt>();
                 while (!check(TOK_CASE) && !check(TOK_DEFAULT) && !check(TOK_RBRACE) && !isAtEnd()) {
                     block->statements.push_back(declaration());
                 }
                 cases.push_back({nullptr, block});
            } else {
                 // Error or skip
                 advance();
            }
        }
        consume(TOK_RBRACE, "Expect '}' after switch cases.");
        return std::make_shared<SwitchStmt>(condition, cases);
    }
    if (match(TOK_LBRACE)) {
        std::shared_ptr<BlockStmt> block = std::make_shared<BlockStmt>();
        while (!check(TOK_RBRACE) && !isAtEnd()) {
            block->statements.push_back(declaration());
        }
        consume(TOK_RBRACE, "Expect '}'");
        return block;
    }
    std::shared_ptr<Expr> expr = expression();
    match(TOK_SEMICOLON); // optional
    return std::make_shared<ExprStmt>(expr);
}

// Expression Precedence
// expression -> assignment
// assignment -> equality
// ... -> primary

std::shared_ptr<Expr> Parser::expression() {
    return assignment();
}

std::shared_ptr<Expr> Parser::assignment() {
    std::shared_ptr<Expr> expr = equality();
    
    if (match(TOK_EQ)) {
        std::shared_ptr<Expr> value = assignment();
        if (auto var = std::dynamic_pointer_cast<VarExpr>(expr)) {
            return std::make_shared<BinaryExpr>(var, "=", value);
        }
        std::cerr << "Invalid assignment target." << std::endl;
    }
    if (match(TOK_PLUS_EQUAL)) {
        std::shared_ptr<Expr> value = assignment(); // right-associative?
        if (auto var = std::dynamic_pointer_cast<VarExpr>(expr)) {
             return std::make_shared<BinaryExpr>(var, "+=", value);
        }
    }
    
    return expr;
}

std::shared_ptr<Expr> Parser::equality() {
    std::shared_ptr<Expr> expr = term();
    while (match(TOK_EQEQ)) { // != not valid yet
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<BinaryExpr>(expr, "==", right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::term() {
    std::shared_ptr<Expr> expr = primary(); // factor?
    while (match(TOK_PLUS) || match(TOK_MINUS)) {
        std::string op = previous().type == TOK_PLUS ? "+" : "-";
        std::shared_ptr<Expr> right = primary(); 
        expr = std::make_shared<BinaryExpr>(expr, op, right);
    }
    return expr;
}

std::shared_ptr<Expr> Parser::primary() {
    if (match(TOK_NUMBER)) return std::make_shared<LiteralExpr>(previous().text, false);
    if (match(TOK_STRING)) return std::make_shared<LiteralExpr>(previous().text, true);
    
    // Identifier or Call
    if (match(TOK_IDENTIFIER)) {
        std::string name = previous().text;
        if (match(TOK_LPAREN)) {
            return finishCall(name);
        }
        return std::make_shared<VarExpr>(name);
    }
    
    // Grouping or Lambda: ( ) or (a, b) =>
    if (match(TOK_LPAREN)) {
        // Need to lookahead to see if it is Arrow Function
        // Complex lookahead needed or we speculative parse?
        // Let's assume for now if we see identifiers separated by commas followed by ')' it MIGHT be lambda
        // But (a) is also group expr.
        // We really need to find '=>'
        
        // Simpler approach:
        // If we see ')' immediately -> check '=>' or '{'
        // If 'id' -> peek next?
        
        // Let's stick to simple "Try to parse as lambda if '=>' follows parens" logic, 
        // but since we are one-pass...
        
        // We can check if we hit '=>' AFTER matching parens?
        // We need to parse contents inside parens first.
        // If they are ALL identifiers, we can treat them as potential params.
        
        // LIMITATION: For now, support ONLY empty params `()` or we need robust backtracking.
        // OR: (param) => ...
        // We can peek ahead? `Parser` has `peekNext`.
        
        // Let's implement: `(a) =>` and `(a, b) =>`
        // We already consumed `(`.
        
        // Check if next is `)`.
        if (check(TOK_RPAREN)) {
             // () case
             Token t = peekNext(); // token after )
             if (t.type == TOK_ARROW) {
                 consume(TOK_RPAREN, "Expect ')'");
                 consume(TOK_ARROW, "Expect '=>'");
                 consume(TOK_LBRACE, "Expect '{'");
                 std::shared_ptr<BlockStmt> body = std::make_shared<BlockStmt>();
                 while (!check(TOK_RBRACE) && !isAtEnd()) {
                     body->statements.push_back(declaration()); // error: identifier not found
                 }
                 consume(TOK_RBRACE, "Expect '}'");
                 return std::make_shared<FunctionExpr>(std::vector<std::string>{}, body);
             }
             // Legacy () { ... }
             if (t.type == TOK_LBRACE) {
                 consume(TOK_RPAREN, "Expect ')'");
                 consume(TOK_LBRACE, "Expect '{'");
                 std::shared_ptr<BlockStmt> body = std::make_shared<BlockStmt>();
                 while (!check(TOK_RBRACE) && !isAtEnd()) {
                      body->statements.push_back(declaration());
                 }
                 consume(TOK_RBRACE, "Expect '}'");
                 return std::make_shared<FunctionExpr>(std::vector<std::string>{}, body);
             }
        }
        
        // If not empty, it's Expression OR Params.
        // Only way to distinguish is `=>`.
        // Let's parse as Expression first.
        std::shared_ptr<Expr> expr = expression();
        consume(TOK_RPAREN, "Expect ')' after expression.");
        
        // If followed by `=>`, we made a mistake?
        // Actually, if expr is `VarExpr`, we can reinterpret it as Param?
        if (match(TOK_ARROW)) {
             // It WAS a lambda. checking if expr is valid param list.
             // Single param `(a)` -> `VarExpr`.
             std::vector<std::string> params;
             if (auto v = std::dynamic_pointer_cast<VarExpr>(expr)) {
                 params.push_back(v->name);
             } else {
                 // Error: (1+1) => ... invalid
                 std::cerr << "Invalid parameter list for arrow function." << std::endl;
             }
             
             consume(TOK_LBRACE, "Expect '{' for lambda body.");
             std::shared_ptr<BlockStmt> body = std::make_shared<BlockStmt>();
             while (!check(TOK_RBRACE) && !isAtEnd()) {
                 body->statements.push_back(declaration());
             }
             consume(TOK_RBRACE, "Expect '}' after lambda body.");
             return std::make_shared<FunctionExpr>(params, body);
        }
        
        return expr;
    }
    
    // JSX: <Tag ... > ... </Tag> or <Tag ... />
    if (match(TOK_LT)) {
        // Tag Name (Identifier usually, or generic)
        if (!match(TOK_IDENTIFIER)) {
             std::cerr << "Expect tag name." << std::endl;
             // recover?
        }
        std::string tagName = previous().text;
        std::map<std::string, std::shared_ptr<Expr>> attrs;
        std::vector<std::shared_ptr<Expr>> children;
        
        // Parse attributes: name="value" or name={expr} or boolean name
        // Iterate until '/' or '>'
        while (!check(TOK_GT) && !check(TOK_SLASH) && !isAtEnd()) {
             if (match(TOK_IDENTIFIER)) {
                 std::string key = previous().text;
                 std::shared_ptr<Expr> val = std::make_shared<LiteralExpr>("true", false); // Default boolean true
                 
                 if (match(TOK_EQ)) {
                      if (match(TOK_STRING)) {
                           val = std::make_shared<LiteralExpr>(previous().text, true);
                      } else if (match(TOK_LBRACE)) {
                           val = expression();
                           consume(TOK_RBRACE, "Expect '}' after attribute expression.");
                      } else {
                           std::cerr << "Expect string or {expr} for attribute value." << std::endl;
                      }
                 }
                 attrs[key] = val;
             } else {
                 advance(); // skip garbage?
             }
        }
        
        // Self closing?
        if (match(TOK_SLASH)) {
            consume(TOK_GT, "Expect '>' after '/' in self-closing tag.");
            return std::make_shared<JsxExpr>(tagName, attrs, children);
        }
        
        consume(TOK_GT, "Expect '>' after attributes.");
        
        // Parse Children
        // Stop at </TagName>
        // We look for '<' '/' 'TagName'
        while (!isAtEnd()) {
            if (check(TOK_LT) && peekNext().type == TOK_SLASH) {
                 // Closing tag candidate
                 // Check if it matches OUR tagName?
                 // We don't peek 2 ahead easily.
                 // Consuming it is safer.
                 break;
            }
            // Text content?
            // If tokens[current] is TEXT... we don't have text tokens.
            // We have generic tokens. 
            // If we hit '<', it's a child JSX. calling primary() will handle '<'.
            // If it is NOT '<', it is text.
            if (check(TOK_LT)) {
                children.push_back(primary());
            } else {
                // Consume as text. Identifier, number, string, symbol...
                // Jsx text handling is tricky without lexer support.
                // Simple: loop consume and append string until '<'?
                // For now, support ONLY JSX child or {Expr} child.
                if (match(TOK_LBRACE)) {
                    children.push_back(expression());
                    consume(TOK_RBRACE, "Expect '}'");
                } else {
                    // Primitive Text content approximation
                    std::string s = advance().text;
                    children.push_back(std::make_shared<LiteralExpr>(s, true));
                }
            }
        }
        
        consume(TOK_LT, "Expect closing tag.");
        consume(TOK_SLASH, "Expect '/'.");
        if (match(TOK_IDENTIFIER)) {
             if (previous().text != tagName) {
                  std::cerr << "Mismatch closing tag: expected " << tagName << ", got " << previous().text << std::endl;
             }
        }
        consume(TOK_GT, "Expect '>' after closing tag.");
        return std::make_shared<JsxExpr>(tagName, attrs, children);
    }
    
    std::cerr << "Unexpected token: " << peek().text << std::endl;
    throw std::runtime_error("Unexpected token in primary expression.");
}

std::shared_ptr<CallExpr> Parser::finishCall(std::string name) {
    std::vector<std::shared_ptr<Expr>> args;
    if (!check(TOK_RPAREN)) {
        do {
            if (args.size() >= 255) {
                std::cerr << "Can't have more than 255 arguments." << std::endl;
            }
            args.push_back(expression());
        } while (match(TOK_COMMA));
    }
    consume(TOK_RPAREN, "Expect ')' after arguments.");
    return std::make_shared<CallExpr>(name, args);
}

// Helpers
bool Parser::match(TokenType t) {
    if (check(t)) {
        advance();
        return true;
    }
    return false;
}
bool Parser::check(TokenType t) {
    if (isAtEnd()) return false;
    return peek().type == t;
}
Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}
bool Parser::isAtEnd() {
    return peek().type == TOK_EOF;
}
Token Parser::peek() {
    return tokens[current];
}
Token Parser::peekNext() {
    if (isAtEnd()) return peek();
    return tokens[current + 1];
}
Token Parser::previous() {
    return tokens[current - 1];
}
Token Parser::consume(TokenType t, std::string err) {
    if (check(t)) return advance();
    std::cerr << "Parse Error: " << err << " at " << peek().text << std::endl;
    throw std::runtime_error(err);
}
