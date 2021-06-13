#include "lanox2d/lanox2d.h"

static lx_void_t on_draw_ellipse(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_ellipse_t ellipse;
    lx_ellipse_imake(&ellipse, 0, 0, 200, 100);

    lx_canvas_color_set(canvas, LX_COLOR_RED);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_FILL);
    lx_canvas_draw_ellipse(canvas, &ellipse);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_ellipse(canvas, &ellipse);
}


