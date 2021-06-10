#include "lanox2d/lanox2d.h"

int main(int argc, char** argv) {
    lx_path_ref_t path = lx_path_init();
    if (path) {
        // init path
        lx_path_move2i_to(path, 100, 100);
        lx_path_line2i_to(path, 200, 200);
        lx_path_quad2i_to(path, 300, 300, 400, 400);
        lx_path_cubic2i_to(path, 500, 500, 600, 600, 700, 700);
        lx_path_close(path);

        // copy path
        lx_path_ref_t copy = lx_path_init();
        if (copy) {
            lx_path_copy(copy, path);
#ifdef LX_DEBUG
            lx_path_dump(copy);
#endif
            lx_path_exit(copy);
        }
        lx_path_exit(path);
    }
    return 0;
}
