#include "lanox2d/lanox2d.h"

static lx_void_t on_draw(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_draw_clear(canvas, LX_COLOR_RED);
}

static lx_void_t on_event(lx_window_ref_t window, lx_event_ref_t event) {
#ifdef LX_DEBUG
    lx_event_dump(event);
#endif
    if (event->type == LX_EVENT_TYPE_KEYBOARD && event->u.keyboard.pressed) {
        switch (event->u.keyboard.code) {
        case LX_KEY_ESCAPE:
        case 'q':
            lx_window_quit(window);
            break;
        case 'f':
            if (lx_window_is_fullscreen(window)) {
                lx_window_show_cursor(window, lx_true);
                lx_window_fullscreen(window, lx_false);
            } else {
                lx_window_show_cursor(window, lx_false);
                lx_window_fullscreen(window, lx_true);
            }
        default:
            break;
        }
    }
}

#ifndef LX_NOMAIN_ENTRY
int main(int argc, char** argv) {
    lx_window_ref_t window = lx_window_init(640, 480, "lanox2d", lx_null);
    if (window) {
        lx_window_flags_set(window, LX_WINDOW_FLAG_SHOW_FPS);
        lx_window_on_draw(window, on_draw);
        lx_window_on_event(window, on_event);
        lx_window_runloop(window);
        lx_window_exit(window);
    }
    return 0;
}
#endif
