#include "persistence.h"

void load_settings(Settings *settings) {
  if (persist_exists(NUM_POINTS_SETTING_KEY)) settings->num_points = persist_read_int(NUM_POINTS_SETTING_KEY);
  if (persist_exists(NUM_GAMES_SETTING_KEY)) settings->num_games = persist_read_int(NUM_GAMES_SETTING_KEY);
  if (persist_exists(SCORING_MODE_SETTING_KEY)) settings->scoring_mode = persist_read_int(SCORING_MODE_SETTING_KEY);
}

void save_settings(Settings *settings) {
  persist_write_int(NUM_POINTS_SETTING_KEY, settings->num_points);
  persist_write_int(NUM_GAMES_SETTING_KEY, settings->num_games);
  persist_write_int(SCORING_MODE_SETTING_KEY, settings->scoring_mode);
}

void clear_match() {
}

void save_match(list_t *serial) {
}

void load_match(list_t *serial) {
}

void clear_all() {
  persist_delete(NUM_POINTS_SETTING_KEY);
  persist_delete(NUM_GAMES_SETTING_KEY);
  persist_delete(SCORING_MODE_SETTING_KEY);
}
