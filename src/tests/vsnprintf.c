#include "lanox2d/lanox2d.h"

#ifdef LX_DEBUG
static lx_int_t lx_vsnprintf_test(lx_char_t* s, lx_size_t n, lx_cpointer_t object) {
    return lx_snprintf(s, n, "%s", object);
}
#endif

int main(int argc, char** argv) {
#ifdef LX_DEBUG
    lx_vsnprintf_object_register("test", lx_vsnprintf_test);
#endif
    lx_trace_i("%{test}", "hello");
    return 0;
}
