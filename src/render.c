#include "render.h"
#include "draw.h"

#define SCREEN_SIZE 160

#include <math.h>

void mult_vec_matrix(const vec3f_t *src, vec3f_t *dst, const matrix44f_t *mat) {
  float a = src->x * mat->m[0][0] + src->y * mat->m[1][0] +
            src->z * mat->m[2][0] + mat->m[3][0];
  float b = src->x * mat->m[0][1] + src->y * mat->m[1][1] +
            src->z * mat->m[2][1] + mat->m[3][1];
  float c = src->x * mat->m[0][2] + src->y * mat->m[1][2] +
            src->z * mat->m[2][2] + mat->m[3][2];
  float w = src->x * mat->m[0][3] + src->y * mat->m[1][3] +
            src->z * mat->m[2][3] + mat->m[3][3];

  dst->x = a / w;
  dst->y = b / w;
  dst->z = c / w;
}

vec3f_t project_vertex(const vec3f_t *world, vec2i_t *raster,
                       const matrix44f_t *world_to_camera, float canvas_width,
                       float canvas_height, float image_width,
                       float image_height) {
  vec3f_t camera;
  mult_vec_matrix(world, &camera, world_to_camera);

  vec2f_t screen;
  screen.x = camera.x / -camera.z;
  screen.y = camera.y / -camera.z;

  vec2f_t ndc;
  ndc.x = (screen.x + canvas_width * 0.5f) / canvas_width;
  ndc.y = (screen.y + canvas_height * 0.5f) / canvas_height;

  raster->x = (int)(ndc.x * image_width);
  raster->y = (int)((1.0f - ndc.y) * image_height);
  return camera;
}

matrix44f_t build_camera_matrix(camera_t *camera) {
  float cos_yaw = cosf(camera->yaw);
  float sin_yaw = sinf(camera->yaw);
  float cos_pitch = cosf(camera->pitch);
  float sin_pitch = sinf(camera->pitch);

  vec3f_t forward = {cos_yaw * cos_pitch, sin_pitch, sin_yaw * cos_pitch};

  // Right is perpendicular to forward (cross product of forward and world up)
  vec3f_t right = {sin_yaw, 0,
                   -cos_yaw}; // This is right = cross(forward, world_up)

  // Up is perpendicular to both forward and right
  vec3f_t up = {cos_yaw * sin_pitch, cos_pitch, sin_yaw * sin_pitch};

  matrix44f_t matrix = {
      .m = {{right.x, up.x, -forward.x, 0},
            {right.y, up.y, -forward.y, 0},
            {right.z, up.z, -forward.z, 0},
            {camera->pos.x, camera->pos.y, camera->pos.z, 1}}};

  return matrix;
}

matrix44f_t inverse_matrix44f(const matrix44f_t *mat) {
  matrix44f_t result = {{{0}}};
  matrix44f_t t = *mat;
  matrix44f_t s = {
      .m = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
  int i, j, k;

  for (i = 0; i < 3; i++) {
    int pivot = i;
    float pivotsize = (float)fabs(t.m[i][i]);
    for (j = i + 1; j < 4; j++) {
      float tmp = (float)fabs(t.m[j][i]);
      if (tmp > pivotsize) {
        pivot = j;
        pivotsize = tmp;
      }
    }

    if (pivotsize == 0)
      return result;

    if (pivot != i) {
      for (j = 0; j < 4; j++) {
        float tmp = t.m[i][j];
        t.m[i][j] = t.m[pivot][j];
        t.m[pivot][j] = tmp;

        tmp = s.m[i][j];
        s.m[i][j] = s.m[pivot][j];
        s.m[pivot][j] = tmp;
      }
    }

    for (j = i + 1; j < 4; j++) {
      float f = t.m[j][i] / t.m[i][i];
      for (k = 0; k < 4; k++) {
        t.m[j][k] -= f * t.m[i][k];
        s.m[j][k] -= f * s.m[i][k];
      }
    }
  }

  for (i = 3; i >= 0; --i) {
    float f = t.m[i][i];
    if (f == 0)
      return result;

    for (j = 0; j < 4; j++) {
      t.m[i][j] /= f;
      s.m[i][j] /= f;
    }

    for (j = 0; j < i; j++) {
      f = t.m[j][i];
      for (k = 0; k < 4; k++) {
        t.m[j][k] -= f * t.m[i][k];
        s.m[j][k] -= f * s.m[i][k];
      }
    }
  }
  return s;
}

int is_behind_camera(const vec3f_t *v0, const vec3f_t *v1, const vec3f_t *v2,
                     const matrix44f_t *world_to_camera) {
  // Transform vertices to camera space
  vec3f_t c0, c1, c2;
  mult_vec_matrix(v0, &c0, world_to_camera);
  mult_vec_matrix(v1, &c1, world_to_camera);
  mult_vec_matrix(v2, &c2, world_to_camera);

  const float near_threshold = -1.f;
  return c0.z > near_threshold || c1.z > near_threshold ||
         c2.z > near_threshold;
}

void create_translation_matrix(float x, float y, float z, float scale,
                               matrix44f_t *dest) {
  dest->m[0][0] = scale;
  dest->m[0][1] = 0.0f;
  dest->m[0][2] = 0.0f;
  dest->m[0][3] = 0.0f;

  dest->m[1][0] = 0.0f;
  dest->m[1][1] = scale;
  dest->m[1][2] = 0.0f;
  dest->m[1][3] = 0.0f;

  dest->m[2][0] = 0.0f;
  dest->m[2][1] = 0.0f;
  dest->m[2][2] = scale;
  dest->m[2][3] = 0.0f;

  dest->m[3][0] = x;
  dest->m[3][1] = y;
  dest->m[3][2] = z;
  dest->m[3][3] = 1.0f;
}

void create_rotation_y_matrix(float angle, matrix44f_t *dest) {
  float c = cosf(angle);
  float s = sinf(angle);
  dest->m[0][0] = c;
  dest->m[0][1] = 0.0f;
  dest->m[0][2] = -s;
  dest->m[0][3] = 0.0f;

  dest->m[1][0] = 0.0f;
  dest->m[1][1] = 1.0f;
  dest->m[1][2] = 0.0f;
  dest->m[1][3] = 0.0f;

  dest->m[2][0] = s;
  dest->m[2][1] = 0.0f;
  dest->m[2][2] = c;
  dest->m[2][3] = 0.0f;

  dest->m[3][0] = 0.0f;
  dest->m[3][1] = 0.0f;
  dest->m[3][2] = 0.0f;
  dest->m[3][3] = 1.0f;
}

// Multiply two matrices
void mult_matrices(const matrix44f_t *a, const matrix44f_t *b,
                   matrix44f_t *result) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result->m[i][j] = 0;
      for (int k = 0; k < 4; k++) {
        result->m[i][j] += a->m[i][k] * b->m[k][j];
      }
    }
  }
}

