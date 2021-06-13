#include "lanox2d/lanox2d.h"

static lx_void_t on_draw_arc(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_arc2i(canvas, 0, 0, 300, 150, -45, 180);
    lx_canvas_draw_arc2i(canvas, 0, 0, 200, 100, 45, 200);
    lx_canvas_draw_arc2i(canvas, 0, 0, 100, 50, 45, -22);
    lx_canvas_draw_arc2i(canvas, 0, 0, 10, 10, 300, -181);
    lx_canvas_draw_arc2i(canvas, 0, 0, 50, 20, -100, -222);
    lx_canvas_draw_arc2i(canvas, -50, -50, 50, 50, 180, 90);
    lx_canvas_draw_arc2i(canvas, 50, -50, 50, 50, 90, 90);
}

