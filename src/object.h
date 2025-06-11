#ifndef OBJECT_H_INCLUDED
#define OBJECT_H_INCLUDED

#include "render.h"
#include <stdint.h>

typedef struct object_s object_t;

typedef void (*update_func_t)(object_t *obj, size_t idx, float time);

struct object_s {
  model_t *model;
  vec3f_t pos;
  float rot_y;
  float scale;
  float spawn_time;
  update_func_t update;
  uint8_t tag;
};

void handle_camera_movement(uint8_t gamepad, camera_t *camera);

object_t *spawn_object(model_t *model, float x, float y, float z, float rot_y,
                       float scale, float spawn_time, update_func_t func,
                       object_t *objects, size_t *obj_idx,
                       const size_t obj_len);
void remove_object(object_t *objects, size_t obj_idx, size_t *obj_count);

void object_matrix(object_t *object, matrix44f_t *dest);

void object_update(object_t *obj, size_t idx, float time);

#endif