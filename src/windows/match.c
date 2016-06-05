#include "match.h"

static list_t *serial;
static Window *s_main_window;
static Settings settings;

static TextLayer *sets_label;
static TextLayer *games_label;

static TextLayer *player_points;
static char player_points_str[4];
static TextLayer *opponent_points;
static char opponent_points_str[4];

static TextLayer *player_games;
static char player_games_str[2];
static TextLayer *opponent_games;
static char opponent_games_str[2];

static Layer *layout_layer;
static Layer *server_marker_layer;

static StatusBarLayer *status_layer;

static int server;

static void display_score_update(TextLayer *t, char * str, int s) {
  snprintf(str, 3, "%d", s);
  text_layer_set_text(t, str);
}

static void display_digit_update(TextLayer *t, int s, char *str) {
  snprintf(str, sizeof(str), "%d", s);
  text_layer_set_text(t, str);
}

static void render(State *state) {
  display_score_update(player_points, player_points_str, state->player_points);
  display_digit_update(player_games, state->player_games, player_games_str);
  display_score_update(opponent_points, opponent_points_str, state->opponent_points);
  display_digit_update(opponent_games, state->opponent_games, opponent_games_str);
  server = state->server;
  draw_server_marker();
  state_destroy(state);
}

static void show_summary() {
  window_stack_pop_all(true);
  State state = compute_state(serial, &settings);
  summary_window_push(state);
}

static void opponent_point_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_opponent_points(serial);
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
  if (state.is_complete) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Match complete!");
    show_summary();
  }
}

static void player_point_click_handler(ClickRecognizerRef recognizer, void *context) {
  add_player_points(serial);
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
  if (state.is_complete) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Match complete!");
    show_summary();
  }
}

static void undo_click_handler(ClickRecognizerRef recognizer, void *context) {
  if (serial->len > 0) {
    // If there are scores, middle button rewinds
    undo(serial);
  } else {
    // If there is no score yet, middle button toggles first server
    if (settings.first_server == PLAYER) {
      settings.first_server = OPPONENT;
    } else {
      settings.first_server = PLAYER;
    }
  }
  State state = compute_state(serial, &settings);
  debug_state(&state);
  render(&state);
}

static void menu_click_handler(ClickRecognizerRef recognize, void *context) {
  in_play_menu_window_push(serial);
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_UP, opponent_point_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, player_point_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, undo_click_handler);
  window_single_click_subscribe(BUTTON_ID_BACK, menu_click_handler);
}

static void layout_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
  int available_height = bounds.size.h - STATUS_BAR_LAYER_HEIGHT;
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_draw_line(ctx
    , GPoint(2, STATUS_BAR_LAYER_HEIGHT + (available_height / 2))
    , GPoint(bounds.size.w - 2,  STATUS_BAR_LAYER_HEIGHT + (available_height / 2)));
}

static void draw_layout() {
  GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));
  layout_layer = layer_create(bounds);
  layer_set_update_proc(layout_layer, layout_update_proc);
  layer_add_child(window_get_root_layer(s_main_window), layout_layer);
}

static void server_marker_update_proc(Layer *layer, GContext *ctx) {
  GRect bounds = layer_get_frame(layer);
  int available_height = bounds.size.h - STATUS_BAR_LAYER_HEIGHT;
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);
  if (server == PLAYER) {
    graphics_fill_circle(ctx, GPoint(bounds.size.w - 8, STATUS_BAR_LAYER_HEIGHT + (available_height * 0.75)), 3);
  } else {
    graphics_fill_circle(ctx, GPoint(bounds.size.w - 8, STATUS_BAR_LAYER_HEIGHT + (available_height / 4)), 3);
  }
}

void draw_server_marker() {
  if (server_marker_layer) {
    layer_mark_dirty(server_marker_layer);
  } else {
    GRect bounds = layer_get_bounds(window_get_root_layer(s_main_window));
    server_marker_layer = layer_create(bounds);
    layer_set_update_proc(server_marker_layer, server_marker_update_proc);
    layer_add_child(window_get_root_layer(s_main_window), server_marker_layer);
  }
}

static void window_load(Window *window) {

  // if (!serial) serial = serial_new();
  State state = compute_state(serial, &settings);
  debug_state(&state);

  // Make the text
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  int available_height = bounds.size.h - STATUS_BAR_LAYER_HEIGHT;

  // Player games
  player_games = text_layer_create(GRect(25, STATUS_BAR_LAYER_HEIGHT - 19 + (available_height * 0.75), 30, 28));
  text_layer_set_text(player_games, "0");
  text_layer_set_font(player_games, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(player_games, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_games);

  // Opponent games
  opponent_games = text_layer_create(GRect(25, STATUS_BAR_LAYER_HEIGHT - 19 + (available_height / 4), 30, 28));
  text_layer_set_text(opponent_games, "0");
  text_layer_set_font(opponent_games, fonts_get_system_font(FONT_KEY_GOTHIC_28_BOLD));
  text_layer_set_text_alignment(opponent_games, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_games);

  // Player score
  player_points = text_layer_create(GRect(bounds.size.w * 3 / 7, STATUS_BAR_LAYER_HEIGHT + 11 + (available_height / 2), (bounds.size.w / 2), available_height / 2));
  text_layer_set_text(player_points, "0");
  text_layer_set_font(player_points, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(player_points, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) player_points);

  // Opponent score
  opponent_points = text_layer_create(GRect(bounds.size.w * 3 / 7, STATUS_BAR_LAYER_HEIGHT + 11, (bounds.size.w / 2), available_height / 2));
  text_layer_set_text(opponent_points, "0");
  text_layer_set_font(opponent_points, fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
  text_layer_set_text_alignment(opponent_points, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) opponent_points);

  draw_layout();

  // Games label
  games_label = text_layer_create(GRect(21, STATUS_BAR_LAYER_HEIGHT - 10 + (available_height / 2), 42, 20));
  text_layer_set_background_color(games_label, GColorWhite);
  text_layer_set_text(games_label, "GAMES");
  text_layer_set_font(games_label, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text_alignment(games_label, GTextAlignmentCenter);
  layer_add_child(window_layer, (Layer *) games_label);

  status_layer = status_bar_layer_create();
  layer_add_child(window_layer, (Layer *) status_layer);

  render(&state);

}

static void window_unload(Window *window) {

  serial_destroy(serial);

  text_layer_destroy(sets_label);
  text_layer_destroy(games_label);
  text_layer_destroy(player_points);
  text_layer_destroy(opponent_points);
  text_layer_destroy(player_games);
  text_layer_destroy(opponent_games);

  layer_destroy(layout_layer);
  layout_layer = NULL;

  layer_destroy(server_marker_layer);
  server_marker_layer = NULL;

  status_bar_layer_destroy(status_layer);
  status_layer = NULL;

  window_destroy(window);
  s_main_window = NULL;

}

void match_window_push(Settings *s, list_t *srl) {
  if (!s_main_window) {
    settings = *s;
    serial = srl;
    s_main_window = window_create();
    window_set_window_handlers(s_main_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
    window_set_click_config_provider(s_main_window, click_config_provider);
  }
  window_stack_push(s_main_window, true);
}
