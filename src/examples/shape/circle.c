#include "lanox2d/lanox2d.h"

static lx_void_t on_draw_circle(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_circle_t circle;
    lx_circle_imake(&circle, 0, 0, 200);

    lx_canvas_color_set(canvas, LX_COLOR_RED);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_FILL);
    lx_canvas_shader_set(canvas, g_shaders[g_shader]);
    lx_canvas_draw_circle(canvas, &circle);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_shader_set(canvas, lx_null);
    lx_canvas_draw_circle(canvas, &circle);
}

