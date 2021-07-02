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
 * @file        fixed_array.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fixed_array.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * private implementation
 */
static lx_size_t lx_fixed_array_ptr_iterator_head(lx_iterator_ref_t iterator) {
    return 0;
}

static lx_size_t lx_fixed_array_ptr_iterator_tail(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    return ((lx_fixed_array_ref_t)iterator->container)->count;
}

static lx_size_t lx_fixed_array_ptr_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_assert(itor < ((lx_fixed_array_ref_t)iterator->container)->count);
    return itor + 1;
}

static lx_size_t lx_fixed_array_ptr_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && itor);
    return itor - 1;
}

static lx_pointer_t lx_fixed_array_ptr_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_assert(itor < ((lx_fixed_array_ref_t)iterator->container)->count);
    return ((lx_pointer_t*)((lx_fixed_array_ref_t)iterator->container)->items)[itor];
}

static lx_size_t lx_fixed_array_ptr_iterator_size(lx_iterator_ref_t iterator) {
    lx_assert(iterator && iterator->container);
    return ((lx_fixed_array_ref_t)iterator->container)->count;
}

static lx_long_t lx_fixed_array_ptr_iterator_comp(lx_iterator_ref_t iterator, lx_cpointer_t litem, lx_cpointer_t ritem) {
    return (litem < ritem)? -1 : (litem > ritem);
}

static lx_void_t lx_fixed_array_ptr_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_fixed_array_ptr_iterator_head,
        lx_fixed_array_ptr_iterator_tail,
        lx_fixed_array_ptr_iterator_prev,
        lx_fixed_array_ptr_iterator_next,
        lx_fixed_array_ptr_iterator_item,
        lx_fixed_array_ptr_iterator_size,
        lx_fixed_array_ptr_iterator_comp
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_RACCESS | LX_ITERATOR_MODE_MUTABLE;
    iterator->op        = &op;
}

static lx_pointer_t lx_fixed_array_mem_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor) {
    lx_assert(iterator && iterator->container);
    lx_fixed_array_ref_t fixed_array = (lx_fixed_array_ref_t)iterator->container;
    lx_assert(itor < fixed_array->count);
    return (lx_pointer_t)((lx_byte_t*)(fixed_array->items) + itor * fixed_array->itemsize);
}

static lx_void_t lx_fixed_array_mem_iterator_of(lx_iterator_ref_t iterator, lx_cpointer_t container) {
    static lx_iterator_op_t op = {
        lx_fixed_array_ptr_iterator_head,
        lx_fixed_array_ptr_iterator_tail,
        lx_fixed_array_ptr_iterator_prev,
        lx_fixed_array_ptr_iterator_next,
        lx_fixed_array_mem_iterator_item,
        lx_fixed_array_ptr_iterator_size
    };
    iterator->container = container;
    iterator->mode      = LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_REVERSE | LX_ITERATOR_MODE_RACCESS | LX_ITERATOR_MODE_MUTABLE;
    iterator->op        = &op;
}

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_void_t lx_fixed_array_init_ptr(lx_fixed_array_ref_t self, lx_pointer_t* items, lx_size_t count) {
    lx_assert(self && items && count);
    self->base.iterator_of = lx_fixed_array_ptr_iterator_of;
    self->items            = items;
    self->count            = count;
}

lx_void_t lx_fixed_array_init_mem(lx_fixed_array_ref_t self, lx_pointer_t items, lx_size_t count, lx_size_t itemsize) {
    lx_assert(self && items && count);
    self->base.iterator_of = lx_fixed_array_mem_iterator_of;
    self->items            = items;
    self->count            = count;
    self->itemsize         = itemsize;
}
