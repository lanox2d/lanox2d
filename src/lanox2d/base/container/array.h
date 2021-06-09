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
 * @file        array.h
 *
 */
#ifndef LX_BASE_CONTAINER_ARRAY_H
#define LX_BASE_CONTAINER_ARRAY_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the array ref type
typedef lx_typeref(array);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init array
 *
 * @param grow      the grow number of elements
 * @param itemsize  the item size
 * @param itemfree  the free function of element item
 *
 * @return          the array
 */
lx_array_ref_t      lx_array_init(lx_size_t grow, lx_size_t itemsize, lx_element_free_t itemfree);

/*! exist array
 *
 * @param array     the array
 */
lx_void_t           lx_array_exit(lx_array_ref_t array);

/*! get the array data
 *
 * @param array     the array
 *
 * @return          the array data
 */
lx_pointer_t        lx_array_data(lx_array_ref_t array);

/*! get the array size
 *
 * @param array     the array
 *
 * @return          the array size
 */
lx_size_t           lx_array_size(lx_array_ref_t array);

/*! get the array head item
 *
 * @param array     the array
 *
 * @return          the array head item
 */
lx_pointer_t        lx_array_head(lx_array_ref_t array);

/*! get the array last item
 *
 * @param array     the array
 *
 * @return          the array last item
 */
lx_pointer_t        lx_array_last(lx_array_ref_t array);

/*! get the array item at the given index
 *
 * @param array     the array
 * @param index     the item index
 *
 * @return          the array item
 */
lx_pointer_t        lx_array_item(lx_array_ref_t array, lx_size_t index);

/*! resize the array
 *
 * @param array     the array
 * @param size      the array size
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_array_resize(lx_array_ref_t array, lx_size_t size);

/*! clear the array
 *
 * @param array     the array
 */
lx_void_t           lx_array_clear(lx_array_ref_t array);

/*! copy the array
 *
 * @param array     the array
 * @param copy      the copied array
 */
lx_void_t           lx_array_copy(lx_array_ref_t array, lx_array_ref_t copied);

/*! insert new item at given index
 *
 * @param array     the array
 * @param index     the item index
 * @param data      the item data
 */
lx_void_t           lx_array_insert(lx_array_ref_t array, lx_size_t index, lx_cpointer_t data);

/*! insert new item at tail
 *
 * @param array     the array
 * @param data      the item data
 */
lx_void_t           lx_array_insert_tail(lx_array_ref_t array, lx_cpointer_t data);

/*! replace the item at given index
 *
 * @param array     the array
 * @param index     the item index
 * @param data      the item data
 */
lx_void_t           lx_array_replace(lx_array_ref_t array, lx_size_t index, lx_cpointer_t data);

/*! replace the head item
 *
 * @param array     the array
 * @param data      the item data
 */
lx_void_t           lx_array_replace_head(lx_array_ref_t array, lx_cpointer_t data);

/*! replace the last item
 *
 * @param array     the array
 * @param data      the item data
 */
lx_void_t           lx_array_replace_last(lx_array_ref_t array, lx_cpointer_t data);

/*! remove the last item
 *
 * @param array     the array
 */
lx_void_t           lx_array_remove_last(lx_array_ref_t array);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


