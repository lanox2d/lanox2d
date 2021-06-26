#include "lanox2d/lanox2d.h"

static lx_long_t lx_test_heap_min_comp(lx_cpointer_t ldata, lx_cpointer_t rdata) {
    lx_size_t lval = *((lx_size_t*)ldata);
    lx_size_t rval = *((lx_size_t*)rdata);
    return ((lval < rval)? -1 : (lval > rval));
}

static lx_long_t lx_test_heap_max_comp(lx_cpointer_t ldata, lx_cpointer_t rdata) {
    lx_size_t lval = *((lx_size_t*)ldata);
    lx_size_t rval = *((lx_size_t*)rdata);
    return ((lval > rval)? -1 : (lval < rval));
}

static lx_void_t lx_test_heap_min_func() {
    lx_element_t element = lx_element_mem(sizeof(lx_size_t), lx_null); element.comp = lx_test_heap_min_comp;
    lx_heap_ref_t heap = lx_heap_init(16, element);
    lx_assert_and_check_return(heap);

    lx_size_t i = 0;
    lx_size_t val = 0;
    lx_volatile lx_size_t rand = 0xbeaf;
    for (i = 0; i < 100; i++) {
        rand = (rand * 10807 + 1) & 0xffffffff;
        val = rand % 50;
//        lx_trace_i("heap_min: put: %lu", val);
        lx_heap_put(heap, &val);
    }

    lx_trace_i("");
    while (lx_heap_size(heap)) {
        lx_size_t val = *((lx_size_t*)lx_heap_top(heap));
        lx_trace_i("heap_min: pop: %lu", val);
        lx_heap_pop(heap);
    }
    lx_heap_exit(heap);
}

static lx_void_t lx_test_heap_min_perf() {
    lx_element_t element = lx_element_mem(sizeof(lx_size_t), lx_null); element.comp = lx_test_heap_min_comp;
    lx_heap_ref_t heap = lx_heap_init(16, element);
    lx_assert_and_check_return(heap);

    lx_size_t i = 0;
    lx_size_t val = 0;
    lx_volatile lx_size_t rand = 0xbeaf;
    lx_hong_t time = lx_mclock();
    lx_volatile lx_size_t n = 100000;
    for (i = 0; i < n; i++) {
        rand = (rand * 10807 + 1) & 0xffffffff;
        val = rand % 50;
        lx_heap_put(heap, &val);
    }
    lx_volatile lx_size_t p;
    for (i = 0; lx_heap_size(heap); i++) {
        lx_size_t val = *((lx_size_t*)lx_heap_top(heap));
        lx_assert(!i || p <= val);
        p = val;
        lx_heap_pop(heap);
    }
    time = lx_mclock() - time;
    lx_trace_i("heap_min: %lld ms", time);
    lx_heap_exit(heap);
}

static lx_void_t lx_test_heap_max_func() {
    lx_element_t element = lx_element_mem(sizeof(lx_size_t), lx_null); element.comp = lx_test_heap_max_comp;
    lx_heap_ref_t heap = lx_heap_init(16, element);
    lx_assert_and_check_return(heap);

    lx_size_t i = 0;
    lx_size_t val = 0;
    lx_volatile lx_size_t rand = 0xbeaf;
    for (i = 0; i < 100; i++) {
        rand = (rand * 10807 + 1) & 0xffffffff;
        val = rand % 50;
//        lx_trace_i("heap_max: put: %lu", val);
        lx_heap_put(heap, &val);
    }

    lx_trace_i("");
    while (lx_heap_size(heap)) {
        lx_size_t val = *((lx_size_t*)lx_heap_top(heap));
        lx_trace_i("heap_max: pop: %lu", val);
        lx_heap_pop(heap);
    }
    lx_heap_exit(heap);
}

static lx_void_t lx_test_heap_max_perf() {
    lx_element_t element = lx_element_mem(sizeof(lx_size_t), lx_null); element.comp = lx_test_heap_max_comp;
    lx_heap_ref_t heap = lx_heap_init(16, element);
    lx_assert_and_check_return(heap);

    lx_size_t i = 0;
    lx_size_t val = 0;
    lx_volatile lx_size_t rand = 0xbeaf;
    lx_hong_t time = lx_mclock();
    lx_volatile lx_size_t n = 100000;
    for (i = 0; i < n; i++) {
        rand = (rand * 10807 + 1) & 0xffffffff;
        val = rand % 50;
        lx_heap_put(heap, &val);
    }
    lx_volatile lx_size_t p;
    for (i = 0; lx_heap_size(heap); i++) {
        lx_size_t val = *((lx_size_t*)lx_heap_top(heap));
        lx_assert(!i || p <= val);
        p = val;
        lx_heap_pop(heap);
    }
    time = lx_mclock() - time;
    lx_trace_i("heap_max: %lld ms", time);
    lx_heap_exit(heap);
}

int main(int argc, char** argv) {
    lx_test_heap_min_func();
    lx_test_heap_max_func();
    lx_test_heap_min_perf();
    lx_test_heap_max_perf();
    return 0;
}
