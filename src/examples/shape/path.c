#include "lanox2d/lanox2d.h"

static lx_size_t     g_index = 0;
static lx_size_t     g_count = 0;
static lx_path_ref_t g_pathes[16] = {lx_null};
static lx_path_ref_t g_maker = lx_null;

static lx_void_t on_init_path(lx_window_ref_t window) {
    lx_path_ref_t path0 = lx_path_init();
    if (path0) {
        lx_path_move2i_to(path0, -100, -100);
        lx_path_line2i_to(path0, -50, -100);
        lx_path_line2i_to(path0, 0, -150);
        lx_path_line2i_to(path0, 50, -100);
        lx_path_line2i_to(path0, 100, -100);
        lx_path_line2i_to(path0, 100, -50);
        lx_path_line2i_to(path0, 20, 0);
        lx_path_line2i_to(path0, 100, 50);
        lx_path_line2i_to(path0, 100, 100);
        lx_path_line2i_to(path0, 50, 100);
        lx_path_line2i_to(path0, 0, 150);
        lx_path_line2i_to(path0, -50, 100);
        lx_path_line2i_to(path0, -100, 100);
        lx_path_line2i_to(path0, -100, 50);
        lx_path_line2i_to(path0, -200, 0);
        lx_path_line2i_to(path0, -100, -50);
        lx_path_close(path0);
        g_pathes[g_count++] = path0;
    }

    lx_path_ref_t path1 = lx_path_init();
    if (path1) {
        lx_path_move2i_to(path1, 0, 0);
        lx_path_line2i_to(path1, -50, -100);
        lx_path_line2i_to(path1, 0, -200);
        lx_path_line2i_to(path1, 50, -100);
        lx_path_close(path1);

        lx_path_move2i_to(path1, 0, 0);
        lx_path_line2i_to(path1, 100, -50);
        lx_path_line2i_to(path1, 200, 0);
        lx_path_line2i_to(path1, 100, 50);
        lx_path_close(path1);

        lx_path_move2i_to(path1, 0, 0);
        lx_path_line2i_to(path1, 50, 100);
        lx_path_line2i_to(path1, 0, 200);
        lx_path_line2i_to(path1, -50, 100);
        lx_path_close(path1);

        lx_path_move2i_to(path1, 0, 0);
        lx_path_line2i_to(path1, -100, 50);
        lx_path_line2i_to(path1, -200, 0);
        lx_path_line2i_to(path1, -100, -50);
        lx_path_close(path1);
        g_pathes[g_count++] = path1;
    }

    lx_path_ref_t path2 = lx_path_init();
    if (path2) {
        lx_path_add_rect2i(path2, -200, -200, 400, 400, LX_ROTATE_DIRECTION_CW);
        lx_path_add_rect2i(path2, -100, -100, 200, 200, LX_ROTATE_DIRECTION_CW);
        lx_path_add_rect2i(path2, -50, -50, 100, 100, LX_ROTATE_DIRECTION_CW);
        g_pathes[g_count++] = path2;
    }

    lx_path_ref_t path3 = lx_path_init();
    if (path3) {
        lx_path_move2i_to(path3, -100, -100);
        lx_path_quad2i_to(path3, -50, -100, 0, -150);
        lx_path_quad2i_to(path3, 50, -100, 100, -100);
        lx_path_quad2i_to(path3, 100, -50, 20, 0);
        lx_path_quad2i_to(path3, 100, 50, 100, 100);
        lx_path_quad2i_to(path3, 50, 100, 0, 150);
        lx_path_quad2i_to(path3, -50, 100, -100, 100);
        lx_path_quad2i_to(path3, -100, 50, -200, 0);
        lx_path_quad2i_to(path3, -100, -50, -100, -100);
        lx_path_close(path3);
        g_pathes[g_count++] = path3;
    }

    lx_path_ref_t path4 = lx_path_init();
    if (path4) {
        lx_path_move2i_to(path4, 0, 0);
        lx_path_quad2i_to(path4, -50, -100, 0, -200);
        lx_path_quad2i_to(path4, 50, -100, 0, 0);
        lx_path_close(path4);

        lx_path_move2i_to(path4, 0, 0);
        lx_path_quad2i_to(path4, 100, -50, 200, 0);
        lx_path_quad2i_to(path4, 100, 50, 0, 0);
        lx_path_close(path4);

        lx_path_move2i_to(path4, 0, 0);
        lx_path_quad2i_to(path4, 50, 100, 0, 200);
        lx_path_quad2i_to(path4, -50, 100, 0, 0);
        lx_path_close(path4);

        lx_path_move2i_to(path4, 0, 0);
        lx_path_quad2i_to(path4, -100, 50, -200, 0);
        lx_path_quad2i_to(path4, -100, -50, 0, 0);
        lx_path_close(path4);
        g_pathes[g_count++] = path4;
    }
}

static lx_void_t on_exit_path(lx_window_ref_t window) {
    lx_size_t i = 0;
    for (i = 0; i < g_count; i++) {
        if (g_pathes[i]) {
            lx_path_exit(g_pathes[i]);
            g_pathes[i] = lx_null;
        }
    }
    if (g_maker) lx_path_exit(g_maker);
    g_maker = lx_null;
}

static lx_void_t on_draw_path(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_color_set(canvas, LX_COLOR_RED);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_FILL);
    lx_canvas_shader_set(canvas, g_shaders[g_shader]);
    lx_canvas_draw_path(canvas, g_pathes[g_index]);

    lx_canvas_color_set(canvas, LX_COLOR_BLUE);
    lx_canvas_mode_set(canvas, LX_PAINT_MODE_STROKE);
    lx_canvas_shader_set(canvas, lx_null);
    lx_canvas_draw_path(canvas, g_pathes[g_index]);
}

static lx_void_t on_event_path(lx_window_ref_t window, lx_event_ref_t event) {
    if (event->type == LX_EVENT_TYPE_KEYBOARD && event->u.keyboard.pressed) {
        if (event->u.keyboard.code == 'p') {
            g_index = (g_index + 1) % g_count;
        } else if (event->u.keyboard.code == 'q') {
            if (g_maker) {
                lx_path_close(g_maker);
                g_pathes[g_count++] = g_maker;
                g_maker = lx_null;
                g_index = g_count - 1;
            }
        }
    } else if (event->type == LX_EVENT_TYPE_MOUSE) {
        if (    event->u.mouse.code == LX_MOUSE_DOWN
            &&  event->u.mouse.button == LX_MOUSE_BUTTON_LEFT) {

            lx_long_t x0 = lx_window_width(window) >> 1;
            lx_long_t y0 = lx_window_height(window) >> 1;
            lx_long_t cx = (lx_long_t)(event->u.mouse.cursor.x);
            lx_long_t cy = (lx_long_t)(event->u.mouse.cursor.y);

            if (!g_maker) {
                g_maker = lx_path_init();
                lx_assert(g_maker);
                lx_path_move2i_to(g_maker, cx - x0, cy - y0);
            } else {
                lx_path_line2i_to(g_maker, cx - x0, cy - y0);
            }
        }
    }
}
