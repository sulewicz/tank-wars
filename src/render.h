#ifndef RENDER_H_INCLUDED
#define RENDER_H_INCLUDED

#include "draw.h"

typedef struct {
  float x, y, z;
} vec3f_t;

typedef struct {
  float m[4][4];
} matrix44f_t;

typedef struct {
  const uint32_t *tris;
  const vec3f_t *verts;
  const size_t tris_count;
} model_t;

typedef struct {
  vec2i_t raster_verts[3]; // Vertices in screen space
  float depth;             // Average Z depth in camera space
} polygon_t;

typedef struct {
  vec3f_t pos;
  float yaw;
  float pitch;
  float movement_speed;
  float rotation_speed;
} camera_t;

void mult_vec_matrix(const vec3f_t *src, vec3f_t *dst, const matrix44f_t *mat);
vec3f_t project_vertex(const vec3f_t *world, vec2i_t *raster,
                       const matrix44f_t *world_to_camera, float canvas_width,
                       float canvas_height, float image_width,
                       float image_height);
matrix44f_t build_camera_matrix(camera_t *camera);
matrix44f_t inverse_matrix44f(const matrix44f_t *mat);
int is_behind_camera(const vec3f_t *v0, const vec3f_t *v1, const vec3f_t *v2,
                     const matrix44f_t *world_to_camera);
void mult_matrices(const matrix44f_t *a, const matrix44f_t *b,
                   matrix44f_t *result);
void create_translation_matrix(float x, float y, float z, float scale,
                               matrix44f_t *dest);
void create_rotation_y_matrix(float angle, matrix44f_t *dest);
void buffer_model(model_t *model, matrix44f_t *transform,
                  matrix44f_t *world_to_camera, polygon_t *buffer,
                  size_t *buf_idx, const size_t buf_len);
void render_buffer(polygon_t *buffer, size_t buf_len);
float vec3f_xz_distance(const vec3f_t v1, const vec3f_t v2);

#endif
