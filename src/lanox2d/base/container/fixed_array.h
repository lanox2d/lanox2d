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
 * @file        fixed_array.h
 *
 */
#ifndef LX_BASE_CONTAINER_FIXED_ARRAY_H
#define LX_BASE_CONTAINER_FIXED_ARRAY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "iterator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the array iterator type
typedef struct lx_fixed_array_t_ {
    lx_iterator_base_t      base;
    lx_pointer_t            items;
    lx_size_t               count;
    lx_size_t               itemsize;
}lx_fixed_array_t, *lx_fixed_array_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init pointer fixed array
 *
 * @param array     the array
 * @param items     the items
 * @param count     the count
 */
lx_void_t           lx_fixed_array_init_ptr(lx_fixed_array_ref_t array, lx_pointer_t* items, lx_size_t count);

/*! init memory fixed array
 *
 * @param array     the array
 * @param items     the items
 * @param count     the count
 * @param itemsize  the item size
 */
lx_void_t           lx_fixed_array_init_mem(lx_fixed_array_ref_t array, lx_pointer_t items, lx_size_t count, lx_size_t itemsize);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
