#include "lanox2d/lanox2d.h"

static lx_void_t itemfree(lx_pointer_t item) {
    lx_trace_i("free: %lu", *((lx_size_t*)item));
}

static lx_void_t foreach(lx_pointer_t item, lx_cpointer_t udata) {
    lx_trace_i("foreach: %lu", *((lx_size_t*)item));
}

int main(int argc, char** argv) {
    lx_stack_ref_t stack = lx_stack_init(0, sizeof(lx_size_t), itemfree);
    if (stack) {
        lx_size_t val[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
        lx_trace_i("-------------------------- test push --------------------------");
        lx_stack_push(stack, &val[0]);
        lx_stack_push(stack, &val[1]);
        lx_stack_push(stack, &val[2]);
        lx_stack_push(stack, &val[3]);
        lx_stack_push(stack, &val[4]);
        lx_stack_push(stack, &val[5]);
        lx_stack_push(stack, &val[6]);
        lx_stack_push(stack, &val[7]);
        lx_stack_push(stack, &val[8]);
        lx_stack_push(stack, &val[9]);
        lx_assert(*((lx_size_t*)lx_stack_head(stack)) == val[0]);
        lx_assert(*((lx_size_t*)lx_stack_last(stack)) == val[9]);
        lx_assert(*((lx_size_t*)lx_stack_top(stack)) == val[9]);
        lx_stack_foreach(stack, foreach, lx_null);
        lx_trace_i("-------------------------- test pop ---------------------------");
        lx_stack_pop(stack);
        lx_stack_foreach(stack, foreach, lx_null);
        lx_assert(*((lx_size_t*)lx_stack_top(stack)) == val[8]);
        lx_stack_exit(stack);
    }
    return 0;
}

