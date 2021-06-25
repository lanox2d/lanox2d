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
static lx_void_t lx_heap_check(lx_heap_t* heap)
{
    // init
    lx_byte_t*  data = heap->data;
    lx_size_t   tail = heap->size;
    lx_size_t   step = heap->element.size;
    lx_size_t   parent = 0;

    // done
    for (; parent < tail; parent++)
    {
        // the left child node
        lx_size_t lchild = (parent << 1) + 1;
        lx_check_break(lchild < tail);

        // the parent data
        lx_pointer_t parent_data = heap->element.data(&heap->element, data + parent * step);

        // check?
        if (heap->element.comp(&heap->element, heap->element.data(&heap->element, data + lchild * step), parent_data) < 0)
        {
            // dump self
            lx_heap_dump((lx_heap_ref_t)heap);

            // abort
            lx_assertf(0, "lchild[%lu]: invalid, parent: %lu, tail: %lu", lchild, parent, tail);
        }

        // the right child node
        lx_size_t rchild = lchild + 1;
        lx_check_break(rchild < tail);

        // check?
        if (heap->element.comp(&heap->element, heap->element.data(&heap->element, data + rchild * step), parent_data) < 0)
        {
            // dump self
            lx_heap_dump((lx_heap_ref_t)heap);

            // abort
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
static lx_pointer_t lx_heap_shift_up(lx_heap_t* heap, lx_size_t hole, lx_cpointer_t data)
{
    // check
    lx_assert_and_check_return_val(heap && heap->data, lx_null);

    // the element function
    lx_element_comp_func_t func_comp = heap->element.comp;
    lx_element_data_func_t func_data = heap->element.data;
    lx_assert(func_comp && func_data);

    // (hole - 1) / 2: the parent node of the hole
    lx_size_t   parent = 0;
    lx_byte_t*  head = heap->data;
    lx_size_t   step = heap->element.size;
    switch (step)
    {
    case sizeof(lx_size_t):
        {
            for (parent = (hole - 1) >> 1; hole && (func_comp(&heap->element, func_data(&heap->element, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
            {
                // move item: parent => hole
                *((lx_size_t*)(head + hole * step)) = *((lx_size_t*)(head + parent * step));

                // move node: hole => parent
                hole = parent;
            }
        }
        break;
    default:
        for (parent = (hole - 1) >> 1; hole && (func_comp(&heap->element, func_data(&heap->element, head + parent * step), data) > 0); parent = (hole - 1) >> 1)
        {
            // move item: parent => hole
            lx_memcpy(head + hole * step, head + parent * step, step);

            // move node: hole => parent
            hole = parent;
        }
        break;
    }

    // ok?
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
static lx_pointer_t lx_heap_shift_down(lx_heap_t* heap, lx_size_t hole, lx_cpointer_t data)
{
    // check
    lx_assert_and_check_return_val(heap && heap->data, lx_null);

    // init element
    lx_element_comp_func_t func_comp = heap->element.comp;
    lx_element_data_func_t func_data = heap->element.data;
    lx_assert(func_comp && func_data);

    // 2 * hole + 1: the left child node of hole
    lx_size_t       step = heap->element.size;
    lx_byte_t*      head = heap->data;
    lx_byte_t*      tail = head + heap->size * step;
    lx_byte_t*      phole = head + hole * step;
    lx_byte_t*      lchild = head + ((hole << 1) + 1) * step;
    lx_pointer_t    data_lchild = lx_null;
    lx_pointer_t    data_rchild = lx_null;
    switch (step)
    {
    case sizeof(lx_size_t):
        {
            for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step))
            {
                // the smaller child node
                data_lchild = func_data(&heap->element, lchild);
                if (lchild + step < tail && func_comp(&heap->element, data_lchild, (data_rchild = func_data(&heap->element, lchild + step))) > 0)
                {
                    lchild += step;
                    data_lchild = data_rchild;
                }

                // end?
                if (func_comp(&heap->element, data_lchild, data) >= 0) break;

                // the smaller child node => hole
                *((lx_size_t*)phole) = *((lx_size_t*)lchild);

                // move the hole down to it's smaller child node
                phole = lchild;
            }
        }
        break;
    default:
        {
            for (; lchild < tail; lchild = head + (((lchild - head) << 1) + step))
            {
                // the smaller child node
                data_lchild = func_data(&heap->element, lchild);
                if (lchild + step < tail && func_comp(&heap->element, data_lchild, (data_rchild = func_data(&heap->element, lchild + step))) > 0)
                {
                    lchild += step;
                    data_lchild = data_rchild;
                }

                // end?
                if (func_comp(&heap->element, data_lchild, data) >= 0) break;

                // the smaller child node => hole
                lx_memcpy(phole, lchild, step);

                // move the hole down to it's smaller child node
                phole = lchild;
            }
        }
        break;
    }

    // ok?
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
    return itor < heap>size? itor + 1 : heap->size;
}

static lx_size_t lx_heap_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    return itor? itor - 1 : 0;
}

static lx_pointer_t lx_heap_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_heap_t* heap = (lx_heap_t*)iterator->container;
    return itor < heap->size? heap->data + itor * heap->element.size : lx_null;
}

static lx_void_t lx_heap_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_heap_iterator_head,
        lx_heap_iterator_tail,
        lx_heap_iterator_prev,
        lx_heap_iterator_next,
        lx_heap_iterator_item
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_RACCESS | LX_ITERATOR_MODE_MUTABLE;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_heap_ref_t lx_heap_init(lx_size_t grow, lx_element_t element) {
    lx_assert_and_check_return_val(element.size, lx_null);

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
#if 0 // TODO
        if (heap->element.nfree)
            heap->element.nfree(&heap->element, heap->data, heap->size);
#endif
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
    return heap->data;
}

lx_void_t lx_heap_put(lx_heap_ref_t self, lx_cpointer_t data) {
    lx_heap_t* heap = (lx_heap_t*)self;
    lx_assert_and_check_return(heap && heap->element.dupl && heap->data);

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
        heap->element.dupl(&heap->element, hole, data);
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
        heap->element.free(heap->data);
    }

    // the last item is not in top
    if (heap->size > 1) {
        lx_assert(heap->element.data);

        // the step
        lx_size_t step = heap->element.size;
        lx_assert(step);

        // the last
        lx_pointer_t last = heap->data + (heap->size - 1) * step;

        // shift down the self from the top hole
        lx_pointer_t hole = lx_heap_shift_down(heap, 0, heap->element.data(&heap->element, last));
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

