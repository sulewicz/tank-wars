#include "wasm4.h"

#include "menu.h"
#include "models.h"
#include "nanoprintf.h"
#include "object.h"
#include "render.h"
#include <math.h>
#include <stdint.h>

#define TEXT_BUFFER_LEN 256
#define POLYGON_BUFFER_LEN 1024
#define OBJECTS_LEN 128
#define CAMERA_OFFSET 15.f
#define PLAYER_COUNT 4

#define TANK_SCALE 2.f
#define TANK_COLLISION_RADIUS (TANK_SCALE * 5.f)

#define SHOT_DELAY 3.f

char text_buffer[TEXT_BUFFER_LEN];
polygon_t polygon_buffer[POLYGON_BUFFER_LEN];
object_t objects[OBJECTS_LEN];
camera_t cameras[PLAYER_COUNT];
uint16_t score[PLAYER_COUNT] = {0, 0, 0, 0};
size_t object_count = 0;
float shot_time[4] = {-SHOT_DELAY, -SHOT_DELAY, -SHOT_DELAY, -SHOT_DELAY};
uint32_t tick = 0;

void transformation_debug(object_t *obj, size_t obj_idx __attribute__((unused)),
                          float time) {
  obj->pos.x = cosf(time * M_PI) * 30;
  obj->rot_y = 2.f * M_PI * time / 2.0f;
  obj->scale = 0.5f + 0.5f * sinf(2 * time * M_PI);
}

void init_game() {
  object_count = 0;
  for (int i = 0; i < PLAYER_COUNT; i++) {
    score[i] = 0;
    shot_time[i] = -SHOT_DELAY;
  }

  // Spawn tanks based on selected player count
  float positions[][2] = {{100, 100}, {-100, 100}, {100, -100}, {-100, -100}};
  float rotations[] = {0.75f * M_PI, 0.25f * M_PI, -0.75f * M_PI,
                       -0.25f * M_PI};

  for (int i = 0; i < selected_players; i++) {
    spawn_object(&tank_model, positions[i][0], 0, positions[i][1], rotations[i],
                 TANK_SCALE, 0.f, NULL, objects, &object_count, OBJECTS_LEN);
  }

  spawn_object(&cube_model, 0, 0, 0, 0, 1.f, 0.f, transformation_debug, objects,
               &object_count, OBJECTS_LEN);

  for (int i = 0; i < selected_players; i++) {
    cameras[i].pos = objects[i].pos;
    cameras[i].pos.y = CAMERA_OFFSET;
    cameras[i].yaw = -objects[i].rot_y;
    cameras[i].pitch = 0.f;
    cameras[i].movement_speed = 0.5f;
    cameras[i].rotation_speed = 0.05f;
  }
}

void start() { init_menu_system(); }

void update_explosion(object_t *obj, size_t obj_idx, float time) {
  float life_time = time - obj->spawn_time;
  obj->scale = 4.f + sinf(life_time * M_PI / 0.5f) * 20.f;
  if (life_time >= 0.5f) {
    remove_object(objects, obj_idx, &object_count);
  }
}

void update_projectile(object_t *obj, size_t obj_idx, float time) {
  float cos_yaw = cosf(-obj->rot_y);
  float sin_yaw = sinf(-obj->rot_y);
  float speed = 2.f;
  vec3f_t forward = {cos_yaw, 0, sin_yaw};
  obj->pos.x += forward.x * speed;
  obj->pos.z += forward.z * speed;

  if (time - obj->spawn_time > 3.f) {
    remove_object(objects, obj_idx, &object_count);
  } else {
    for (int i = 0; i < selected_players; i++) {
      uint8_t owner = obj->tag;
      if (owner == i) {
        continue; // Not colliding with the player who shot.
      } else {
        object_t *tank = &objects[i];
        float distance = vec3f_xz_distance(obj->pos, tank->pos);
        if (distance < TANK_COLLISION_RADIUS) {
          spawn_object(&explosion_model, obj->pos.x, 8.f * TANK_SCALE,
                       obj->pos.z, 0, 4.f, time, update_explosion, objects,
                       &object_count, OBJECTS_LEN);
          remove_object(objects, obj_idx, &object_count);
          score[owner]++;
          tone(300 | (110 << 16), 30, 40, 3);

          // Check win condition
          if (score[owner] >= WIN_SCORE) {
            winner = owner;
            game_state = GAME_STATE_WIN;
            win_timer = 0;
          }
          break;
        }
      }
    }
  }
}

size_t current_player_id() {
  if (*NETPLAY & 0b100) {
    size_t netplay_id = *NETPLAY & 0b011;
    return (netplay_id < (size_t)selected_players) ? netplay_id : 0;
  }
  return 0;
}

