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
 * @file        array_iterator.h
 *
 */
#ifndef LX_BASE_CONTAINER_ARRAY_ITERATOR_H
#define LX_BASE_CONTAINER_ARRAY_ITERATOR_H

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
typedef struct lx_array_iterator_t_ {
    lx_iterator_base_t      base;
    lx_pointer_t            items;
    lx_size_t               count;
}lx_array_iterator_t, *lx_array_iterator_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init array iterator for pointer element
 *
 * @param iterator  the array iterator
 * @param items     the items
 * @param count     the count
 */
lx_void_t           lx_array_iterator_init_ptr(lx_array_iterator_ref_t iterator, lx_pointer_t* items, lx_size_t count);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
