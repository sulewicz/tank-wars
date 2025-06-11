#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#include <stdint.h>

#define WIN_SCORE 10
#define WIN_DELAY 180 // 3 seconds at 60fps

typedef enum {
  GAME_STATE_MENU,
  GAME_STATE_PLAYER_SELECT,
  GAME_STATE_HELP,
  GAME_STATE_PLAYING,
  GAME_STATE_WIN
} game_state_t;

// Global menu state variables
extern game_state_t game_state;
extern int selected_players;
extern int menu_selection;
extern int winner;
extern uint32_t win_timer;

// Menu drawing functions
void draw_menu(void);
void draw_player_select(void);
void draw_help(void);
void draw_win_screen(void);

// Menu update functions
void update_menu(void);
void update_player_select(void);
void update_help(void);
void update_win(void);

// Menu initialization
void init_menu_system(void);

#endif
