#include "lanox2d/lanox2d.h"

static lx_void_t on_draw(lx_window_ref_t window, lx_canvas_ref_t canvas) {
    lx_print("draw");
}

int main(int argc, char** argv) {
    lx_window_ref_t window = lx_window_init(640, 480, "lanox2d");
    if (window) {
        lx_window_on_draw(window, on_draw);
        lx_window_runloop(window);
        lx_window_exit(window);
    }
    return 0;
}
