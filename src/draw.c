#include "draw.h"
#include "wasm4.h"

void pixel(uint8_t x, uint8_t y, uint8_t color) {
  uint16_t byte_idx = y * 40 + (x / 4);
  uint8_t bit_offset = (x % 4) * 2;
  FRAMEBUFFER[byte_idx] &= ~(0x3 << bit_offset);        // Clear existing bits
  FRAMEBUFFER[byte_idx] |= (color & 0x3) << bit_offset; // Set new color
}

void bline(int x0, int y0, int x1, int y1);

void tri(int x0, int y0, int x1, int y1, int x2, int y2) {
  // Sort the vertices by y-coordinate ascending (y0 <= y1 <= y2)
  if (y0 > y1) {
    int tmp;
    tmp = y0;
    y0 = y1;
    y1 = tmp;
    tmp = x0;
    x0 = x1;
    x1 = tmp;
  }
  if (y1 > y2) {
    int tmp;
    tmp = y1;
    y1 = y2;
    y2 = tmp;
    tmp = x1;
    x1 = x2;
    x2 = tmp;
  }
  if (y0 > y1) {
    int tmp;
    tmp = y0;
    y0 = y1;
    y1 = tmp;
    tmp = x0;
    x0 = x1;
    x1 = tmp;
  }

  // Option 1: Draw the filled triangle first, then the outline separately
  // Fill the triangle
  int total_height = y2 - y0;
  for (int i = 0; i < total_height; i++) {
    int yi = y0 + i;
    int second_half = i > y1 - y0 || y1 == y0;
    int segment_height = second_half ? y2 - y1 : y1 - y0;
    float alpha = (float)i / total_height;
    float beta = (float)(i - (second_half ? y1 - y0 : 0)) / segment_height;

    int ax = x0 + (x2 - x0) * alpha;
    int bx = second_half ? x1 + (x2 - x1) * beta : x0 + (x1 - x0) * beta;

    if (ax > bx) {
      int tmp = ax;
      ax = bx;
      bx = tmp;
    }

    // Draw horizontal line for filling
    if (yi >= 0 && yi < SCREEN_SIZE) {
      hline(ax, yi, bx - ax + 1); // +1 to include the endpoint
    }
  }

  // Draw the outline separately using line algorithm
  bline(x0, y0, x1, y1);
  bline(x1, y1, x2, y2);
  bline(x2, y2, x0, y0);
}

// Bresenham's line algorithm for the outline
void bline(int x0, int y0, int x1, int y1) {
  int steep = 0;
  if (abs(x0 - x1) < abs(y0 - y1)) {
    steep = 1;
    // Swap x and y coordinates
    int tmp;
    tmp = x0;
    x0 = y0;
    y0 = tmp;
    tmp = x1;
    x1 = y1;
    y1 = tmp;
  }

  if (x0 > x1) {
    // Make sure x is increasing
    int tmp;
    tmp = x0;
    x0 = x1;
    x1 = tmp;
    tmp = y0;
    y0 = y1;
    y1 = tmp;
  }

  int dx = x1 - x0;
  int dy = abs(y1 - y0);
  int error = dx / 2;
  int ystep = (y0 < y1) ? 1 : -1;
  int y = y0;

  for (int x = x0; x <= x1; x++) {
    if (steep) {
      if (y >= 0 && y < SCREEN_SIZE && x >= 0 && x < SCREEN_SIZE) {
        pixel(y, x, *DRAW_COLORS);
      }
    } else {
      if (x >= 0 && x < SCREEN_SIZE && y >= 0 && y < SCREEN_SIZE) {
        pixel(x, y, *DRAW_COLORS);
      }
    }

    error -= dy;
    if (error < 0) {
      y += ystep;
      error += dx;
    }
  }
}

int is_point_visible(const vec2i_t *p) {
  return p->x >= 0 && p->x < SCREEN_SIZE && p->y >= 0 && p->y < SCREEN_SIZE;
}

int is_triangle_visible(const vec2i_t *r0, const vec2i_t *r1,
                        const vec2i_t *r2) {
  if (is_point_visible(r0) || is_point_visible(r1) || is_point_visible(r2)) {
    return 1;
  }
  return 0;
}
