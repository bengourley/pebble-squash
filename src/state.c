#include <pebble.h>
#include "state.h"
#include "../deps/list/list.h"

State state_new(Settings *settings) {

  // Allocate memory for tracking previous sets
  int **completed_games = (int **) malloc(settings->num_games * sizeof(int*));
  for (int i = 0; i < settings->num_games; i++) {
    completed_games[i] = (int *) malloc(4 * sizeof(int*));
    completed_games[i][0] = 0;
    completed_games[i][1] = 0;
  }

  return (State)
    { .player_points = 0
    , .opponent_points = 0
    , .player_games = 0
    , .opponent_games = 0

    , .num_games = settings->num_games
    , .num_points = settings->num_points
    , .scoring_mode = settings->scoring_mode
    , .server = settings->first_server

    // Collected stats
    , .match_start_time = -1
    , .match_end_time = -1
    , .player_total_points = 0
    , .completed_games = completed_games

    };

}

void state_destroy(State *s) {
  for (int i = 0; i < s->num_games; i++) free(s->completed_games[i]);
  free(s->completed_games);
}

State compute_state(list_t *serial, Settings *settings) {
  list_node_t *node;
  list_iterator_t *it = list_iterator_new(serial, LIST_HEAD);
  State state = state_new(settings);
  while ((node = list_iterator_next(it))) next_state(&state, (Point *) node->val);
  list_iterator_destroy(it);
  return state;
}

void next_state(State *s, Point *point) {

  if (s->match_start_time == -1) s->match_start_time = point->timestamp;
  s->match_end_time = point->timestamp;

  bool is_player_point = point->scorer == PLAYER_SCORE;

  Player scorer =
    { .points = is_player_point ? &s->player_points : &s->opponent_points
    , .games = is_player_point ? &s->player_games : &s->opponent_games
    , .is_serving = is_player_point ? s->server == PLAYER : s->server == OPPONENT
    , .is_player = is_player_point
    };

  Player non_scorer =
    { .points = is_player_point ? &s->opponent_points : &s->player_points
    , .games = is_player_point ? &s->opponent_games : &s->player_games
    , .is_serving = is_player_point ? s->server == OPPONENT : s->server == PLAYER
    , .is_player = !is_player_point
    };

  increment_point(s, &scorer, &non_scorer);

}

void toggle_server(State *s) {
  // Toggle the server
  if (s->server == PLAYER) {
    s->server = OPPONENT;
  } else if (s->server == OPPONENT) {
    s->server = PLAYER;
  }
}

void increment_point(State *s, Player *scorer, Player *non_scorer) {

  // Short circuit, don't increment point when only scoring on serve
  if (s->scoring_mode == ONLY_ON_SERVE && !scorer->is_serving) {
    if (!scorer->is_serving) toggle_server(s);
    return;
  }

  if (!scorer->is_serving) toggle_server(s);

  scorer->is_player ? s->player_total_points++ : s->opponent_total_points++;

  if (*scorer->points < s->num_points - 1) {
    *scorer->points = *scorer->points + 1;
    return;
  } else if (*scorer->points - *non_scorer->points < 1) {
    *scorer->points = *scorer->points + 1;
    return;
  } else {
    *scorer->points = *scorer->points + 1;
    increment_game(s, scorer, non_scorer);
    return;
  }

}

void increment_game(State *s, Player *scorer, Player *non_scorer) {

  int current_game = s->player_games + s->opponent_games;
  s->completed_games[current_game][scorer->is_player ? 0 : 1] = *scorer->points;
  s->completed_games[current_game][scorer->is_player ? 1 : 0] = *non_scorer->points;
  *scorer->points = 0;
  *non_scorer->points = 0;
  *scorer->games = *scorer->games + 1;
  if (*scorer->games > s->num_games - *scorer->games) s->is_complete = true;

}

void debug_state(State *s) {

  char player_points[4];
  char opponent_points[4];

  snprintf(player_points, 3, "%d", s->player_points);
  snprintf(opponent_points, 3, "%d", s->opponent_points);

  APP_LOG(APP_LOG_LEVEL_INFO, "%d game match", s->num_games);

  APP_LOG(APP_LOG_LEVEL_INFO, "%s-%s, GAMES: %d-%d"
    , player_points, opponent_points
    , s->player_games, s->opponent_games);

  APP_LOG(APP_LOG_LEVEL_INFO, "Total points. Player: %d, Opponent: %d"
    , s->player_total_points, s->opponent_total_points);

  for (int i = 0; i < s->num_games; i++) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Set %d: %d-%d", i, s->completed_games[i][0], s->completed_games[i][1]);
  }

}
