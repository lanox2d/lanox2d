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
 * @file        array_iterator.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "array_iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_size_t lx_array_iterator_ptr_head(lx_iterator_ref_t iterator) {
    return 0;
}

static lx_size_t lx_array_iterator_ptr_tail(lx_iterator_ref_t iterator) {
    lx_assert(iterator);
    return ((lx_array_iterator_ref_t)iterator)->count;
}

static lx_size_t lx_array_iterator_ptr_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && itor < ((lx_array_iterator_ref_t)iterator)->count);
    return itor + 1;
}

static lx_size_t lx_array_iterator_ptr_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && itor);
    return itor - 1;
}

static lx_pointer_t lx_array_iterator_ptr_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && itor < ((lx_array_iterator_ref_t)iterator)->count);
    return ((lx_pointer_t*)((lx_array_iterator_ref_t)iterator)->items)[itor];
}

static lx_long_t lx_array_iterator_ptr_comp(lx_iterator_ref_t iterator, lx_cpointer_t litem, lx_cpointer_t ritem) {
    return (litem < ritem)? -1 : (litem > ritem);
}

static lx_void_t lx_array_iterator_of_ptr(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_array_iterator_ptr_head,
        lx_array_iterator_ptr_tail,
        lx_array_iterator_ptr_prev,
        lx_array_iterator_ptr_next,
        lx_array_iterator_ptr_item,
        lx_array_iterator_ptr_comp
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_RACCESS | LX_ITERATOR_MODE_MUTABLE;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_array_iterator_init_ptr(lx_array_iterator_ref_t iterator, lx_pointer_t* items, lx_size_t count) {
    lx_assert(iterator && items && count);
    iterator->base.iterator_of = lx_array_iterator_of_ptr;
    iterator->items            = items;
    iterator->count            = count;
}
