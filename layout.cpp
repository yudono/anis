#include "layout.h"
#include "renderer.h"
#include "widgets.h"
#include <iostream>
#include <OpenGL/gl.h>

float parse_dim(const std::string& val, float maxVal, float defaultVal) {
    if (val.empty() || val == "auto") return defaultVal;
    
    if (val.back() == '%') {
        try {
            float pct = std::stof(val.substr(0, val.size() - 1));
            return maxVal * (pct / 100.0f);
        } catch (...) { return defaultVal; }
    }
    
    // px or just number
    try {
        size_t pxPos = val.find("px");
        if (pxPos != std::string::npos) {
            return std::stof(val.substr(0, pxPos));
        }
        return std::stof(val);
    } catch (...) { return defaultVal; }
}

Vec2 measure_node(const Node& n, float constraintsW, float constraintsH) {
    float w = parse_dim(n.attrs.count("width") ? n.attrs.at("width") : "auto", constraintsW, 0);
    float h = parse_dim(n.attrs.count("height") ? n.attrs.at("height") : "auto", constraintsH, 0);

    if (n.tag == "Text") {
        float tw = measure_text_width(n.text);
        // If explicit width set, use it, else auto
        return { (w > 0) ? w : tw, (h > 0) ? h : 24.0f };
    }
    else if (n.tag == "Button") {
        return { (w > 0) ? w : 160.0f, (h > 0) ? h : 40.0f };
    }
    else if (n.tag == "Textfield") {
        return { (w > 0) ? w : 300.0f, (h > 0) ? h : 30.0f };
    }
    else if (n.tag == "Image") {
        return { (w > 0) ? w : 50.0f, (h > 0) ? h : 50.0f };
    }
    else if (n.tag == "Row") {
         float totalW = 0;
         float maxH = 0;
         for (const auto& c : n.children) {
             Vec2 s = measure_node(c, constraintsW, constraintsH);
             totalW += s.x; // naive, ignores gap
             if (s.y > maxH) maxH = s.y;
         }
         return { (w > 0) ? w : totalW, (h > 0) ? h : maxH };
    }
    else if (n.tag == "Column" || n.tag == "View") {
         float totalH = 0;
         float maxW = 0;
         for (const auto& c : n.children) {
             Vec2 s = measure_node(c, constraintsW, constraintsH);
             totalH += s.y;
             if (s.x > maxW) maxW = s.x;
         }
         return { (w > 0) ? w : maxW, (h > 0) ? h : totalH };
    }
    
    return { w, h };
}


