#include "object.h"

object_t create_object(model_t *model, float x, float y, float z, float rot_y,
                       float scale, float spawn_time, update_func_t func) {
  object_t ret = {.model = model,
                  .update = func,
                  .rot_y = rot_y,
                  .scale = scale,
                  .spawn_time = spawn_time,
                  .tag = 0,
                  .pos = {.x = x, .y = y, .z = z}};
  return ret;
}

object_t *spawn_object(model_t *model, float x, float y, float z, float angle,
                       float scale, float spawn_time, update_func_t func,
                       object_t *objects, size_t *obj_idx,
                       const size_t obj_len) {
  object_t *ret = NULL;
  if (*obj_idx < obj_len) {
    objects[*obj_idx] =
        create_object(model, x, y, z, angle, scale, spawn_time, func);
    ret = &objects[*obj_idx];
    (*obj_idx)++;
  }
  return ret;
}

void remove_object(object_t *objects, size_t obj_idx, size_t *obj_count) {
  if (*obj_count == 0) {
    return;
  }
  if (obj_idx < *obj_count - 1) {
    // Defragmenting
    objects[obj_idx] = objects[*obj_count - 1];
  }
  (*obj_count)--;
}

void object_matrix(object_t *object, matrix44f_t *dest) {
  static matrix44f_t translate;
  static matrix44f_t rotate;
  create_translation_matrix(object->pos.x, object->pos.y, object->pos.z,
                            object->scale, &translate);
  create_rotation_y_matrix(object->rot_y, &rotate);
  mult_matrices(&rotate, &translate, dest);
}

void object_update(object_t *obj, size_t idx, float time) {
  if (obj->update != NULL) {
    obj->update(obj, idx, time);
  }
}
