#include "points.h"

static void render_points(State *state, GRect bounds, GContext *ctx) {

  graphics_context_set_text_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_context_set_stroke_color(ctx, GColorBlack);

  graphics_draw_text(
      ctx
    , "TOTAL POINTS"
    , fonts_get_system_font(FONT_KEY_GOTHIC_14)
    , GRect(10 + ((bounds.size.w - 30) / 6), 40, (bounds.size.w - 30) * 4 / 6, 50)
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

  char player_points_str[4];
  snprintf(player_points_str, sizeof(player_points_str), "%d", state->player_total_points);
  graphics_draw_text(
      ctx
    , player_points_str
    , fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK)
    , GRect(10, 65, (bounds.size.w - 30) / 2, 50)
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);
  graphics_draw_text(
      ctx
    , "(YOU)"
    , fonts_get_system_font(FONT_KEY_GOTHIC_09)
    , GRect(10, 100, (bounds.size.w - 30) / 2, 20)
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

  char opponent_points_str[4];
  snprintf(opponent_points_str, sizeof(opponent_points_str), "%d", state->opponent_total_points);
  graphics_draw_text(
      ctx
    , opponent_points_str
    , fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK)
    , GRect(10 + (bounds.size.w - 30) / 2, 65, (bounds.size.w - 30) / 2, 50)
    , GTextOverflowModeTrailingEllipsis
    , GTextAlignmentCenter
    , NULL);

}

static void render(Layer *layer, GContext *ctx) {

  PointsData *data = (PointsData *) layer_get_data(layer);
  State *state = data->state;

  GRect bounds = layer_get_frame(layer);
  render_points(state, bounds, ctx);
  // render_break_points(state, bounds, ctx);

}

Layer *points_create(GRect bounds, State *state) {

  Layer *layer = layer_create_with_data(bounds, sizeof(PointsData));
  PointsData *data = (PointsData *) layer_get_data(layer);
  data->state = state;
  layer_set_update_proc(layer, render);
  return layer;

}
