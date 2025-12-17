#include "renderer.h"
#include "widgets.h"
#include "minigui.h"
#include <iostream>
#include <map>
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

// Helper to trim whitespace
std::string trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (std::string::npos == first) return str;
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

Vec2 measure_node(const Node& n, float constraintsW, float constraintsH) {
    float w = parse_dim(n.attrs.count("width") ? n.attrs.at("width") : "auto", constraintsW, 0);
    float h = parse_dim(n.attrs.count("height") ? n.attrs.at("height") : "auto", constraintsH, 0);

    if (n.tag == "Text") {
        std::string rawTxt = trim(n.text);
        std::string txt = resolve_binding(rawTxt);
        
        float sizeScale = 1.0f;
        if (n.attrs.count("fontSize")) {
             float px = parse_dim(n.attrs.at("fontSize"), 100, 16);
             sizeScale = px / 16.0f; 
        }

        float tw = measure_text_width(txt) * sizeScale;
        // If explicit width set, use it, else auto
        return { (w > 0) ? w : tw, (h > 0) ? h : (24.0f * sizeScale) };
    }
    else if (n.tag == "Button") {
        // Treat Button as auto-width/height container wrapping content
        // Default padding accounted for in render, but for measurement we must know usage
        
        float usedW = 0;
        float usedH = 0;
        
        // Measure children
        for (const auto& c : n.children) {
             Vec2 s = measure_node(c, constraintsW, constraintsH);
             if (s.x > usedW) usedW = s.x;
             usedH += s.y;
        }
        
        // Measure text content if children empty (or mix? assuming mix)
        if (!n.text.empty()) {
             std::string txt = trim(n.text);
             if (!txt.empty()) {
                 float scale = 1.0f;
                 if (n.attrs.count("fontSize")) {
                     float px = parse_dim(n.attrs.at("fontSize"), 100, 16);
                     scale = px / 16.0f; 
                 }
                 float tw = measure_text_width(txt, scale);
                 float th = 24.0f * scale; 
                 
                 if (tw > usedW) usedW = tw;
                 usedH += th;
             }
        }
        
        // Button padding default 10px? 20px?
        // Let's assume 10px if not set, but render_node handles defaults.
        // We add strict min size?
        // Button padding default 10px? 20px?
        // Match render_node: default 10px per side -> 20px total
        float padding = 10; 
        if (n.attrs.count("padding")) {
            padding = parse_dim(n.attrs.at("padding"), 0, 0);
        }

        return { (w > 0) ? w : (usedW + (padding*2)), (h > 0) ? h : (usedH + (padding*2)) };
    }
    else if (n.tag == "Column" || n.tag == "View" || n.tag == "Row") { // Row logic separate?
         // Merge Row? No, Row is horizontal. Column is vertical.
         // "View" is Column-like usually.
         // ...
         // Existing Row/Column logic is separate.
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
    else if (n.tag == "Row") {
         float totalW = 0;
         float maxH = 0;
         for (const auto& c : n.children) {
             Vec2 s = measure_node(c, constraintsW, constraintsH);
             totalW += s.x; 
             if (s.y > maxH) maxH = s.y;
         }
         return { (w > 0) ? w : totalW, (h > 0) ? h : maxH };
    }
    
    return { w, h };
}


void render_node(const Node& n, float x, float& y, double mx, double my, bool click, float windowW, float windowH) {
    float w = parse_dim(n.attrs.count("width") ? n.attrs.at("width") : "auto", windowW, 0);
    float h = parse_dim(n.attrs.count("height") ? n.attrs.at("height") : "auto", windowH, 0);

    // Helpers for styles
    // Helpers for styles
    Color textCol = {0.2f, 0.2f, 0.2f, 1}; // Default dark text
    if (n.tag == "Button") textCol = {1,1,1,1}; // Default white for button
    if (n.attrs.count("color")) textCol = parse_color(n.attrs.at("color"));
    

    float sizeScale = 1.0f;
    if (n.attrs.count("fontSize")) {
         float px = parse_dim(n.attrs.at("fontSize"), 100, 16);
         sizeScale = px / 16.0f; 
    }
    
    // Background Color & Styling
    float r = 0;
    if (n.tag == "Button") r = 4; // Default radius
    if (n.attrs.count("borderRadius")) r = parse_dim(n.attrs.at("borderRadius"), 0, 0);

    // Padding (Uniform for now)
    float padding = 0;
    if (n.tag == "Button") padding = 10; // Default padding
    if (n.attrs.count("padding")) padding = parse_dim(n.attrs.at("padding"), 0, 0);


    
    // Adjust logic for children to respect padding
    float childX = x + padding;
    float childY = y + padding;
    float childW = (w > 0) ? w - (padding*2) : windowW - (padding*2);
    // Note: Height adjustment is tricky for auto-height containers, usually we sum children then add padding at end.

    if (n.tag == "Page") {
        if (w <= 0) w = windowW;
        if (h <= 0) h = windowH;
        // Page ignores padding on itself for now, or applies it? Let's apply it.
        for (const auto& c : n.children) {
            render_node(c, childX, childY, mx, my, click, childW, h);
        }
    }
    
    else if (n.tag == "Scrollview") {
        float svW = (w > 0) ? w : windowW;
        float svH = (h > 0) ? h : windowH;
        // Background already drawn above if exists
        
        float startY = childY; 
        childY -= appState.scrollOffset; 

        float contentH = 0;
        for (const auto& c : n.children) contentH += measure_node(c, svW, svH).y;
        appState.maxScroll = (contentH > svH) ? (contentH - svH) : 0;

        for (const auto& c : n.children) {
            render_node(c, childX, childY, mx, my, click, svW, svH);
        }
        
        y = startY + svH - padding; // Restore Y but account for what we used
    }
    else if (n.tag == "Row") {
        float rowH = (h > 0) ? h : 0; 
        
        // Measure children
        std::vector<Vec2> sizes;
        float totalChildrenW = 0;
        float maxChildH = 0;
        for (const auto& c : n.children) {
            Vec2 s = measure_node(c, childW, windowH); // Constrain by padding
            sizes.push_back(s);
            totalChildrenW += s.x;
            if (s.y > maxChildH) maxChildH = s.y;
        }
        if (rowH == 0) rowH = maxChildH + (padding * 2);

        // Layout
        std::string justify = n.attrs.count("justifyContent") ? n.attrs.at("justifyContent") : "start";
        std::string align = n.attrs.count("alignItems") ? n.attrs.at("alignItems") : "top";
        
        float startX = childX;
        float gap = 0;
        int count = n.children.size();
        float freeSpace = childW - totalChildrenW;
        if (justify == "center") startX += freeSpace / 2;
        else if (justify == "right" || justify == "end") startX += freeSpace;
        else if (justify == "between" && count > 1) gap = freeSpace / (count - 1);
        else if (justify == "around" && count > 0) { gap = freeSpace / count; startX += gap / 2; }

        float cx = startX;
        float cy = childY;
        for (size_t i = 0; i < n.children.size(); i++) {
            float chH = sizes[i].y;
            float chY = cy;
            // Align relative to content height (rowH - 2*padding)
            float contentHeight = rowH - (padding*2);
            if (align == "center") chY += (contentHeight - chH) / 2;
            else if (align == "bottom") chY += (contentHeight - chH);
            
            render_node(n.children[i], cx, chY, mx, my, click, childW, windowH);
            cx += sizes[i].x + gap;
        }
        y += rowH;
    }
    else if (n.tag == "Column" || n.tag == "View" || n.tag == "Button") {
        
        // Defaults for Button
        std::string align = "left";
        std::string justify = "start";
        
        if (n.tag == "Button") {
             align = "center";
             justify = "center";
        }
        
        if (n.attrs.count("alignItems")) align = n.attrs.at("alignItems");
        if (n.attrs.count("justifyContent")) justify = n.attrs.at("justifyContent");



        // Measure content including text
        float totalChildrenH = 0;
        float maxChildW = 0; // Track max width for fit-content calculation
        std::vector<Vec2> sizes;
        
        // Track text node size if present
        Vec2 textSize = {0,0};
        bool hasText = !n.text.empty() && trim(n.text).size() > 0;
        if (hasText) {
             float scale = sizeScale; // Use parent's sizeScale
             std::string txt = resolve_binding(trim(n.text));
             textSize.x = measure_text_width(txt, scale);
             textSize.y = 24.0f * scale; // Line height approx
             
             sizes.push_back(textSize);
             totalChildrenH += textSize.y;
             if (textSize.x > maxChildW) maxChildW = textSize.x;
        }

        for (const auto& c : n.children) {
            Vec2 s = measure_node(c, childW, windowH);
            sizes.push_back(s);
            totalChildrenH += s.y;
             if (s.x > maxChildW) maxChildW = s.x;
        }

        float colH = (h > 0) ? h : (totalChildrenH + (padding * 2)); 
        // For Button, if auto width, use maxChildW + padding. For Column, use full width.
        float colW = (w > 0) ? w : (n.tag == "Button" ? (maxChildW + (padding*2)) : windowW); 
        // Enforce min width for button if needed?
        if (n.tag == "Button" && colW < maxChildW + (padding*2)) colW = maxChildW + (padding*2);

        float freeSpace = (colH - (padding * 2)) - totalChildrenH;
        // Safety check if content overflows
        if (freeSpace < 0) freeSpace = 0;
        
        // --- Deferred Background Drawing ---
        bool hasBg = n.attrs.count("backgroundColor");
        Color bg = {0,0,0,0};
        if (n.tag == "Button" && !hasBg) {
             bg = {0.0f, 0.55f, 0.73f, 1.0f}; // #008CBA
             hasBg = true;
        }
        
        if (hasBg) {
             if (n.attrs.count("backgroundColor")) {
                 std::string bgVal = trim(n.attrs.at("backgroundColor"));
                 if (bgVal == "transparent") bg = {0,0,0,0};
                 else bg = parse_color(bgVal);
             }
             
             // Shadow
             if (n.attrs.count("shadow") && n.attrs.at("shadow") == "true") {
                 draw_rounded_rect(x+2, y+2, colW, colH, r, {0,0,0,0.2f});
             }
             
             // Hover & Click
             if (n.attrs.count("onClick")) {
                 bool hover = mx > x && mx < x+colW && my > y && my < y+colH;
                 if (hover) {
                     // Visual feedback handled by color mod below
                 }
                 
                 // Trigger click action
                 if (hover && click) {
                     // We trigger ONLY if this is the "topmost" clickable?
                     // For now simple trigger.
                     trigger_click(n.attrs.at("onClick"));
                 }

                 if (n.tag == "Button") {
                     if (hover && !click) { bg.r *= 0.9f; bg.g *= 0.9f; bg.b *= 0.9f; }
                     if (hover && click) { bg.r *= 0.8f; bg.g *= 0.8f; bg.b *= 0.8f; }
                 }
             }
             
             if (colH > 0) {
                 if (r > 0) draw_rounded_rect(x, y, colW, colH, r, bg);
                 else rect(x, y, colW, colH, bg);
             }
        }
        // -----------------------------------

        float startY = childY;
        float gap = 0;
        int count = sizes.size(); // children + text

        if (justify == "center") startY += freeSpace / 2;
        else if (justify == "right" || justify == "end") startY += freeSpace; 
        else if (justify == "between" && count > 1) gap = freeSpace / (count - 1);
        else if (justify == "around" && count > 0) { gap = freeSpace / count; startY += gap / 2; }

        float cy = startY;
        
        // Render Text First
        if (hasText) {
             Vec2 s = textSize;
             // Calculate local centered X based on colW (not childW which was constraints)
             // Using colW - padding*2 as content area
             float contentW = colW - (padding*2);
             float cx = x + padding; 
             
             if (align == "center") cx += (contentW - s.x) / 2;
             else if (align == "right") cx += (contentW - s.x);
             
             std::string txt = resolve_binding(trim(n.text));
             Color drawCol = textCol;
             if (n.tag == "Button" && !n.attrs.count("color")) {
                  drawCol = {1,1,1,1}; 
             }
             
             // std::cout << "RenderText: '" << txt << "' at " << cx << "," << cy 
             //           << " Scale:" << sizeScale 
             //           << " Col:" << drawCol.r << "," << drawCol.g << "," << drawCol.b 
             //           << " Padding:" << padding << " ColW:" << colW << std::endl;

             draw_text(cx, cy, txt, sizeScale, drawCol);
             cy += s.y + gap;
        }

        for (size_t i = 0; i < n.children.size(); i++) {
            const auto& c = n.children[i];
            Vec2 s = sizes[hasText ? i + 1 : i];

            // Re-calc cx based on actual colW
             float contentW = colW - (padding*2);
             float cx = x + padding; 
             if (align == "center") cx += (contentW - s.x) / 2;
             else if (align == "right") cx += (contentW - s.x);
            
            float nodeY = cy;
            // Pass colW as width constraint for child? 
            // Or childW (windowW - padding)? childW was derived from constraints.
            // Let's pass contentW.
             float renderW = contentW;
            render_node(c, cx, nodeY, mx, my, click, renderW, windowH);
            cy += s.y + gap; 
        }
        
        if (h <= 0) y += totalChildrenH + (padding * 2);
        else y += h; 
    }
    // -- Widgets --
    else if (n.tag == "Text") {
        std::string rawTxt = trim(n.text);
        std::string txt = resolve_binding(rawTxt);
        
        // Text specific styling
        Color drawCol = textCol; 
        
        draw_text(x + padding, y + padding, txt, sizeScale, drawCol);
        y += (24 * sizeScale) + (padding * 2);
    } 
    else if (n.tag == "Textfield") {
        float tw = (w > 0) ? w : 300;
        float th = (h > 0) ? h : 40; // larger default
        draw_textbox(appState.notesBox, x, y, tw, th, mx, my, click);
        y += th;
    }
    else if (n.tag == "Image") {
         static std::map<std::string, unsigned int> textureCache;
         std::string src = n.attrs.count("src") ? n.attrs.at("src") : "";
         
         if (!src.empty()) {
             if (textureCache.find(src) == textureCache.end()) {
                 unsigned int tex = load_image(src.c_str());
                 if (tex != 0) textureCache[src] = tex;
             }
             
             if (textureCache.count(src)) {
                 float imgW = (w > 0) ? w : 50;
                 float imgH = (h > 0) ? h : 50;
                 draw_image(textureCache[src], x, y, imgW, imgH);
                 y += imgH;
             } else {
                 y += 50;
             }
         }
    }
}