void handle_camera_movement(uint8_t gamepad, camera_t *camera) {
  // Calculate forward and right vectors based on camera orientation
  float cos_yaw = cosf(camera->yaw);
  float sin_yaw = sinf(camera->yaw);

  vec3f_t forward = {cos_yaw, 0, sin_yaw};

  // Move forward/backward (UP/DOWN)
  if (gamepad & BUTTON_UP) {
    camera->pos.x += forward.x * camera->movement_speed;
    camera->pos.y += forward.y * camera->movement_speed;
    camera->pos.z += forward.z * camera->movement_speed;
  }
  if (gamepad & BUTTON_DOWN) {
    camera->pos.x -= forward.x * camera->movement_speed;
    camera->pos.y -= forward.y * camera->movement_speed;
    camera->pos.z -= forward.z * camera->movement_speed;
  }

  float rotation_speed = camera->rotation_speed;
  if (gamepad & BUTTON_1) {
    rotation_speed /= 2.f;
  }
  if (gamepad & BUTTON_LEFT) {
    camera->yaw += rotation_speed;
  }
  if (gamepad & BUTTON_RIGHT) {
    camera->yaw -= rotation_speed;
  }
}

void update_game() {
  float time = tick / 60.f;

  // Input and game logic.
  size_t player_id = current_player_id();
  for (int i = 0; i < selected_players; i++) {
    const uint8_t pad = *(GAMEPAD1 + i);
    object_t *player_object = &objects[i];
    handle_camera_movement(pad, &cameras[i]);
    player_object->pos = cameras[i].pos;
    player_object->pos.y -= CAMERA_OFFSET;
    player_object->rot_y = -cameras[i].yaw;
    if (pad & BUTTON_2 && time - shot_time[i] > SHOT_DELAY) {
      shot_time[i] = time;
      object_t *obj = spawn_object(
          &projectile_model, player_object->pos.x, player_object->pos.y,
          player_object->pos.z, player_object->rot_y, TANK_SCALE, time,
          update_projectile, objects, &object_count, OBJECTS_LEN);
      tone(60 | (40 << 16), 10, 40, 0); // Low-frequency pulse wave
      obj->tag = (uint8_t)i;
    }
  }

  // Rendering.
  *DRAW_COLORS = 2;
  rect(0, 80, 160, 80);
  matrix44f_t camera_to_world = build_camera_matrix(&cameras[player_id]);
  matrix44f_t world_to_camera = inverse_matrix44f(&camera_to_world);

  size_t buf_idx = 0;
  static matrix44f_t transform;
  size_t i = object_count;
  while (i-- > 0) {
    // Skip the current player's tank (first selected_players objects are tanks)
    if (i < (size_t)selected_players && i == player_id) {
      continue;
    }
    object_t *object = &objects[i];
    object_matrix(object, &transform);
    buffer_model(object->model, &transform, &world_to_camera, polygon_buffer,
                 &buf_idx, POLYGON_BUFFER_LEN);
    object_update(object, i, time);
  }

  *DRAW_COLORS = 0x43;
  render_buffer(polygon_buffer, buf_idx);

  // UI.
  *DRAW_COLORS = 0x32;
  rect(78, 76, 4, 4);

  *DRAW_COLORS = 3;
  int text_len;

  // Only show scores for active players
  for (int i = 0; i < selected_players; i++) {
    text_len = npf_snprintf(text_buffer, sizeof(text_buffer), "P%d: %02d",
                            i + 1, score[i]);

    if (i == 0) {
      text(text_buffer, 1, 1);
    } else if (i == 1) {
      text(text_buffer, SCREEN_SIZE - text_len * FONT_SIZE, 1);
    } else if (i == 2) {
      text(text_buffer, 1, SCREEN_SIZE - FONT_SIZE);
    } else if (i == 3) {
      text(text_buffer, SCREEN_SIZE - text_len * FONT_SIZE,
           SCREEN_SIZE - FONT_SIZE);
    }
  }

  float shot_cooldown = time - shot_time[current_player_id()];
  if (shot_cooldown > SHOT_DELAY) {
    text("OK", SCREEN_SIZE / 2 - FONT_SIZE, SCREEN_SIZE - FONT_SIZE);
  } else {
    npf_snprintf(text_buffer, sizeof(text_buffer), "%02d",
                 (int)(1 + SHOT_DELAY - shot_cooldown));
    text(text_buffer, SCREEN_SIZE / 2 - FONT_SIZE, SCREEN_SIZE - FONT_SIZE);
  }
}

void update() {
  switch (game_state) {
  case GAME_STATE_MENU:
    update_menu();
    draw_menu();
    break;
  case GAME_STATE_PLAYER_SELECT:
    update_player_select();
    draw_player_select();
    break;
  case GAME_STATE_HELP:
    update_help();
    draw_help();
    break;
  case GAME_STATE_PLAYING:
    update_game();
    break;
  case GAME_STATE_WIN:
    update_win();
    draw_win_screen();
    break;
  }

  tick++;
}
