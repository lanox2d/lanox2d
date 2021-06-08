#include "lanox2d/lanox2d.h"

static lx_void_t test_length_near0() {
    lx_vector_t vector;
    lx_vector_make(&vector, LX_NEAR0, LX_NEAR0);
    lx_float_t length = lx_vector_length(&vector);
    lx_trace_i("%{vector}.length: %{float}", &vector, &length);
}

static lx_void_t test_length(lx_long_t x, lx_long_t y) {
    lx_vector_t vector;
    lx_vector_imake(&vector, x, y);
    lx_float_t length = lx_vector_length(&vector);
    lx_trace_i("%{vector}.length: %{float}", &vector, &length);
}

static lx_void_t test_normalize_near0() {
    lx_vector_t vector;
    lx_vector_make(&vector, LX_NEAR0, LX_NEAR0);

    lx_bool_t ok = lx_vector_normalize(&vector);
    lx_float_t length = lx_vector_length(&vector);
    lx_trace_i("%{vector}.length: %{float}: %s", &vector, &length, ok? "ok" : "no");
}

static lx_void_t test_normalize(lx_long_t x, lx_long_t y) {
    lx_vector_t vector;
    lx_vector_imake(&vector, x, y);
    lx_bool_t ok = lx_vector_normalize(&vector);
    lx_float_t length = lx_vector_length(&vector);
    lx_trace_i("normalize: %{vector}.length: %{float}: %s", &vector, &length, ok? "ok" : "no");
}

static lx_void_t test_length_set(lx_long_t x, lx_long_t y, lx_float_t n) {
    lx_vector_t vector;
    lx_vector_imake(&vector, x, y);
    lx_bool_t ok = lx_vector_length_set(&vector, n);
    lx_float_t length = lx_vector_length(&vector);
    lx_trace_i("set %{vector}.length: %{float} => %{float}: %s", &vector, &n, &length, ok? "ok" : "no");
}

int main(int argc, char** argv) {

    test_length_near0();
    test_length(0, 1);
    test_length(0, 10);
    test_length(1, 10);
    test_length(10, 10);
    test_length(200, 200);
    test_length(512, 512);
    test_length(1024, 1024);
    test_length(4096, 4096);

    test_normalize_near0();
    test_normalize(0, 1);
    test_normalize(0, 10);
    test_normalize(1, 10);
    test_normalize(10, 10);
    test_normalize(512, 512);
    test_normalize(1024, 1024);
    test_normalize(4096, 4096);

    test_length_set(0, 1, 10);
    test_length_set(0, 10, 50);
    test_length_set(1, 10, 100);
    test_length_set(10, 10, 200);
    test_length_set(512, 512, 300);
    test_length_set(1024, 1024, 400);
    test_length_set(4096, 4096, 500);
    return 0;
}