void buffer_model(model_t *model, matrix44f_t *transform,
                  matrix44f_t *world_to_camera, polygon_t *buffer,
                  size_t *buf_idx, const size_t buf_len) {
  for (size_t i = 0; i < model->tris_count && *buf_idx < buf_len; ++i) {
    vec3f_t v0 = model->verts[model->tris[i * 3]];
    vec3f_t v1 = model->verts[model->tris[i * 3 + 1]];
    vec3f_t v2 = model->verts[model->tris[i * 3 + 2]];

    mult_vec_matrix(&v0, &v0, transform);
    mult_vec_matrix(&v1, &v1, transform);
    mult_vec_matrix(&v2, &v2, transform);

    if (is_behind_camera(&v0, &v1, &v2, world_to_camera)) {
      continue;
    }

    vec2i_t r0, r1, r2;
    vec3f_t c0, c1, c2;
    c0 = project_vertex(&v0, &r0, world_to_camera, 2.0f, 2.0f, SCREEN_SIZE,
                        SCREEN_SIZE);
    c1 = project_vertex(&v1, &r1, world_to_camera, 2.0f, 2.0f, SCREEN_SIZE,
                        SCREEN_SIZE);
    c2 = project_vertex(&v2, &r2, world_to_camera, 2.0f, 2.0f, SCREEN_SIZE,
                        SCREEN_SIZE);
    // Skip triangles completely outside the screen
    if (!is_triangle_visible(&r0, &r1, &r2)) {
      continue;
    }
    buffer[*buf_idx].raster_verts[0] = r0;
    buffer[*buf_idx].raster_verts[1] = r1;
    buffer[*buf_idx].raster_verts[2] = r2;
    buffer[*buf_idx].depth = (c0.z + c1.z + c2.z) / 3.0f;
    (*buf_idx)++;
  }
}

int compare_triangles(const void *a, const void *b) {
  const polygon_t *t1 = (const polygon_t *)a;
  const polygon_t *t2 = (const polygon_t *)b;

  if (t1->depth > t2->depth)
    return 1;
  if (t1->depth < t2->depth)
    return -1;
  return 0;
}

void render_buffer(polygon_t *buffer, size_t buf_len) {
  qsort(buffer, buf_len, sizeof(polygon_t), compare_triangles);
  for (size_t i = 0; i < buf_len; i++) {
    polygon_t *polygon = &buffer[i];
    vec2i_t r0, r1, r2;
    r0 = polygon->raster_verts[0];
    r1 = polygon->raster_verts[1];
    r2 = polygon->raster_verts[2];
    tri(r0.x, r0.y, r1.x, r1.y, r2.x, r2.y);
  }
}

float vec3f_xz_distance(const vec3f_t v1, const vec3f_t v2) {
  // Calculate the squared differences in x and z coordinates
  float dx = v1.x - v2.x;
  float dz = v1.z - v2.z;

  // Return the Euclidean distance (square root of sum of squared differences)
  return sqrtf(dx * dx + dz * dz);
}
