#ifndef RENDERER_H
#define RENDERER_H

#include "types.h"
#include <string>
#ifdef __APPLE__
#include <OpenGL/gl.h> 
#else
#include <GL/gl.h>
#endif

void init_freetype();
void draw_text(float x, float y, const std::string& text, float scale = 1.0f, Color color = {1,1,1,1});
float measure_text_width(const std::string& text, float scale = 1.0f);

unsigned int load_image(const char* path);
void draw_image(unsigned int tex, float x, float y, float w, float h);

void rect(float x, float y, float w, float h, Color c = {1,1,1,1});
void draw_rounded_rect(float x, float y, float w, float h, float r, Color c);
Color parse_color(const std::string& hex);

#endif
