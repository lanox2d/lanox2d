/*!A lightweight and fast 2D vector graphics engine
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2021-present, Lanox2D Open Source Group.
 *
 * @author      ruki
 * @file        heap.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "heap.h"
#include "iterator.h"
#include "../libc/libc.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

// the self grow
#ifdef LX_CONFIG_SMALL
#   define LX_HEAP_GROW             (128)
#else
#   define LX_HEAP_GROW             (256)
#endif

// the self maxn
#ifdef LX_CONFIG_SMALL
#   define LX_HEAP_MAXN             (1 << 16)
#else
#   define LX_HEAP_MAXN             (1 << 30)
#endif

// enable check
#ifdef LX_DEBUG
#   define LX_HEAP_CHECK_ENABLE     (0)
#else
#   define LX_HEAP_CHECK_ENABLE     (0)
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

// the heap type
typedef struct lx_heap_t_ {
    lx_iterator_base_t      base;
    lx_byte_t*              data;
    lx_size_t               size;
    lx_size_t               maxn;
    lx_size_t               grow;
    lx_element_t            element;
}lx_heap_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
#if LX_HEAP_CHECK_ENABLE
static lx_void_t lx_heap_check(lx_heap_t* heap) {
    lx_byte_t*  data = heap->data;
    lx_size_t   tail = heap->size;
    lx_size_t   step = heap->element.size;
    lx_size_t   parent = 0;
    for (; parent < tail; parent++) {
        lx_pointer_t parent_data = data + parent * heap->element.size;

        // check left child node
        lx_size_t lchild = (parent << 1) + 1;
        lx_check_break(lchild < tail);
        if (heap->element.comp(data + lchild * heap->element.size, parent_data) < 0) {
            lx_heap_dump((lx_heap_ref_t)heap);
            lx_assertf(0, "lchild[%lu]: invalid, parent: %lu, tail: %lu", lchild, parent, tail);
        }

        // check right child node
        lx_size_t rchild = lchild + 1;
        lx_check_break(rchild < tail);
        if (heap->element.comp(data + rchild * heap->element.size, parent_data) < 0) {
            lx_heap_dump((lx_heap_ref_t)heap);
            lx_assertf(0, "rchild[%lu]: invalid, parent: %lu, tail: %lu", rchild, parent, tail);
        }
    }
}
#endif

/*! shift up the self
 *
 * <pre>
 *
 * before:
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6(parent)      9           7             8
 *                   ---------
 *                  |         |
 *                  10      5(hole) <------ data
 * after:
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *         data -------> 5(hole)        9           7             8
 *                   ---------
 *                  |         |
 *                  10        6
 * </pre>
 */
static lx_pointer_t lx_heap_shift_up(lx_heap_t* heap, lx_size_t hole, lx_cpointer_t data) {
    lx_element_comp_t comp = heap->element.comp;
    lx_assert(heap && heap->data && comp);

    // (hole - 1) / 2: the parent node of the hole
    lx_size_t   parent = 0;
    lx_byte_t*  head = heap->data;
    lx_size_t   step = heap->element.size;
    switch (step) {
    case sizeof(lx_size_t): {
            for (parent = (hole - 1) >> 1; hole && (comp(head + parent * step, data) > 0); parent = (hole - 1) >> 1) {
                // move item: parent => hole
                *((lx_size_t*)(head + hole * step)) = *((lx_size_t*)(head + parent * step));
                // move node: hole => parent
                hole = parent;
            }
        }
        break;
    default:
        for (parent = (hole - 1) >> 1; hole && (comp(head + parent * step, data) > 0); parent = (hole - 1) >> 1) {
            // move item: parent => hole
            lx_memcpy(head + hole * step, head + parent * step, step);
            // move node: hole => parent
            hole = parent;
        }
        break;
    }
    return head + hole * step;
}

/*! shift down the self
 *
 * <pre>
 *
 * before:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                           (hole)                       2
 *                        --------------             -------------
 *                       |              |           |             |
 *            lchild --> 6(smaller)     7           7             8
 *                   ---------     ------
 *                  |         |   |
 *                 11        16  10
 *
 *
 * move hole:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                     (hole)           7           7             8
 *                   ---------      -----
 *                  |         |    |
 *      lchild --> 11(smaller)16  10
 *
 * 11 >= data: 9? break it
 *
 * move data to hole:
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              6                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *    data ------------> 9              7           7             8
 *                   ---------       ---
 *                  |         |     |
 *                 11        16    10
 *
 * </pre>
 */
