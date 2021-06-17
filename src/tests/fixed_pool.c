#include "lanox2d/lanox2d.h"

#if 0
static lx_void_t lx_test_fixed_pool_leak() {
    lx_fixed_pool_ref_t pool = lx_null;
    do {
        pool = lx_fixed_pool_init(0, sizeof(lx_size_t), lx_null, lx_null);
        lx_assert_and_check_break(pool);

        lx_pointer_t data0 = lx_fixed_pool_malloc(pool);
        lx_assert_and_check_break(data0);

        lx_pointer_t data1 = lx_fixed_pool_malloc(pool);
        lx_assert_and_check_break(data1);

#ifdef LX_DEBUG
        lx_fixed_pool_dump(pool);
#endif

    } while (0);

    if (pool) lx_fixed_pool_exit(pool);
}
#endif

#if 0
static lx_void_t lx_test_fixed_pool_free2() {
    lx_fixed_pool_ref_t pool = lx_null;
    do {
        pool = lx_fixed_pool_init(0, sizeof(lx_size_t), lx_null, lx_null);
        lx_assert_and_check_break(pool);

        lx_pointer_t data = lx_fixed_pool_malloc(pool);
        lx_assert_and_check_break(data);

        lx_fixed_pool_free(pool, data);
        lx_fixed_pool_free(pool, data);

#ifdef LX_DEBUG
        lx_fixed_pool_dump(pool);
#endif
    } while (0);

    if (pool) lx_fixed_pool_exit(pool);
}
#endif

#if 0
static lx_void_t lx_test_fixed_pool_underflow() {
    lx_fixed_pool_ref_t pool = lx_null;
    do {
        pool = lx_fixed_pool_init(0, sizeof(lx_size_t), lx_null, lx_null);
        lx_assert_and_check_break(pool);

        lx_pointer_t data = lx_fixed_pool_malloc(pool);
        lx_assert_and_check_break(data);

        lx_memset(data, 0, sizeof(lx_size_t) + 1);

        lx_fixed_pool_free(pool, data);

#ifdef LX_DEBUG
        lx_fixed_pool_dump(pool);
#endif
    } while (0);

    if (pool) lx_fixed_pool_exit(pool);
}
#endif

#if 0
static lx_void_t lx_test_fixed_pool_underflow2() {
    lx_fixed_pool_ref_t pool = lx_null;
    do {
        pool = lx_fixed_pool_init(0, sizeof(lx_size_t), lx_null, lx_null);
        lx_assert_and_check_break(pool);

        lx_pointer_t data = lx_fixed_pool_malloc(pool);
        lx_assert_and_check_break(data);

        lx_memset(data, 0, sizeof(lx_size_t) + 1);

        data = lx_fixed_pool_malloc(pool);
        lx_assert_and_check_break(data);

#ifdef LX_DEBUG
        lx_fixed_pool_dump(pool);
#endif
    } while (0);

    if (pool) lx_fixed_pool_exit(pool);
}
#endif

static lx_void_t lx_test_fixed_pool_perf(lx_size_t item_size) {
    lx_fixed_pool_ref_t pool = lx_null;
    do {
        pool = lx_fixed_pool_init(0, item_size, lx_null, lx_null);
        lx_assert_and_check_break(pool);

        lx_size_t       maxn = 10000;
        lx_pointer_t*   list = (lx_pointer_t*)lx_nalloc0(maxn, sizeof(lx_pointer_t));
        lx_assert_and_check_break(list);

        lx_volatile lx_size_t indx = 0;
        lx_volatile lx_hong_t time = lx_mclock();
        lx_volatile lx_size_t rand = 0xbeaf;
        for (indx = 0; indx < maxn; indx++) {
            list[indx] = lx_fixed_pool_malloc(pool);
            lx_assert_and_check_break(list[indx]);

            rand = (rand * 10807 + 1) & 0xffffffff;

            lx_volatile lx_size_t size = rand & 15;
            if (size > 5 && indx) {
                size -= 5;
                while (size--) {
                    lx_size_t free_indx = rand % indx;
                    if (list[free_indx]) {
                        lx_fixed_pool_free(pool, list[free_indx]);
                        list[free_indx] = lx_null;
                    }
                }
            }
        }
        time = lx_mclock() - time;

#ifdef LX_DEBUG
        lx_fixed_pool_dump(pool);
#endif

        lx_trace_i("time: %lld ms", time);
        lx_fixed_pool_clear(pool);
        lx_free(list);

    } while (0);

    if (pool) lx_fixed_pool_exit(pool);
}

int main(int argc, char** argv) {
#if 1
    lx_test_fixed_pool_perf(16);
    lx_test_fixed_pool_perf(32);
    lx_test_fixed_pool_perf(64);
    lx_test_fixed_pool_perf(96);
    lx_test_fixed_pool_perf(128);
    lx_test_fixed_pool_perf(192);
    lx_test_fixed_pool_perf(256);
    lx_test_fixed_pool_perf(384);
    lx_test_fixed_pool_perf(512);
    lx_test_fixed_pool_perf(1024);
    lx_test_fixed_pool_perf(2048);
    lx_test_fixed_pool_perf(3072);
#endif

#if 0
    lx_test_fixed_pool_leak();
#endif

#if 0
    lx_test_fixed_pool_free2();
#endif

#if 0
    lx_test_fixed_pool_underflow();
#endif

#if 0
    lx_test_fixed_pool_underflow2();
#endif
    return 0;
}

