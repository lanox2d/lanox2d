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
 * @file        priority_queue.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "priority_queue.h"
#include "heap.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */
lx_priority_queue_ref_t lx_priority_queue_init(lx_size_t grow, lx_element_t element) {
    return (lx_priority_queue_ref_t)lx_heap_init(grow, element);
}

lx_void_t lx_priority_queue_exit(lx_priority_queue_ref_t self) {
    lx_heap_exit((lx_heap_ref_t)self);
}

lx_void_t lx_priority_queue_clear(lx_priority_queue_ref_t self) {
    lx_heap_clear((lx_heap_ref_t)self);
}

lx_size_t lx_priority_queue_size(lx_priority_queue_ref_t self) {
    return lx_heap_size((lx_heap_ref_t)self);
}

lx_size_t lx_priority_queue_maxn(lx_priority_queue_ref_t self) {
    return lx_heap_maxn((lx_heap_ref_t)self);
}

lx_pointer_t lx_priority_queue_get(lx_priority_queue_ref_t self) {
    return lx_heap_top((lx_heap_ref_t)self);
}

lx_void_t lx_priority_queue_put(lx_priority_queue_ref_t self, lx_cpointer_t data) {
    lx_heap_put((lx_heap_ref_t)self, data);
}

lx_void_t lx_priority_queue_pop(lx_priority_queue_ref_t self) {
    lx_heap_pop((lx_heap_ref_t)self);
}