static lx_pointer_t lx_heap_shift_down(lx_heap_t* heap, lx_size_t hole, lx_cpointer_t data) {
    lx_element_comp_t comp = heap->element.comp;
    lx_assert(heap && heap->data && comp);

    // 2 * hole + 1: the left child node of hole
    lx_size_t       step = heap->element.size;
    lx_byte_t*      head = heap->data;
    lx_byte_t*      tail = head + heap->size * step;
    lx_byte_t*      phole = head + hole * step;
    lx_byte_t*      lchild = head + ((hole << 1) + 1) * step;
    lx_pointer_t    data_lchild = lx_null;
    lx_pointer_t    data_rchild = lx_null;
    switch (step) {
    case sizeof(lx_size_t): {
            for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step)) {
                // the smaller child node
                data_lchild = lchild;
                if (lchild + step < tail && comp(data_lchild, (data_rchild = lchild + step)) > 0) {
                    lchild += step;
                    data_lchild = data_rchild;
                }

                // end?
                if (comp(data_lchild, data) >= 0) {
                    break;
                }

                // the smaller child node => hole
                *((lx_size_t*)phole) = *((lx_size_t*)lchild);

                // move the hole down to it's smaller child node
                phole = lchild;
            }
        }
        break;
    default: {
            for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step)) {
                // the smaller child node
                data_lchild = lchild;
                if (lchild + step < tail && comp(data_lchild, (data_rchild = lchild + step)) > 0) {
                    lchild += step;
                    data_lchild = data_rchild;
                }

                // end?
                if (comp(data_lchild, data) >= 0) {
                    break;
                }

                // the smaller child node => hole
                lx_memcpy(phole, lchild, step);

                // move the hole down to it's smaller child node
                phole = lchild;
            }
        }
        break;
    }
    return phole;
}

static lx_size_t lx_heap_iterator_head(lx_iterator_ref_t iterator) {
    return 0;
}

static lx_size_t lx_heap_iterator_tail(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    return heap? heap->size : 0;
}

static lx_size_t lx_heap_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    return itor < heap->size? itor + 1 : heap->size;
}

static lx_size_t lx_heap_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    return itor? itor - 1 : 0;
}

static lx_pointer_t lx_heap_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    return itor < heap->size? heap->data + itor * heap->element.size : lx_null;
}

static lx_void_t lx_heap_iterator_remove(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    lx_assert_and_check_return(heap && heap->data && heap->size && itor < heap->size);

    // free the item first
    lx_size_t step = heap->element.size;
    if (heap->element.free) {
        heap->element.free(heap->data + itor * step, heap->element.udata);
    }

    // the removed item is not the last item?
    if (itor != heap->size - 1) {
        lx_pointer_t last = heap->data + (heap->size - 1) * step;
        lx_pointer_t parent = heap->data + ((itor - 1) >> 1) * step;

        /* we might need to shift it upward if it is less than its parent,
         * or downward if it is greater than one or both its children.
         *
         * since the children are known to be less than the parent,
         * it can't need to shift both up and down.
         */
        lx_pointer_t hole = lx_null;
        if (itor && heap->element.comp(parent, last) > 0) {
            // shift up the self from the given hole
            hole = lx_heap_shift_up(heap, itor, last);
        }
        // shift down the self from the given hole
        else {
            hole = lx_heap_shift_down(heap, itor, last);
        }
        lx_assert(hole);

        // copy the last data to the hole
        if (hole != last) {
            lx_memcpy(hole, last, step);
        }
    }
    heap->size--;
#if LX_HEAP_CHECK_ENABLE
    lx_heap_check(heap);
#endif
}

static lx_size_t lx_heap_iterator_size(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    return heap->size;
}

static lx_long_t lx_heap_iterator_comp(lx_iterator_ref_t iterator, lx_cpointer_t litem, lx_cpointer_t ritem) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    return heap->element.comp(litem, ritem);
}

