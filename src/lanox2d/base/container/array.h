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
 * @param grow      the item grow
 * @param element   the element
 *
 * @return          the array
 */
lx_array_ref_t      lx_array_init(lx_size_t grow/*, lx_element_t element*/);

/*! exist array
 *
 * @param vector    the array
 */
lx_void_t           lx_array_exit(lx_array_ref_t array);

/*! the array data
 *
 * @param vector    the array
 *
 * @return          the array data
 */
lx_pointer_t        lx_array_data(lx_array_ref_t array);

/*! the array head item
 *
 * @param vector    the array
 *
 * @return          the array head item
 */
lx_pointer_t        lx_array_head(lx_array_ref_t array);

/*! the array last item
 *
 * @param vector    the array
 *
 * @return          the array last item
 */
lx_pointer_t        lx_array_last(lx_array_ref_t array);

/*! resize the array
 *
 * @param vector    the array
 * @param size      the array size
 *
 * @return          lx_true or lx_false
 */
lx_bool_t           lx_array_resize(lx_array_ref_t array, lx_size_t size);

/*! clear the array
 *
 * @param vector    the array
 */
lx_void_t           lx_array_clear(lx_array_ref_t array);

/*! copy the array
 *
 * @param vector    the array
 * @param copy      the copied vector
 */
lx_void_t           lx_array_copy(lx_array_ref_t array, lx_array_ref_t copy);

/*! insert the array prev item
 *
 * @param vector    the array
 * @param itor      the item itor
 * @param data      the item data
 */
lx_void_t           lx_array_insert_prev(lx_array_ref_t array, lx_size_t itor, lx_cpointer_t data);

/*! insert the array next item
 *
 * @param vector    the array
 * @param itor      the item itor
 * @param data      the item data
 */
lx_void_t           lx_array_insert_next(lx_array_ref_t array, lx_size_t itor, lx_cpointer_t data);

/*! insert the array head item
 *
 * @param vector    the array
 * @param data      the item data
 */
lx_void_t           lx_array_insert_head(lx_array_ref_t array, lx_cpointer_t data);

/*! insert the array tail item
 *
 * @param vector    the array
 * @param data      the item data
 */
lx_void_t           lx_array_insert_tail(lx_array_ref_t array, lx_cpointer_t data);

/*! replace the array item
 *
 * @param vector    the array
 * @param itor      the item itor
 * @param data      the item data
 */
lx_void_t           lx_array_replace(lx_array_ref_t array, lx_size_t itor, lx_cpointer_t data);

/*! replace the array head item
 *
 * @param vector    the array
 * @param data      the item data
 */
lx_void_t           lx_array_replace_head(lx_array_ref_t array, lx_cpointer_t data);

/*! replace the array last item
 *
 * @param vector    the array
 * @param data      the item data
 */
lx_void_t           lx_array_replace_last(lx_array_ref_t array, lx_cpointer_t data);

/*! remove the array item
 *
 * @param vector    the array
 * @param itor      the item itor
 */
lx_void_t           lx_array_remove(lx_array_ref_t array, lx_size_t itor);

/*! remove the array head item
 *
 * @param vector    the array
 */
lx_void_t           lx_array_remove_head(lx_array_ref_t array);

/*! remove the array last item
 *
 * @param vector    the array
 */
lx_void_t           lx_array_remove_last(lx_array_ref_t array);

/*! the array size
 *
 * @param vector    the array
 *
 * @return          the array size
 */
lx_size_t           lx_array_size(lx_array_ref_t array);

/*! the array grow
 *
 * @param vector    the array
 *
 * @return          the array grow
 */
lx_size_t           lx_array_grow(lx_array_ref_t array);

/*! the array maxn
 *
 * @param vector    the array
 *
 * @return          the array maxn
 */
lx_size_t           lx_array_maxn(lx_array_ref_t array);


/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


