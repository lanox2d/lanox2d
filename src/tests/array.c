#include "lanox2d/lanox2d.h"

static lx_void_t itemfree(lx_pointer_t item) {
    lx_trace_i("free: %lu", *((lx_size_t*)item));
}

static lx_bool_t foreach(lx_iterator_ref_t iterator, lx_pointer_t item, lx_cpointer_t udata) {
    lx_trace_i("foreach: %lu", *((lx_size_t*)item));
    return lx_true;
}

int main(int argc, char** argv) {
    lx_array_ref_t array = lx_array_init(0, sizeof(lx_size_t), itemfree);
    if (array) {
        lx_size_t val[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        lx_trace_i("-------------------------- test insert --------------------------");
        lx_array_insert_tail(array, &val[0]);
        lx_array_insert_tail(array, &val[1]);
        lx_array_insert_tail(array, &val[2]);
        lx_array_insert_tail(array, &val[3]);
        lx_array_insert_tail(array, &val[4]);
        lx_array_insert_tail(array, &val[5]);
        lx_array_insert_tail(array, &val[6]);
        lx_array_insert_tail(array, &val[7]);
        lx_array_insert_tail(array, &val[8]);
        lx_array_insert_tail(array, &val[9]);
        lx_foreach_all(array, foreach, lx_null);
        lx_assert(*((lx_size_t*)lx_array_head(array)) == val[0]);
        lx_assert(*((lx_size_t*)lx_array_last(array)) == val[9]);
        lx_trace_i("-------------------------- test remove --------------------------");
        lx_array_remove_last(array);
        lx_foreach_all(array, foreach, lx_null);
        lx_assert(*((lx_size_t*)lx_array_last(array)) == val[8]);
        lx_trace_i("-------------------------- test replace ------------------------");
        lx_array_replace_head(array, &val[9]);
        lx_array_replace_last(array, &val[0]);
        lx_foreach_all(array, foreach, lx_null);
        lx_assert(*((lx_size_t*)lx_array_head(array)) == val[9]);
        lx_assert(*((lx_size_t*)lx_array_last(array)) == val[0]);
        lx_for_all(lx_cpointer_t, item, array) {
            lx_trace_i("for_all: %lu", *((lx_size_t*)item));
        }
        lx_array_exit(array);
    }
    return 0;
}
