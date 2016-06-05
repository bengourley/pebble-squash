#include <pebble.h>
#include "windows/menu.h"
#include "persistence.h"

void init(void) {
  // clear_all();
  menu_window_push();
}

void deinit(void) {
}

// Match screen test

// #include "windows/match.h"
//
// void init(void) {
//   Settings settings = (Settings)
//     { .num_sets = 3
//     , .tie_breaks = YES
//     , .final_set = FINAL_SET_SIX_ALL_TIE_BREAK
//     , .first_server = PLAYER
//   };
//   list_t *serial = list_new();
//   for (int i = 0; i < 30; i++) add_player_points(serial);
//   for (int j = 0; j < 18; j++) add_opponent_points(serial);
//   for (int k = 0; k < 3; k++) add_player_points(serial);
//   match_window_push(&settings, serial);
// }

// Summary screen test
//
// #include "windows/summary.h"
// #include "state.h"
//
// static Settings s =
//   { .num_points = 11
//   , .num_games = 3
//   , .scoring_mode = EVERY_POINT
//   , .first_server = PLAYER
//   };
//
// void init(void) {
//   State state = state_new(&s);
//   state.player_games = 2;
//   state.opponent_games = 1;
//   state.completed_games[0][0] = 11;
//   state.completed_games[0][1] = 6;
//   state.completed_games[1][0] = 4;
//   state.completed_games[1][1] = 11;
//   state.completed_games[2][0] = 11;
//   state.completed_games[2][1] = 3;
//   state.match_start_time = time(NULL);
//   state.match_end_time = state.match_start_time + (1.2 * (60 * 60));
//   state.player_total_points = 84;
//   state.opponent_total_points = 167;
//   summary_window_push(state);
// }

int main(void) {
  init();
  app_event_loop();
  deinit();
}
