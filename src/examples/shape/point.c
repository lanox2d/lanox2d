#include "lanox2d/lanox2d.h"

static lx_void_t on_draw_point(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_point2i(canvas, 0, 0);
}

