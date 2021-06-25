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
 * @file        heap.h
 *
 */
#ifndef LX_BASE_CONTAINER_HEAP_H
#define LX_BASE_CONTAINER_HEAP_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the head ref type
 *
 * <pre>
 * heap:    1      4      2      6       9       7       8       10       14       16
 *
 *                                          1(head)
 *                               -------------------------
 *                              |                         |
 *                              4                         2
 *                        --------------             -------------
 *                       |              |           |             |
 *                       6       (last / 2 - 1)9    7             8
 *                   ---------       ----
 *                  |         |     |
 *                  10        14    16(last - 1)
 *
 * performance:
 *
 * put: O(lgn)
 * pop: O(1)
 * top: O(1)
 * del: O(lgn) + find: O(n)
 *
 * iterator:
 *
 * next: fast
 * prev: fast
 *
 * </pre>
 *
 * @note the itor of the same item is mutable
 */

/// the heap ref type
typedef lx_typeref(heap);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init heap, default: minheap
 *
 * @param grow      the item grow, using the default grow if be zero
 * @param element   the element
 *
 * @return          the heap
 */
lx_heap_ref_t       lx_heap_init(lx_size_t grow, lx_element_t element);

/*! exit heap
 *
 * @param heap      the heap
 */
lx_void_t           lx_heap_exit(lx_heap_ref_t heap);

/*! clear the heap
 *
 * @param heap      the heap
 */
lx_void_t           lx_heap_clear(lx_heap_ref_t heap);

/*! the heap size
 *
 * @param heap      the heap
 *
 * @return          the heap size
 */
lx_size_t           lx_heap_size(lx_heap_ref_t heap);

/*! the heap maxn
 *
 * @param heap      the heap
 *
 * @return          the heap maxn
 */
lx_size_t           lx_heap_maxn(lx_heap_ref_t heap);

/*! the heap top item
 *
 * @param heap      the heap
 *
 * @return          the heap top item
 */
lx_pointer_t        lx_heap_top(lx_heap_ref_t heap);

/*! put the heap item
 *
 * @param heap      the heap
 * @param data      the item data
 */
lx_void_t           lx_heap_put(lx_heap_ref_t heap, lx_cpointer_t data);

/*! pop the heap item
 *
 * @param heap      the heap
 */
lx_void_t           lx_heap_pop(lx_heap_ref_t heap);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif

