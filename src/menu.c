#include "menu.h"
#include "nanoprintf.h"
#include "wasm4.h"
#include <string.h>

#define TEXT_BUFFER_LEN 256

// Global menu state variables
game_state_t game_state = GAME_STATE_MENU;
int selected_players = 2;
int menu_selection = 0;
int winner = -1;
uint32_t win_timer = 0;

// Global input state to prevent button press carry-over between states
static uint8_t prev_gamepad = 0;
static char text_buffer[TEXT_BUFFER_LEN];

void init_menu_system(void) {
  game_state = GAME_STATE_MENU;
  selected_players = 2;
  menu_selection = 0;
  winner = -1;
  win_timer = 0;
  prev_gamepad = 0;
}

void text_center(const char *label, int y) {
  text(label, (SCREEN_SIZE - strlen(label) * FONT_SIZE) / 2, y);
}

void draw_menu(void) {
  *DRAW_COLORS = 2;
  rect(0, 0, SCREEN_SIZE, SCREEN_SIZE);

  *DRAW_COLORS = 3;
  text_center("TANK WARS", 20);

  *DRAW_COLORS = (menu_selection == 0) ? 0x41 : 3;
  text_center("New Game", 60);

  *DRAW_COLORS = (menu_selection == 1) ? 0x41 : 3;
  text_center("Help", 80);

  *DRAW_COLORS = 3;
  text_center("UP/DOWN: Select", 120);
  text_center("X: Confirm", 130);
}

void draw_player_select(void) {
  *DRAW_COLORS = 2;
  rect(0, 0, SCREEN_SIZE, SCREEN_SIZE);

  *DRAW_COLORS = 3;
  text_center("Select Players", 20);

  for (int i = 2; i <= 4; i++) {
    *DRAW_COLORS = (selected_players == i) ? 0x41 : 3;
    npf_snprintf(text_buffer, sizeof(text_buffer), "%d Players", i);
    text_center(text_buffer, 40 + (i - 2) * 20);
  }

  *DRAW_COLORS = 3;
  text_center("Press Enter to", 100);
  text_center("copy Netplay URL", 110);
  text_center("UP/DOWN: Select", 130);
  text_center("X: Start, Z: Back", 140);
}

void draw_help(void) {
  *DRAW_COLORS = 2;
  rect(0, 0, SCREEN_SIZE, SCREEN_SIZE);

  *DRAW_COLORS = 3;
  int y = FONT_SIZE;
  int dy = FONT_SIZE + 1;
  int x = FONT_SIZE;
  text_center("TANK WARS - HELP", y);
  y += 2 * dy;
  text("Controls:", x, y);
  y += 2 * dy;
  text("Arrow Keys: Move", x, y);
  y += dy;
  text("Z: Slow Turn (Hold)", x, y);
  y += dy;
  text("X: Shoot", x, y);
  y += 2 * dy;

  text("Goal:", x, y);
  y += 2 * dy;
  text("First to 10 points", x, y);
  y += dy;
  text("wins!", x, y);
  y += 2 * dy;
  text_center("Z: Back", y);
}

void draw_win_screen(void) {
  *DRAW_COLORS = 2;
  rect(0, 0, SCREEN_SIZE, SCREEN_SIZE);

  *DRAW_COLORS = 3;
  npf_snprintf(text_buffer, sizeof(text_buffer), "PLAYER %d WINS!", winner + 1);
  text(text_buffer, 24, 60);

  int remaining = (WIN_DELAY - win_timer) / 60 + 1;
  npf_snprintf(text_buffer, sizeof(text_buffer), "Menu in %d...", remaining);
  text(text_buffer, 44, 80);
}

void update_menu(void) {
  const uint8_t pad = *GAMEPAD1;

  if ((pad & BUTTON_UP) && !(prev_gamepad & BUTTON_UP)) {
    menu_selection = (menu_selection - 1 + 2) % 2;
  }
  if ((pad & BUTTON_DOWN) && !(prev_gamepad & BUTTON_DOWN)) {
    menu_selection = (menu_selection + 1) % 2;
  }
  if ((pad & BUTTON_1) && !(prev_gamepad & BUTTON_1)) {
    if (menu_selection == 0) {
      game_state = GAME_STATE_PLAYER_SELECT;
    } else {
      game_state = GAME_STATE_HELP;
    }
  }

  prev_gamepad = pad;
}

void update_player_select(void) {
  const uint8_t pad = *GAMEPAD1;

  if ((pad & BUTTON_UP) && !(prev_gamepad & BUTTON_UP)) {
    selected_players = (selected_players - 1 < 2) ? 4 : selected_players - 1;
  }
  if ((pad & BUTTON_DOWN) && !(prev_gamepad & BUTTON_DOWN)) {
    selected_players = (selected_players + 1 > 4) ? 2 : selected_players + 1;
  }
  if ((pad & BUTTON_1) && !(prev_gamepad & BUTTON_1)) {
    extern void init_game(void);
    init_game();
    game_state = GAME_STATE_PLAYING;
  }
  if ((pad & BUTTON_2) && !(prev_gamepad & BUTTON_2)) {
    game_state = GAME_STATE_MENU;
  }

  prev_gamepad = pad;
}

void update_help(void) {
  const uint8_t pad = *GAMEPAD1;

  if ((pad & BUTTON_2) && !(prev_gamepad & BUTTON_2)) {
    game_state = GAME_STATE_MENU;
  }

  prev_gamepad = pad;
}

void update_win(void) {
  win_timer++;
  if (win_timer >= WIN_DELAY) {
    game_state = GAME_STATE_MENU;
    menu_selection = 0;
  }
}
