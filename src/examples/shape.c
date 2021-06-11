#include "lanox2d/lanox2d.h"

typedef struct lx_entry_t_ {
    lx_char_t const*    name;
    lx_void_t           (*on_draw)(lx_window_ref_t window, lx_canvas_ref_t canvas);
    lx_void_t           (*on_event)(lx_window_ref_t window, lx_event_ref_t event);
}lx_entry_t;

static lx_matrix_t      g_matrix;
static lx_size_t        g_quality = LX_QUALITY_TOP;
static lx_size_t        g_cap = LX_PAINT_STROKE_CAP_BUTT;
static lx_size_t        g_join = LX_PAINT_STROKE_JOIN_MITER;
static lx_float_t       g_width = 1.0f;
static lx_byte_t        g_alpha = 255;
static lx_bool_t        g_transform = lx_false;
static lx_entry_t*      g_entry = lx_null;
static lx_long_t        g_line_dx = 100;
static lx_long_t        g_line_dy = 100;

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

static lx_entry_t g_entries[] = {
    {"line", on_draw_line, on_event_line}
};

static lx_void_t on_draw(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_draw_clear(canvas, LX_COLOR_DEFAULT);
    lx_matrix_copy(lx_canvas_save_matrix(canvas), &g_matrix);
    lx_canvas_stroke_cap_set(canvas, g_cap);
    lx_canvas_stroke_join_set(canvas, g_join);
    lx_canvas_stroke_width_set(canvas, g_width);
    lx_canvas_alpha_set(canvas, g_alpha);
    g_entry->on_draw(window, canvas);
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
    } else if (event->type == LX_EVENT_TYPE_MOUSE) {
        if (event->u.mouse.code == LX_MOUSE_MOVE) {
            lx_float_t x = event->u.mouse.cursor.x;
            lx_float_t y = event->u.mouse.cursor.y;
            if (g_transform) {
                lx_float_t an = 0;
                lx_float_t dw = (lx_float_t)lx_window_width(window);
                lx_float_t dh = (lx_float_t)lx_window_height(window);
                lx_float_t x0 = dw / 2;
                lx_float_t y0 = dh / 2;
                lx_float_t dx = x > x0? (x - x0) : (x0 - x);
                lx_float_t dy = y > y0? (y - y0) : (y0 - y);
                dx = dx * 2;
                dy = dy * 2;
                if (y == y0) an = 0;
                else if (x == x0) an = 90.0f;
                else an = lx_atan(dy / dx) * 180 / LX_PI;
                if (y < y0 && x < x0) an = 180.0f - an;
                if (y > y0 && x < x0) an += 180.0f;
                if (y > y0 && x > x0) an = 360.0f - an;
                an = -an;
                dx = dx * 4;
                dy = dy * 4;
                lx_matrix_init_translate(&g_matrix, x0, y0);
                lx_matrix_scale(&g_matrix, dx / dw, dy / dh);
                lx_matrix_rotate(&g_matrix, an);
            }
        }
    }
    g_entry->on_event(window, event);
}

static lx_void_t on_resize(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_float_t x0 = lx_window_width(window) / 2.0f;
    lx_float_t y0 = lx_window_height(window) / 2.0f;
    lx_matrix_init_translate(&g_matrix, x0, y0);
}

static lx_entry_t* get_entry(lx_char_t const* name) {
    lx_size_t i = 0;
    lx_size_t n = lx_arrayn(g_entries);
    for (i = 0; i < n; i++) {
        if (!lx_strcmp(g_entries[i].name, name)) {
            return g_entries + i;
        }
    }
    return lx_null;
}

int main(int argc, char** argv) {
    g_entry = get_entry(argv[1]? argv[1] : "rect");
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
