#include "lanox2d/lanox2d.h"

static lx_void_t on_draw_rect(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_rect_t rect;
    lx_rect_imake(&rect, -200, -200, 400, 400);

    lx_canvas_color_set(canvas, LX_COLOR_RED);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_FILL);
    lx_canvas_shader_set(canvas, g_shaders[g_shader]);
    lx_canvas_draw_rect(canvas, &rect);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_shader_set(canvas, lx_null);
    lx_canvas_draw_rect(canvas, &rect);
}
