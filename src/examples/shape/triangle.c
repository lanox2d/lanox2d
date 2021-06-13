#include "lanox2d/lanox2d.h"

static lx_void_t on_draw_triangle(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_triangle_t triangle;
    lx_triangle_imake(&triangle, -100, 100, 0, -100, 100, 100);

    lx_canvas_color_set(canvas, LX_COLOR_RED);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_FILL);
    lx_canvas_draw_triangle(canvas, &triangle);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_triangle(canvas, &triangle);
}