static lx_void_t lx_heap_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_heap_iterator_head,
        lx_heap_iterator_tail,
        lx_heap_iterator_prev,
        lx_heap_iterator_next,
        lx_heap_iterator_item,
        lx_heap_iterator_size,
        lx_heap_iterator_comp,
        lx_heap_iterator_remove
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_RACCESS | LX_ITERATOR_MODE_MUTABLE;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_heap_ref_t lx_heap_init(lx_size_t grow, lx_element_t element) {
    lx_assert_and_check_return_val(element.size && element.comp, lx_null);

    lx_bool_t   ok = lx_false;
    lx_heap_t*  heap = lx_null;
    do {

        // init heap
        heap = lx_malloc0_type(lx_heap_t);
        lx_assert_and_check_break(heap);

        heap->size             = 0;
        heap->grow             = grow? grow : LX_HEAP_GROW;
        heap->maxn             = heap->grow;
        heap->element          = element;
        heap->base.iterator_of = lx_heap_iterator_of;
        lx_assert_and_check_break(heap->maxn < LX_HEAP_MAXN);

        // make data
        heap->data = (lx_byte_t*)lx_nalloc0(heap->maxn, element.size);
        lx_assert_and_check_break(heap->data);

        // ok
        ok = lx_true;

    } while (0);

    if (!ok && heap) {
        lx_heap_exit((lx_heap_ref_t)heap);
        heap = lx_null;
    }
    return (lx_heap_ref_t)heap;
}

lx_void_t lx_heap_exit(lx_heap_ref_t self) {
    lx_heap_t* heap = (lx_heap_t*)self;
    if (heap) {
        lx_heap_clear(self);
        if (heap->data) {
            lx_free(heap->data);
            heap->data = lx_null;
        }
        lx_free(heap);
    }
}

lx_void_t lx_heap_clear(lx_heap_ref_t self) {
    lx_heap_t* heap = (lx_heap_t*)self;
    if (heap) {
        if (heap->element.free) {
            lx_size_t size = heap->size;
            lx_size_t i = 0;
            for (i = 0; i < size; i++) {
                heap->element.free(heap->data + i * heap->element.size, heap->element.udata);
            }
        }
        heap->size = 0;
    }
}

lx_size_t lx_heap_size(lx_heap_ref_t self) {
    lx_heap_t const* heap = (lx_heap_t const*)self;
    return heap? heap->size : 0;
}

lx_size_t lx_heap_maxn(lx_heap_ref_t self) {
    lx_heap_t const* heap = (lx_heap_t const*)self;
    return heap? heap->maxn : 0;
}

lx_pointer_t lx_heap_top(lx_heap_ref_t self) {
    lx_heap_t const* heap = (lx_heap_t const*)self;
    return heap? heap->data : lx_null;
}

lx_void_t lx_heap_put(lx_heap_ref_t self, lx_cpointer_t data) {
    lx_heap_t* heap = (lx_heap_t*)self;
    lx_assert_and_check_return(heap && heap->data);

    // no enough? grow it
    if (heap->size == heap->maxn) {
        lx_size_t maxn = lx_align4(heap->maxn + heap->grow);
        lx_assert_and_check_return(maxn < LX_HEAP_MAXN);

        heap->data = (lx_byte_t*)lx_ralloc(heap->data, maxn * heap->element.size);
        lx_assert_and_check_return(heap->data);

        lx_assert_and_check_return(!(((lx_size_t)(heap->data)) & 3));
        lx_memset(heap->data + heap->size * heap->element.size, 0, (maxn - heap->maxn) * heap->element.size);
        heap->maxn = maxn;
    }

    lx_assert_and_check_return(heap->size < heap->maxn);

    // shift up the self from the tail hole
    lx_pointer_t hole = lx_heap_shift_up(heap, heap->size, data);
    lx_assert(hole);

    // save data to the hole
    if (hole) {
        lx_memcpy(hole, data, heap->element.size);
    }
    heap->size++;
#if LX_HEAP_CHECK_ENABLE
    lx_heap_check(heap);
#endif
}

lx_void_t lx_heap_pop(lx_heap_ref_t self) {
    lx_heap_t* heap = (lx_heap_t*)self;
    lx_assert_and_check_return(heap && heap->data && heap->size);

    // free the top item first
    if (heap->element.free) {
        heap->element.free(heap->data, heap->element.udata);
    }

    // the last item is not in top
    if (heap->size > 1) {

        // the step
        lx_size_t step = heap->element.size;
        lx_assert(step);

        // the last
        lx_pointer_t last = heap->data + (heap->size - 1) * step;

        // shift down the self from the top hole
        lx_pointer_t hole = lx_heap_shift_down(heap, 0, last);
        lx_assert(hole);

        // copy the last data to the hole
        if (hole != last) {
            lx_memcpy(hole, last, step);
        }
    }
    heap->size--;
#if LX_HEAP_CHECK_ENABLE
    lx_heap_check(heap);
#endif
}

