#ifndef STATE_H
#define STATE_H

#include <stdbool.h>
#include "serial.h"
#include "../deps/list/list.h"

typedef struct {

  // Match state
  int player_points;
  int opponent_points;
  int player_games;
  int opponent_games;
  bool is_complete;

  // Settings
  int num_games;
  int num_points;
  int scoring_mode;
  int server;

  // Collected stats
  int match_start_time;
  int match_end_time;
  int player_total_points;
  int opponent_total_points;
  int **completed_games;

} State;

typedef struct {
  int *points;
  int *games;
  bool is_serving;
  bool is_player;
} Player;

typedef enum {
  PLAYER=0,
  OPPONENT=1
} SERVER;

typedef struct {
  int num_games;
  int num_points;
  int scoring_mode;
  int first_server;
} Settings;

typedef enum {
  YES=0,
  NO=1
} SWITCH_SETTING;

typedef enum {
  EVERY_POINT,
  ONLY_ON_SERVE
} SCORING_MODE;

State compute_state(list_t *serial, Settings *settings);
State state_new(Settings *settings);
void state_destroy(State *state);
void next_state(State *s, Point *point);
void debug_state(State *s);
void increment_point(State *s, Player *scorer, Player *non_scorer);
void increment_game(State *s, Player *scorer, Player *non_scorer);

#endif
