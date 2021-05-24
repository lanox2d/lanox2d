#include "lanox2d/lanox2d.h"

static lx_void_t on_draw(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_canvas_draw_clear(canvas, LX_COLOR_RED);
}

static lx_void_t on_event(lx_window_ref_t window, lx_event_ref_t event) {
#ifdef LX_DEBUG
    lx_event_dump(event);
#endif
}

static lx_void_t on_resize(lx_window_ref_t window, lx_canvas_ref_t canvas) {
}

int main(int argc, char** argv) {
    lx_window_ref_t window = lx_window_init(640, 480, "lanox2d");
    if (window) {
        lx_window_flags_set(window, LX_WINDOW_FLAG_SHOW_FPS);
        lx_window_on_draw(window, on_draw);
        lx_window_on_event(window, on_event);
        lx_window_on_resize(window, on_resize);
        lx_window_runloop(window);
        lx_window_exit(window);
    }
    return 0;
}
