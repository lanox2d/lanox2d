#include "lanox2d/lanox2d.h"

static lx_long_t g_line_dx = 100;
static lx_long_t g_line_dy = 100;

static lx_void_t on_draw_line(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_line2i(canvas, -g_line_dx, -g_line_dy, g_line_dx, g_line_dy);
}

static lx_void_t on_event_line(lx_window_ref_t window, lx_event_ref_t event) {
    if (event->type == LX_EVENT_TYPE_MOUSE && event->u.mouse.code == LX_MOUSE_MOVE &&
            event->u.mouse.button == LX_MOUSE_BUTTON_LEFT) {
        lx_long_t x0 = lx_window_width(window) >> 1;
        lx_long_t y0 = lx_window_height(window) >> 1;
        lx_long_t cx = (lx_long_t)event->u.mouse.cursor.x;
        lx_long_t cy = (lx_long_t)event->u.mouse.cursor.y;
        g_line_dx = cx - x0;
        g_line_dy = y0 - cy;
    }
}

