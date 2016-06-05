#ifndef PERSISTENCE_H
#define PERSISTENCE_H

#include <pebble.h>
#include "state.h"

typedef enum {
  NUM_POINTS_SETTING_KEY,
  NUM_GAMES_SETTING_KEY,
  SCORING_MODE_SETTING_KEY
} PERSIST_KEYS;

void load_settings(Settings *settings);
void save_settings(Settings *settings);
void load_match(list_t *serial);
void save_match(list_t *serial);
void clear_all();

#endif
