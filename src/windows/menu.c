#include "menu.h"

static Window *s_main_window;

static SimpleMenuLayer *main_menu_layer;
static SimpleMenuSection main_menu_sections[2];
static SimpleMenuItem main_menu_items[1];
static SimpleMenuItem main_menu_item_options[3];

static Settings settings;

void toggle_switch_setting(int *setting) {
  *setting ^= 1;
}

const char *num_games_to_string(int n) {
  switch (n) {
    case 3: return "3";
    case 5: return "5";
    default: return "?";
  }
}

const char *num_points_to_string(int n) {
  switch (n) {
    case 9: return "9";
    case 11: return "11";
    case 13: return "13";
    case 15: return "15";
    default: return "?";
  }
}

const char *scoring_mode_to_string(int n) {
  if (n == EVERY_POINT) return "Every point";
  if (n == ONLY_ON_SERVE) return "Only on serve";
  return "?";
}

void cycle_num_points() {
  if (settings.num_points < 15) {
    settings.num_points += 2;
  } else {
    settings.num_points = 9;
  }
  main_menu_item_options[0].subtitle = num_points_to_string(settings.num_points);
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void cycle_num_games() {
  settings.num_games = settings.num_games == 3 ? 5 : 3;
  main_menu_item_options[1].subtitle = num_games_to_string(settings.num_games);
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void cycle_scoring_mode_setting() {
  settings.scoring_mode = settings.scoring_mode == EVERY_POINT ? ONLY_ON_SERVE : EVERY_POINT;
  main_menu_item_options[2].subtitle = scoring_mode_to_string(settings.scoring_mode);
  layer_mark_dirty(simple_menu_layer_get_layer(main_menu_layer));
}

void start_match() {
  window_stack_pop(false);
  match_window_push(&settings, serial_new());
  save_settings(&settings);
}

static void window_load(Window *window) {

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  settings = (Settings)
    { .num_points = 11
    , .num_games = 3
    , .scoring_mode = EVERY_POINT
    , .first_server = PLAYER
    };

  load_settings(&settings);

  main_menu_sections[0] = (SimpleMenuSection) {
    .num_items = 1,
    .items = main_menu_items
  };

  main_menu_items[0] = (SimpleMenuItem) {
    .title = "Start Match",
    .callback = start_match
  };

  main_menu_sections[1] = (SimpleMenuSection) {
    .title = "Match Settings",
    .items = main_menu_item_options,
    .num_items = 3
  };

  main_menu_item_options[0] = (SimpleMenuItem) {
    .title = "Points",
    .subtitle = num_points_to_string(settings.num_points),
    .callback = cycle_num_points
  };

  main_menu_item_options[1] = (SimpleMenuItem) {
    .title = "Games",
    .subtitle = num_games_to_string(settings.num_games),
    .callback = cycle_num_games
  };

  main_menu_item_options[2] = (SimpleMenuItem) {
    .title = "Scoring mode",
    .subtitle = scoring_mode_to_string(settings.scoring_mode),
    .callback = cycle_scoring_mode_setting
  };

  main_menu_layer = simple_menu_layer_create(bounds, window, main_menu_sections, 2, NULL);
  layer_add_child(window_layer, simple_menu_layer_get_layer(main_menu_layer));

}

static void window_unload(Window *window) {
  simple_menu_layer_destroy(main_menu_layer);
  window_destroy(window);
  s_main_window = NULL;
}

void menu_window_push() {
  if (!s_main_window) {
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }
  window_stack_push(s_main_window, true);
}
