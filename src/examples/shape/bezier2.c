#include "lanox2d/lanox2d.h"

static lx_long_t     g_ctrl_x = 0;
static lx_long_t     g_ctrl_y = 200;
static lx_path_ref_t g_path2 = lx_null;

static lx_void_t on_init_bezier2(lx_window_ref_t window) {
    g_path2 = lx_path_init();
}

static lx_void_t on_exit_bezier2(lx_window_ref_t window) {
    if (g_path2) {
        lx_path_exit(g_path2);
    }
}

static lx_void_t on_draw_bezier2(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_path_clear(g_path2);
    lx_path_move2i_to(g_path2, -200, 0);
    lx_path_quad2i_to(g_path2, g_ctrl_x, g_ctrl_y, 200, 0);
    lx_path_move2i_to(g_path2, -100, 0);
    lx_path_quad2i_to(g_path2, g_ctrl_x + (g_ctrl_x >> 2), g_ctrl_y + (g_ctrl_y >> 2), 100, 0);
    lx_path_move2i_to(g_path2, -50, 0);
    lx_path_quad2i_to(g_path2, g_ctrl_x + (g_ctrl_x >> 1), g_ctrl_y + (g_ctrl_y >> 1), 50, 0);
    lx_path_move2i_to(g_path2, -10, 0);
    lx_path_quad2i_to(g_path2, g_ctrl_x << 1, g_ctrl_y << 1, 10, 0);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_draw_path(canvas, g_path2);
}

static lx_void_t on_event_bezier2(lx_window_ref_t window, lx_event_ref_t event) {
    if (    event->type == LX_EVENT_TYPE_MOUSE
        &&  event->u.mouse.code == LX_MOUSE_MOVE
        &&  event->u.mouse.button == LX_MOUSE_BUTTON_LEFT) {
        lx_long_t x0 = lx_window_width(window) >> 1;
        lx_long_t y0 = lx_window_height(window) >> 1;
        g_ctrl_x = (lx_long_t)event->u.mouse.cursor.x - x0;
        g_ctrl_y = (lx_long_t)event->u.mouse.cursor.y - y0;
    }
}
