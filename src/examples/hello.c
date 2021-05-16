#include "lanox2d/lanox2d.h"

int main(int argc, char** argv) {
    lx_window_ref_t window = lx_window_init();
    if (window) {
        lx_trace_i("hello lanox2d!");
        lx_window_exit(window);
    }
    return 0;
}
