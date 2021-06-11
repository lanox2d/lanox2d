#include "lanox2d/lanox2d.h"

static lx_matrix_t      g_matrix;
static lx_size_t        g_quality = LX_QUALITY_TOP;
static lx_size_t        g_cap = LX_PAINT_STROKE_CAP_BUTT;
static lx_size_t        g_join = LX_PAINT_STROKE_JOIN_MITER;
static lx_float_t       g_width = 1.0f;
static lx_byte_t        g_alpha = 255;
static lx_bool_t        g_transform = lx_false;

static lx_void_t on_draw(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_draw_clear(canvas, LX_COLOR_DEFAULT);
    lx_matrix_copy(lx_canvas_save_matrix(canvas), &g_matrix);
    lx_canvas_stroke_cap_set(canvas, g_cap);
    lx_canvas_stroke_join_set(canvas, g_join);
    lx_canvas_stroke_width_set(canvas, g_width);
    lx_canvas_alpha_set(canvas, g_alpha);
    // TODO
    lx_canvas_load_matrix(canvas);
}

static lx_void_t on_event(lx_window_ref_t window, lx_event_ref_t event) {
    if (event->type == LX_EVENT_TYPE_KEYBOARD && event->u.keyboard.pressed) {
        switch (event->u.keyboard.code) {
        case LX_KEY_ESCAPE:
            lx_window_quit(window);
            break;
        case LX_KEY_UP:
            g_width++;
            break;
        case LX_KEY_DOWN:
            if (g_width >= 1.0f) g_width--;
            break;
        case 'f':
            if (lx_window_is_fullscreen(window)) {
                lx_window_show_cursor(window, lx_true);
                lx_window_fullscreen(window, lx_false);
            } else {
                lx_window_show_cursor(window, lx_false);
                lx_window_fullscreen(window, lx_true);
            }
        case 'q':
            g_quality = (g_quality + 1) % 3;
            lx_quality_set(g_quality);
            break;
        case 'c':
            g_cap = (g_cap + 1) % 3;
            break;
        case 'j':
            g_join = (g_join + 1) % 3;
            break;
        case 'a':
            g_alpha -= 15;
            break;
        case 't':
            g_transform = !g_transform;
            break;
        default:
            break;
        }
    }
}

static lx_void_t on_resize(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_float_t x0 = lx_window_width(window) / 2.0f;
    lx_float_t y0 = lx_window_height(window) / 2.0f;
    lx_matrix_init_translate(&g_matrix, x0, y0);
}

int main(int argc, char** argv) {
    lx_window_ref_t window = lx_window_init(640, 480, "lanox2d");
    if (window) {
        lx_float_t x0 = lx_window_width(window) / 2.0f;
        lx_float_t y0 = lx_window_height(window) / 2.0f;
        lx_window_flags_set(window, LX_WINDOW_FLAG_SHOW_FPS);
        lx_quality_set(g_quality);
        lx_matrix_init_translate(&g_matrix, x0, y0);
        lx_window_on_draw(window, on_draw);
        lx_window_on_event(window, on_event);
        lx_window_on_resize(window, on_resize);
        lx_window_runloop(window);
        lx_window_exit(window);
    }
    return 0;
}
