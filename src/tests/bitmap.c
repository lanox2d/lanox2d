#include "lanox2d/lanox2d.h"

int main(int argc, char** argv) {
    lx_bitmap_ref_t bitmap = lx_bitmap_init_from_file(argv[1], LX_PIXFMT_XRGB8888);
    if (bitmap) {
        lx_trace_i("bitmap: %lux%lu, row_bytes: %lu, pixfmt: %s",
            lx_bitmap_width(bitmap), lx_bitmap_height(bitmap), lx_bitmap_row_bytes(bitmap),
            lx_pixmap(lx_bitmap_pixfmt(bitmap), 0xff)->name);
        lx_bitmap_exit(bitmap);
    } else {
        lx_trace_i("load %s failed!", argv[1]);
    }
    return 0;
}