void render_node(const Node& n, float x, float& y, double mx, double my, bool click, float windowW, float windowH) {
    float w = parse_dim(n.attrs.count("width") ? n.attrs.at("width") : "auto", windowW, 0);
    float h = parse_dim(n.attrs.count("height") ? n.attrs.at("height") : "auto", windowH, 0);

    // Helpers for styles
    Color textCol = {0.2f, 0.2f, 0.2f, 1}; // Default dark text
    if (n.attrs.count("color")) textCol = parse_color(n.attrs.at("color"));
    
    float sizeScale = 1.0f;
    if (n.attrs.count("fontSize")) {
         float px = parse_dim(n.attrs.at("fontSize"), 100, 16);
         sizeScale = px / 16.0f; 
    }
    
    // Background Color (Generic View/Container)
    if (n.attrs.count("backgroundColor")) {
        float drawW = (w > 0) ? w : windowW; // Default fill width if bg set
        float drawH = (h > 0) ? h : 0; 
        if (drawH > 0) rect(x, y, drawW, drawH, parse_color(n.attrs.at("backgroundColor")));
    }

    if (n.tag == "Page") {
        if (w <= 0) w = windowW;
        if (h <= 0) h = windowH;
        for (const auto& c : n.children) {
            render_node(c, x, y, mx, my, click, w, h);
        }
    }
    
    else if (n.tag == "Scrollview") {
        float svW = (w > 0) ? w : windowW;
        float svH = (h > 0) ? h : windowH;
        if (n.attrs.count("backgroundColor")) rect(x, y, svW, svH, parse_color(n.attrs.at("backgroundColor")));

        /*
        glEnable(GL_SCISSOR_TEST);
        float scY = windowH - (y + svH); 
        glScissor((GLint)x, (GLint)scY, (GLint)svW, (GLint)svH);
        */
        
        float startY = y; 
        y -= appState.scrollOffset; 

        float contentH = 0;
        for (const auto& c : n.children) contentH += measure_node(c, svW, svH).y;
        appState.maxScroll = (contentH > svH) ? (contentH - svH) : 0;

        for (const auto& c : n.children) {
            render_node(c, x, y, mx, my, click, svW, svH);
        }
        
        y = startY + svH; 
        glDisable(GL_SCISSOR_TEST);
    }
    else if (n.tag == "Row") {
        float rowW = (w > 0) ? w : windowW;
        float rowH = (h > 0) ? h : 0; 
        
        // 1. Measure  (using same measure_node)
        std::vector<Vec2> sizes;
        float totalChildrenW = 0;
        float maxChildH = 0;
        for (const auto& c : n.children) {
            Vec2 s = measure_node(c, rowW, windowH);
            sizes.push_back(s);
            totalChildrenW += s.x;
            if (s.y > maxChildH) maxChildH = s.y;
        }
        if (rowH == 0) rowH = maxChildH;

        if (n.attrs.count("backgroundColor")) rect(x, y, rowW, rowH, parse_color(n.attrs.at("backgroundColor")));

        // 2. Layout
        std::string justify = n.attrs.count("justifyContent") ? n.attrs.at("justifyContent") : "start";
        std::string align = n.attrs.count("alignItems") ? n.attrs.at("alignItems") : "top";
        
        float startX = x;
        float gap = 0;
        int count = n.children.size();
        float freeSpace = rowW - totalChildrenW;
        if (justify == "center") startX += freeSpace / 2;
        else if (justify == "right" || justify == "end") startX += freeSpace;
        else if (justify == "between" && count > 1) gap = freeSpace / (count - 1);
        else if (justify == "around" && count > 0) { gap = freeSpace / count; startX += gap / 2; }

        float cx = startX;
        float cy = y;
        for (size_t i = 0; i < n.children.size(); i++) {
            float childH = sizes[i].y;
            float childY = cy;
            if (align == "center") childY += (rowH - childH) / 2;
            else if (align == "bottom") childY += (rowH - childH);
            float tempY = childY;
            render_node(n.children[i], cx, tempY, mx, my, click, sizes[i].x, windowH);
            cx += sizes[i].x + gap;
        }
        y += rowH;
    }
    else if (n.tag == "Column" || n.tag == "View") {
        float colW = (w > 0) ? w : windowW;
        
        if (n.attrs.count("backgroundColor") && h > 0) rect(x, y, colW, h, parse_color(n.attrs.at("backgroundColor")));

        std::string align = n.attrs.count("alignItems") ? n.attrs.at("alignItems") : "left";
        for (const auto& c : n.children) {
            Vec2 s = measure_node(c, colW, windowH);
            float cx = x;
            if (align == "center") cx += (colW - s.x) / 2;
            else if (align == "right") cx += (colW - s.x);
            render_node(c, cx, y, mx, my, click, colW, windowH);
        }
    }
    // -- Widgets --
    else if (n.tag == "Text") {
        draw_text(x, y, n.text, sizeScale, textCol);
        y += 24 * sizeScale;
    } 
    else if (n.tag == "Button") {
        float btnW = (w > 0) ? w : 160;
        float btnH = (h > 0) ? h : 40;
        
        if (button(x, y, btnW, btnH, n.text, mx, my, click)) {
            std::cout << "[Action] Button '" << n.text << "' clicked!" << std::endl;
        }
        y += btnH; 
    }
    else if (n.tag == "Textfield") {
        float tw = (w > 0) ? w : 300;
        float th = (h > 0) ? h : 30;
        draw_textbox(appState.notesBox, x, y, tw, th, mx, my, click);
        y += th;
    }
    else if (n.tag == "Image") {
         if (n.attrs.count("src") && n.attrs.at("src") == "logo.png") {
             float imgW = (w > 0) ? w : 50;
             float imgH = (h > 0) ? h : 50;
             glColor3f(1,1,1);
             rect(x, y, imgW, imgH); 
             y += imgH;
         } else {
             y += 60; 
         }
    }
}
