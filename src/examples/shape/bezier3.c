#include "lanox2d/lanox2d.h"

static lx_long_t     g_ctrl_x0 = -100 >> 4;
static lx_long_t     g_ctrl_y0 = 200 >> 1;
static lx_long_t     g_ctrl_x1 = 100 >> 4;
static lx_long_t     g_ctrl_y1 = 200 >> 1;
static lx_path_ref_t g_path3 = lx_null;

static lx_void_t on_init_bezier3(lx_window_ref_t window) {
    g_path3 = lx_path_init();
}

static lx_void_t on_exit_bezier3(lx_window_ref_t window) {
    if (g_path3) {
        lx_path_exit(g_path3);
    }
}

static lx_void_t on_draw_bezier3(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_path_clear(g_path3);
    lx_path_move2i_to(g_path3, -200, 0);
    lx_path_cubic2i_to(g_path3, g_ctrl_x0, g_ctrl_y0, g_ctrl_x1, g_ctrl_y1, 200, 0);
    lx_path_move2i_to(g_path3, -100, 0);
    lx_path_cubic2i_to(g_path3, g_ctrl_x0 + (g_ctrl_x0 >> 2), g_ctrl_y0 + (g_ctrl_y0 >> 2), g_ctrl_x1 + (g_ctrl_x1 >> 2), g_ctrl_y1 + (g_ctrl_y1 >> 2), 100, 0);
    lx_path_move2i_to(g_path3, -50, 0);
    lx_path_cubic2i_to(g_path3, g_ctrl_x0 + (g_ctrl_x0 >> 1), g_ctrl_y0 + (g_ctrl_y0 >> 1), g_ctrl_x1 + (g_ctrl_x1 >> 1), g_ctrl_y1 + (g_ctrl_y1 >> 1), 50, 0);
    lx_path_move2i_to(g_path3, -10, 0);
    lx_path_cubic2i_to(g_path3, g_ctrl_x0 << 1, g_ctrl_y0 << 1, g_ctrl_x1 << 1, g_ctrl_y1 << 1, 10, 0);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_path(canvas, g_path3);
}

static lx_void_t on_event_bezier3(lx_window_ref_t window, lx_event_ref_t event) {
    if (    event->type == LX_EVENT_TYPE_MOUSE
        &&  event->u.mouse.code == LX_MOUSE_MOVE
        &&  event->u.mouse.button == LX_MOUSE_BUTTON_LEFT) {

        lx_long_t x0 = lx_window_width(window) >> 1;
        lx_long_t y0 = lx_window_height(window) >> 1;
        lx_long_t cx = (lx_long_t)event->u.mouse.cursor.x;
        lx_long_t cy = (lx_long_t)event->u.mouse.cursor.y;

        if (cx < x0) {
            g_ctrl_x0 = (cx - x0) >> 1;
            g_ctrl_y0 = (cy - y0) >> 1;
        }

        if (cx > x0) {
            g_ctrl_x1 = (cx - x0) >> 1;
            g_ctrl_y1 = (cy - y0) >> 1;
        }
    }
}
