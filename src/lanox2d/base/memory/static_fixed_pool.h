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
 * @file        static_fixed_pool.h
 *
 */
#ifndef LX_BASE_MEMORY_STATIC_FIXED_POOL_H
#define LX_BASE_MEMORY_STATIC_FIXED_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "fixed_pool.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the static fixed pool ref type
 *
 * <pre>
 *  ---------------------------------------------------------------------------
 * |  head   |      used       |                    data                       |
 *  ---------------------------------------------------------------------------
 *               |
 *              pred
 * </pre>
 */
typedef lx_typeref(static_fixed_pool);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init pool
 *
 * @param data              the data address
 * @param size              the data size
 * @param item_size         the item size
 * @param for_small         add data size field at head for the small allocator
 *
 * @return                  the pool
 */
lx_static_fixed_pool_ref_t  lx_static_fixed_pool_init(lx_byte_t* data, lx_size_t size, lx_size_t item_size, lx_bool_t for_small);

/*! exit pool
 *
 * @param pool              the pool
 */
lx_void_t                   lx_static_fixed_pool_exit(lx_static_fixed_pool_ref_t pool);

/*! the item count
 *
 * @param pool              the pool
 *
 * @return                  the item count
 */
lx_size_t                   lx_static_fixed_pool_size(lx_static_fixed_pool_ref_t pool);

/*! the item maximum count
 *
 * @param pool              the pool
 *
 * @return                  the item maximum count
 */
lx_size_t                   lx_static_fixed_pool_maxn(lx_static_fixed_pool_ref_t pool);

/*! is full?
 *
 * @param pool              the pool
 *
 * @return                  lx_true or lx_false
 */
lx_bool_t                   lx_static_fixed_pool_full(lx_static_fixed_pool_ref_t pool);

/*! is null?
 *
 * @param pool              the pool
 *
 * @return                  lx_true or lx_false
 */
lx_bool_t                   lx_static_fixed_pool_null(lx_static_fixed_pool_ref_t pool);

/*! clear pool
 *
 * @param pool              the pool
 */
lx_void_t                   lx_static_fixed_pool_clear(lx_static_fixed_pool_ref_t pool);

/*! malloc data
 *
 * @param pool              the pool
 *
 * @return                  the data
 */
lx_pointer_t                lx_static_fixed_pool_malloc(lx_static_fixed_pool_ref_t pool);

/*! free data
 *
 * @param pool              the pool
 * @param data              the data
 *
 * @return                  lx_true or lx_false
 */
lx_bool_t                   lx_static_fixed_pool_free(lx_static_fixed_pool_ref_t pool, lx_pointer_t data);

/*! walk data
 *
 * @code
 * lx_bool_t lx_static_fixed_pool_item_func(lx_pointer_t data, lx_cpointer_t udata) {
 *      // ok or break
 *      return lx_true;
 * }
 * @endcode
 *
 * @param pool              the pool
 * @param func              the walk func
 * @param udata             the user data
 *
 */
lx_void_t                   lx_static_fixed_pool_walk(lx_static_fixed_pool_ref_t pool, lx_fixed_pool_item_walk_func_t func, lx_cpointer_t udata);

#ifdef LX_DEBUG
/*! dump pool
 *
 * @param pool              the pool
 */
lx_void_t                   lx_static_fixed_pool_dump(lx_static_fixed_pool_ref_t pool);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
