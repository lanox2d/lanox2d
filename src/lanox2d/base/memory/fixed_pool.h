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
 * @file        fixed_pool.h
 */
#ifndef LX_BASE_MEMORY_FIXED_POOL_H
#define LX_BASE_MEMORY_FIXED_POOL_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "allocator.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/*! the fixed pool ref type
 *
 * <pre>
 *
 * current:
 *         -----------
 *        |           |
 *  --------------    |
 * |     slot     |<--
 * |--------------|
 * ||||||||||||||||
 * |--------------|
 * |              |
 * |--------------|
 * |              |
 * |--------------|
 * ||||||||||||||||
 * |--------------|
 * ||||||||||||||||
 * |--------------|
 * |              |
 *  --------------
 *
 * partial:
 *
 *  --------------       --------------               --------------
 * |     slot     | <=> |     slot     | <=> ... <=> |     slot     |
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     |              |             |              |
 * |--------------|     |--------------|             |--------------|
 * |              |     ||||||||||||||||             |              |
 * |--------------|     |--------------|             |--------------|
 * |              |     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             |              |
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     |              |             |              |
 * |--------------|     |--------------|             |--------------|
 * |              |     |              |             ||||||||||||||||
 *  --------------       --------------               --------------
 *
 * full:
 *
 *  --------------       --------------               --------------
 * |     slot     | <=> |     slot     | <=> ... <=> |     slot     |
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 * |--------------|     |--------------|             |--------------|
 * ||||||||||||||||     ||||||||||||||||             ||||||||||||||||
 *  --------------       --------------               --------------
 *
 * slot:
 *
 *  -------------- ------------------------>|
 * |     head     |                         |
 * |--------------|                         |
 * |||   item     |                         |
 * |--------------|                         |
 * |||   item     |                         |
 * |--------------|                         | data
 * |||   item     |                         |
 * |--------------|                         |
 * |      ...     |                         |
 * |--------------|                         |
 * |||   item     |                         |
 *  -------------- ------------------------>|
 *
 * </pre>
 */
typedef lx_typeref(fixed_pool);

/// the item init callback type
typedef lx_bool_t       (*lx_fixed_pool_item_init_cb_t)(lx_pointer_t data, lx_cpointer_t udata);

/// the item exit callback type
typedef lx_void_t       (*lx_fixed_pool_item_exit_cb_t)(lx_pointer_t data, lx_cpointer_t udata);

/// the item foreach callback type
typedef lx_bool_t       (*lx_fixed_pool_item_foreach_cb_t)(lx_pointer_t data, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! init fixed pool
 *
 * @param slot_size         the item count per-slot, using the default size if be zero
 * @param item_size         the item size
 * @param item_init         the item init callback
 * @param item_exit         the item exit callback
 * @param udata             the user data
 *
 * @return                  the pool
 */
lx_fixed_pool_ref_t         lx_fixed_pool_init(lx_size_t slot_size, lx_size_t item_size, lx_fixed_pool_item_init_cb_t item_init, lx_fixed_pool_item_exit_cb_t item_exit, lx_cpointer_t udata);

/*! exit pool
 *
 * @param pool              the pool
 */
lx_void_t                   lx_fixed_pool_exit(lx_fixed_pool_ref_t pool);

/*! the item count
 *
 * @param pool              the pool
 *
 * @return                  the item count
 */
lx_size_t                   lx_fixed_pool_size(lx_fixed_pool_ref_t pool);

/*! clear pool
 *
 * @param pool              the pool
 */
lx_void_t                   lx_fixed_pool_clear(lx_fixed_pool_ref_t pool);

/*! malloc data
 *
 * @param pool              the pool
 *
 * @return                  the data
 */
lx_pointer_t                lx_fixed_pool_malloc(lx_fixed_pool_ref_t pool);

/*! malloc data and clear it
 *
 * @param pool              the pool
 *
 * @return                  the data
 */
lx_pointer_t                lx_fixed_pool_malloc0(lx_fixed_pool_ref_t pool);

/*! free data
 *
 * @param pool              the pool
 * @param data              the data
 *
 * @return                  lx_true or lx_false
 */
lx_bool_t                   lx_fixed_pool_free(lx_fixed_pool_ref_t pool, lx_pointer_t data);

/*! foreach item
 *
 * @code
    lx_bool_t lx_fixed_pool_item(lx_pointer_t data, lx_cpointer_t udata) {
        // ok or break
        return lx_true;
    }
 * @endcode
 *
 * @param pool              the pool
 * @param callback          the foreach callback
 * @param udata             the user data
 */
lx_void_t                   lx_fixed_pool_foreach(lx_fixed_pool_ref_t pool, lx_fixed_pool_item_foreach_cb_t callback, lx_cpointer_t udata);

#ifdef LX_DEBUG
/*! dump pool
 *
 * @param pool              the pool
 */
lx_void_t                   lx_fixed_pool_dump(lx_fixed_pool_ref_t pool);
#endif

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif
