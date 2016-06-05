#include "overview.h"

static void render(Layer *layer, GContext *ctx) {

  OverviewData *data = (OverviewData *) layer_get_data(layer);
  State *state = data->state;

  GRect bounds = layer_get_frame(layer);

  graphics_context_set_text_color(ctx, GColorWhite);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  GRect text_box = { .origin = { .x = 10, .y = 10 }, .size = { .h = 34, .w = (bounds.size.w - 30) / 2 } };
  graphics_fill_rect(ctx, text_box, 5, GCornerNone);

  graphics_draw_text(
      ctx
    , state->player_games > state->opponent_games ? "WIN" : "LOSE"
    , fonts_get_system_font(FONT_KEY_GOTHIC_24)
    , text_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

  GRect score_box =
    { .origin = { .x = 10 + (bounds.size.w - 30) / 2, .y = 10 }
    , .size = { .h = 34, .w = (bounds.size.w - 30) / 2 }
    };
  graphics_draw_rect(ctx, score_box);

  char str[10];
  snprintf(str, sizeof(str), "%d-%d", state->player_games, state->opponent_games);
  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_draw_text(
      ctx
    , str
    , fonts_get_system_font(FONT_KEY_GOTHIC_24)
    , score_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

  int width = bounds.size.w - 30;
  int num_games = state->player_games + state->opponent_games;
  int cell_size = (width / 5);
  int line_width = cell_size * num_games;

  graphics_draw_line(ctx
    , GPoint(10 + (width / 2) - (line_width / 2), 68 + cell_size + 3)
    , GPoint(10 + (width / 2) + (line_width / 2), 68 + cell_size + 3));

  for (int i = 0; i < num_games; i++) {
    bool player_won_game = state->completed_games[i][0] > state->completed_games[i][1];
    int left_offset = 10 + (width / 2) - (line_width / 2) + (i * cell_size);
    GRect game_box_a = { .origin = GPoint(left_offset, 68), GSize(cell_size, cell_size) };
    GRect game_box_b = { .origin = GPoint(left_offset, 68 + cell_size + 5), GSize(cell_size, cell_size) };
    char player_game_score[4];
    char opponent_game_score[4];
    snprintf(player_game_score, sizeof(player_game_score), "%d", state->completed_games[i][0]);
    snprintf(opponent_game_score, sizeof(opponent_game_score), "%d", state->completed_games[i][1]);
    graphics_draw_text(
        ctx
      , player_game_score
      , fonts_get_system_font(player_won_game ? FONT_KEY_GOTHIC_18_BOLD : FONT_KEY_GOTHIC_18)
      , game_box_a
      , GTextOverflowModeTrailingEllipsis
      , GTextAlignmentCenter
      , NULL);
    graphics_draw_text(
        ctx
      , opponent_game_score
      , fonts_get_system_font(player_won_game ? FONT_KEY_GOTHIC_18 : FONT_KEY_GOTHIC_18_BOLD)
      , game_box_b
      , GTextOverflowModeTrailingEllipsis
      , GTextAlignmentCenter
      , NULL);
  }

  GRect match_time_text_box =
    { .origin = GPoint(10, bounds.size.h - 29)
    , .size = GSize((bounds.size.w - 30) / 2, 30)
    };
  GRect match_time_box =
    { .origin = GPoint(10 + ((bounds.size.w - 30) / 2), bounds.size.h - 33)
    , .size = GSize((bounds.size.w - 30) / 2, 30)
    };

  char time_str[25];
  snprintf(time_str, sizeof(time_str), "%dh %d"
    , (state->match_end_time - state->match_start_time) / SECONDS_PER_HOUR
    , ((state->match_end_time - state->match_start_time) % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE);

  graphics_draw_text(
      ctx
    , "Duration:"
    , fonts_get_system_font(FONT_KEY_GOTHIC_14)
    , match_time_text_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentRight
    , NULL);

  graphics_draw_text(
      ctx
    , time_str
    , fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD)
    , match_time_box
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

}

Layer *overview_create(GRect bounds, State *state) {

  Layer *layer = layer_create_with_data(bounds, sizeof(OverviewData));
  OverviewData *data = (OverviewData *) layer_get_data(layer);
  data->state = state;
  layer_set_update_proc(layer, render);
  return layer;

}
