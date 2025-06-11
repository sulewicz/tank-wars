#ifndef DRAW_H_INCLUDED
#define DRAW_H_INCLUDED

#include <stdlib.h>

typedef struct {
    float x, y;
  } vec2f_t;
  
  typedef struct {
    int x, y;
  } vec2i_t;

void pixel(uint8_t x, uint8_t y, uint8_t color);
void tri(int x0, int y0, int x1, int y1, int x2, int y2);

int is_point_visible(const vec2i_t *p);
int is_triangle_visible(const vec2i_t *r0, const vec2i_t *r1,
    const vec2i_t *r2);

#endif