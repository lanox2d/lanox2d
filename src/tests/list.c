#include "lanox2d/lanox2d.h"

static lx_void_t lx_list_mem_free(lx_pointer_t item) {
    lx_trace_i("free item: %s", item);
}

static lx_void_t lx_list_mem_dump(lx_list_ref_t list) {
    lx_for_all (lx_char_t*, item, list) {
        lx_trace_i("item at[%lx]: %s", item_itor, item);
    }
}

static lx_void_t lx_list_mem_test() {
    lx_list_ref_t   list = lx_null;
    lx_size_t       i;
    lx_size_t       j;
    do {
        // init list
        list = lx_list_init(0, lx_element_mem(11, lx_list_mem_free));
        lx_assert_and_check_break(list);

        // trace
        lx_trace_i("=============================================================");
        lx_trace_i("insert:");

        // insert head
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");
        lx_list_insert_head(list, "AAAAAAAAAA");

        // insert tail
        i = lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");
        lx_list_insert_tail(list, "FFFFFFFFFF");

        // insert prev
        j = lx_list_insert_prev(list, i, "0000000000");
        lx_list_insert_prev(list, i, "1111111111");
        lx_list_insert_prev(list, i, "2222222222");
        lx_list_insert_prev(list, i, "3333333333");
        lx_list_insert_prev(list, i, "4444444444");
        lx_list_insert_prev(list, i, "5555555555");
        lx_list_insert_prev(list, i, "6666666666");
        lx_list_insert_prev(list, i, "7777777777");
        lx_list_insert_prev(list, i, "8888888888");
        lx_list_insert_prev(list, i, "9999999999");

        // insert head
        lx_list_insert_head(list, "4444444444");
        lx_list_insert_head(list, "3333333333");
        lx_list_insert_head(list, "2222222222");
        lx_list_insert_head(list, "1111111111");
        lx_list_insert_head(list, "0000000000");

        // insert tail
        lx_list_insert_tail(list, "5555555555");
        lx_list_insert_tail(list, "6666666666");
        lx_list_insert_tail(list, "7777777777");
        lx_list_insert_tail(list, "8888888888");
        lx_list_insert_tail(list, "9999999999");

        // dump
        lx_list_mem_dump(list);

        // trace
        lx_trace_i("=============================================================");
        lx_trace_i("remove:");

        // remove
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);
        j = lx_list_remove(list, j);

        // dump
        lx_list_mem_dump(list);

        // trace
        lx_trace_i("=============================================================");
        lx_trace_i("clear:");

        // clear
        lx_list_clear(list);

        // dump
        lx_list_mem_dump(list);

    } while (0);

    if (list) {
        lx_list_exit(list);
    }
}

int main(int argc, char** argv) {
    lx_list_mem_test();
    return 0;
}
