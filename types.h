#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>
#include <map>

// --- Structs ---

struct Color { float r, g, b, a; };

struct Vec2 { float x, y; };

struct Node {
    std::string tag;
    std::string text;
    std::map<std::string, std::string> attrs;
    std::vector<Node> children;
};

struct TextBox {
    std::string value;
    bool focus = false;
};

struct AppState {
    TextBox searchBox;
    TextBox notesBox;
    bool btn1Clicked = false;
    bool btn2Clicked = false;
    TextBox* activeTextbox = nullptr;
    float scrollOffset = 0.0f;
    float maxScroll = 0.0f;
};

// Global state declaration
extern AppState appState;

#endif
