#include "lanox2d/lanox2d.h"

typedef struct lx_entry_t_ {
    lx_char_t const*    name;
    lx_void_t           (*on_init)(lx_window_ref_t window);
    lx_void_t           (*on_exit)(lx_window_ref_t window);
    lx_void_t           (*on_draw)(lx_window_ref_t window, lx_canvas_ref_t canvas);
    lx_void_t           (*on_event)(lx_window_ref_t window, lx_event_ref_t event);
}lx_entry_t;

static lx_matrix_t      g_matrix;
static lx_size_t        g_quality = LX_QUALITY_TOP;
static lx_size_t        g_cap = LX_PAINT_STROKE_CAP_BUTT;
static lx_size_t        g_join = LX_PAINT_STROKE_JOIN_MITER;
static lx_float_t       g_width = 1.0f;
static lx_byte_t        g_alpha = 255;
static lx_entry_t*      g_entry = lx_null;
static lx_size_t        g_shader = 0;
static lx_shader_ref_t  g_shaders[3] = {0};
static lx_bitmap_ref_t  g_bitmap = lx_null;

#include "shape/arc.c"
#include "shape/line.c"
#include "shape/rect.c"
#include "shape/path.c"
#include "shape/point.c"
#include "shape/points.c"
#include "shape/circle.c"
#include "shape/ellipse.c"
#include "shape/triangle.c"
#include "shape/bezier2.c"
#include "shape/bezier3.c"
#include "shape/round_rect.c"
#include "shape/tiger.c"

static lx_entry_t g_entries[] = {
    {"line",       lx_null,         lx_null,         on_draw_line,       on_event_line},
    {"arc",        lx_null,         lx_null,         on_draw_arc,        lx_null},
    {"rect",       lx_null,         lx_null,         on_draw_rect,       lx_null},
    {"path",       on_init_path,    on_exit_path,    on_draw_path,       on_event_path},
    {"point",      lx_null,         lx_null,         on_draw_point,      lx_null},
    {"points",     lx_null,         lx_null,         on_draw_points,     lx_null},
    {"circle",     lx_null,         lx_null,         on_draw_circle,     lx_null},
    {"ellipse",    lx_null,         lx_null,         on_draw_ellipse,    lx_null},
    {"bezier2",    on_init_bezier2, on_exit_bezier2, on_draw_bezier2,    on_event_bezier2},
    {"bezier3",    on_init_bezier3, on_exit_bezier3, on_draw_bezier3,    on_event_bezier3},
    {"triangle",   lx_null,         lx_null,         on_draw_triangle,   lx_null},
    {"round_rect", lx_null,         lx_null,         on_draw_round_rect, lx_null},
    {"tiger",      on_init_tiger,   on_exit_tiger,   on_draw_tiger,      lx_null}
};

static lx_void_t on_draw(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_draw_clear(canvas, LX_COLOR_DEFAULT);
    lx_matrix_copy(lx_canvas_save_matrix(canvas), &g_matrix);
    lx_canvas_stroke_cap_set(canvas, g_cap);
    lx_canvas_stroke_join_set(canvas, g_join);
    lx_canvas_stroke_width_set(canvas, g_width);
    lx_canvas_alpha_set(canvas, g_alpha);
    if (g_bitmap && g_shaders[0]) {
        lx_matrix_t mx;
        lx_size_t bw = lx_bitmap_width(g_bitmap);
        lx_size_t bh = lx_bitmap_height(g_bitmap);
        lx_matrix_init_translate(&mx, -100.0f, -100.0f);
        lx_matrix_scale(&mx, 200.0f / bw, 200.0f / bh);
        lx_shader_matrix_set(g_shaders[0], &mx);
    }
    g_entry->on_draw(window, canvas);
    lx_canvas_load_matrix(canvas);
}

static lx_void_t on_event(lx_window_ref_t window, lx_event_ref_t event) {
    if (event->type == LX_EVENT_TYPE_KEYBOARD && event->u.keyboard.pressed) {
        switch (event->u.keyboard.code) {
        case LX_KEY_ESCAPE:
        case 'q':
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
            break;
        case 'g':
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
        case 's':
            g_shader = (g_shader + 1) % 3;
        default:
            break;
        }
    } else if (event->type == LX_EVENT_TYPE_MOUSE || event->type == LX_EVENT_TYPE_TOUCH) {
        lx_float_t x, y;
        lx_bool_t transform = lx_false;
        if (event->type == LX_EVENT_TYPE_MOUSE && event->u.mouse.code == LX_MOUSE_MOVE && event->u.mouse.button == LX_MOUSE_BUTTON_LEFT) {
            x = event->u.mouse.cursor.x;
            y = event->u.mouse.cursor.y;
            transform = lx_true;
        } else if (event->type == LX_EVENT_TYPE_TOUCH && event->u.touch.code == LX_TOUCH_MOVED) {
            x = event->u.touch.touches[0].point.x;
            y = event->u.touch.touches[0].point.y;
            transform = lx_true;
        }
        if (transform) {
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
            else an = lx_atanf(dy / dx) * 180 / LX_PI;
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
    if (g_entry->on_event) {
        g_entry->on_event(window, event);
    }
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

static lx_void_t window_init(lx_window_ref_t window, lx_int_t argc, lx_char_t** argv) {

    g_entry = get_entry(argc > 1? argv[1] : "rect");

    lx_window_on_draw(window, on_draw);
    lx_window_on_event(window, on_event);

    lx_float_t x0 = lx_window_width(window) / 2.0f;
    lx_float_t y0 = lx_window_height(window) / 2.0f;
    lx_window_flags_set(window, LX_WINDOW_FLAG_SHOW_FPS);
    lx_quality_set(g_quality);
    lx_matrix_init_translate(&g_matrix, x0, y0);

    static lx_color_t colors[3];
    colors[0] = LX_COLOR_RED;
    colors[1] = LX_COLOR_GREEN;
    colors[2] = LX_COLOR_BLUE;
    lx_gradient_t gradent = {colors, lx_null, 3};
    g_shaders[1] = lx_shader_init2i_linear_gradient(LX_SHADER_TILE_MODE_CLAMP, &gradent, -50, -50, 50, 50);
    g_shaders[2] = lx_shader_init2i_radial_gradient(LX_SHADER_TILE_MODE_CLAMP, &gradent, 0, 0, 50);
    if (argc > 2) {
        g_bitmap = lx_bitmap_init_from_file(argv[2], lx_window_pixfmt(window));
        if (g_bitmap) {
            g_shaders[0] = lx_shader_init_bitmap(LX_SHADER_TILE_MODE_CLAMP, g_bitmap);
        }
    }
    if (g_entry->on_init) {
        g_entry->on_init(window);
    }
}

static lx_void_t window_exit(lx_window_ref_t window) {
    lx_size_t i = 0;
    for (i = 0; i < lx_arrayn(g_shaders); i++) {
        if (g_shaders[i]) {
            lx_shader_exit(g_shaders[i]);
            g_shaders[i] = lx_null;
        }
    }
    if (g_bitmap) {
        lx_bitmap_exit(g_bitmap);
        g_bitmap = lx_null;
    }
    if (g_entry->on_exit) {
        g_entry->on_exit(window);
    }
    lx_window_exit(window);
}

#ifndef LX_NOMAIN_ENTRY
int main(int argc, char** argv) {
    lx_window_ref_t window = lx_window_init(640, 640, "lanox2d", lx_null);
    if (window) {
        window_init(window, argc, argv);
        lx_window_runloop(window);
        window_exit(window);
    }
    return 0;
}
#endif
