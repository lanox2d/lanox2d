#include "lanox2d/lanox2d.h"

int main(int argc, char** argv) {
    lx_window_ref_t window = lx_window_init(800, 450, "hello lanox2d!");
    if (window) {
        while (!lx_window_is_closed(window)) {
            // TODO
        }
        lx_window_exit(window);
    }
    return 0;
}
