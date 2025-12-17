#include "parser.h"
#include <iostream>
#include <cctype>

XmlParser::XmlParser(const std::string& s) : src(s), pos(0) {}

Node XmlParser::parse() {
    skip_ws();
    return parse_node();
}

Node XmlParser::parse_node() {
    Node node;
    expect('<');
    node.tag = read_word();

    // attributes
    while (peek() != '>' && peek() != '/') {
        skip_ws();
        auto key = read_word();
        if (key.empty()) break; 
        expect('=');
        expect('"');
        auto val = read_until('"');
        expect('"');
        node.attrs[key] = val;
    }

    // self closing
    if (peek() == '/') {
        expect('/');
        expect('>');
        return node;
    }

    expect('>');

    // children or text
    while (true) {
        skip_ws();
        if (pos >= src.size()) break;
        
        if (peek() == '<' && pos + 1 < src.size() && src[pos + 1] == '/') break;

        if (peek() == '<') {

            node.children.push_back(parse_node());
        } else {
            std::string t = read_until('<');
            if (!t.empty()) node.text += t; 
        }
    }

    // closing tag
    expect('<'); expect('/');
    read_word(); // consume tag name
    expect('>');

    return node;
}

char XmlParser::peek() { 
    if (pos >= src.size()) return 0;
    return src[pos]; 
}

void XmlParser::skip_ws() { 
    while (pos < src.size()) {
        if (isspace(src[pos])) {
            pos++;
            continue;
        }
        
        // Check for // comment
        if (pos + 1 < src.size() && src[pos] == '/' && src[pos+1] == '/') {
            pos += 2;
            while (pos < src.size() && src[pos] != '\n') pos++;
            continue;
        }

        // Check for /* comment */
        if (pos + 1 < src.size() && src[pos] == '/' && src[pos+1] == '*') {
            pos += 2;
            while (pos + 1 < src.size()) {
                if (src[pos] == '*' && src[pos+1] == '/') {
                    pos += 2;
                    break;
                }
                pos++;
            }
            continue;
        }
        
        // Check for {* comment *}
        if (pos + 1 < src.size() && src[pos] == '{' && src[pos+1] == '*') {
            pos += 2;
            while (pos + 1 < src.size()) {
                if (src[pos] == '*' && src[pos+1] == '}') {
                    pos += 2;
                    break;
                }
                pos++;
            }
            continue;
        }
        
        // Check for {/* comment */} (JSX style often used)
        if (pos + 2 < src.size() && src[pos] == '{' && src[pos+1] == '/' && src[pos+2] == '*') {
            pos += 3;
            while (pos + 2 < src.size()) {
                 if (src[pos] == '*' && src[pos+1] == '/' && src[pos+2] == '}') {
                      pos += 3;
                      break;
                 }
                 pos++;
            }
            continue;
        }

        // Check for <!-- comment --> 
        // Move XML comment logic here?
        // XML comments start with < which is a token char.
        // If we handle it here, we must peek check.
        if (pos + 3 < src.size() && src[pos] == '<' && src[pos+1] == '!' && src[pos+2] == '-' && src[pos+3] == '-') {
            pos += 4;
            while (pos + 2 < src.size()) {
                 if (src[pos] == '-' && src[pos+1] == '-' && src[pos+2] == '>') {
                      pos += 3;
                      break;
                 }
                 pos++;
            }
            continue;
        }
        
        break;
    }
}

void XmlParser::expect(char c) { 
    if (peek() == c) pos++; 
}

std::string XmlParser::read_word() {
    std::string s;
    while (pos < src.size() && (isalnum(src[pos]) || src[pos] == '_' || src[pos] == '-')) 
        s += src[pos++];
    return s;
}

std::string XmlParser::read_until(char c) {
    std::string s;
    while (pos < src.size() && src[pos] != c) s += src[pos++];
    return s;
}
