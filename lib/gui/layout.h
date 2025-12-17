#ifndef LAYOUT_H
#define LAYOUT_H

#include "types.h"

extern std::map<int, float>* g_activeTableWidths;
extern std::string g_basePath;

Color parse_color(const std::string& hex);

// Returns parsed pixel value or default based on constraints
float parse_dim(const std::string& val, float maxVal, float defaultVal);

struct BoxDims {
    float top, right, bottom, left;
};

BoxDims parse_box_dims(const std::string& val, float maxW, float maxH);

// Measures the size a node takes
Vec2 measure_node(const Node& n, float constraintsW, float constraintsH);

// Recursive rendering function
void render_node(const Node& n, float x, float& y, double mx, double my, bool click, float windowW, float windowH);

#endif
