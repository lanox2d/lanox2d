#include "lanox2d/lanox2d.h"

static lx_void_t on_draw_round_rect(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_rect_t bounds;
    lx_rect_imake(&bounds, -100, -100, 200, 200);

    lx_canvas_color_set(canvas, LX_COLOR_RED);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_FILL);
    lx_canvas_draw_round_rect2i(canvas, &bounds, 50, 50);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_round_rect2i(canvas, &bounds, 50, 50);
}

