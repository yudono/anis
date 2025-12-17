#ifndef LAYOUT_H
#define LAYOUT_H

#include "types.h"

// Returns parsed pixel value or default based on constraints
float parse_dim(const std::string& val, float maxVal, float defaultVal);

// Measures the size a node takes
Vec2 measure_node(const Node& n, float constraintsW, float constraintsH);

// Recursive rendering function
void render_node(const Node& n, float x, float& y, double mx, double my, bool click, float windowW, float windowH);

#endif
