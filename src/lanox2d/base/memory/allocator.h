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
 * @file        allocator.h
 *
 */
#ifndef LX_BASE_ALLOCATOR_H
#define LX_BASE_ALLOCATOR_H

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

/// the allocator type
typedef struct __lx_allocator_t {

    /*! malloc data
     *
     * @param allocator     the allocator
     * @param size          the size
     *
     * @return              the data address
     */
    lx_pointer_t            (*malloc)(struct __lx_allocator_t* allocator, lx_size_t size);

    /*! malloc data and fill zero
     *
     * @param allocator     the allocator
     * @param size          the size
     *
     * @return              the data address
     */
    lx_pointer_t            (*malloc0)(struct __lx_allocator_t* allocator, lx_size_t size);

    /*! realloc data
     *
     * @param allocator     the allocator
     * @param data          the data address
     * @param size          the data size
     *
     * @return              the new data address
     */
    lx_pointer_t            (*ralloc)(struct __lx_allocator_t* allocator, lx_pointer_t data, lx_size_t size);

    /*! free data
     *
     * @param allocator     the allocator
     * @param data          the data address
     */
    lx_void_t               (*free)(struct __lx_allocator_t* allocator, lx_pointer_t data);

}lx_allocator_t, *lx_allocator_ref_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! get the current allocator
 *
 * @return              the allocator
 */
lx_allocator_ref_t      lx_allocator(lx_noarg_t);

/*! set the current allocator
 *
 * @param               the allocator
 */
lx_void_t               lx_allocator_set(lx_allocator_ref_t allocator);

/*! malloc data
 *
 * @param allocator     the allocator
 * @param size          the size
 *
 * @return              the data address
 */
lx_pointer_t            lx_allocator_malloc(lx_allocator_ref_t allocator, lx_size_t size);

/*! malloc data and fill zero
 *
 * @param allocator     the allocator
 * @param size          the size
 *
 * @return              the data address
 */
lx_pointer_t            lx_allocator_malloc0(lx_allocator_ref_t allocator, lx_size_t size);

/*! malloc data with the item count
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 *
 * @return              the data address
 */
lx_pointer_t            lx_allocator_nalloc(lx_allocator_ref_t allocator, lx_size_t item, lx_size_t size);

/*! malloc data with the item count and fill zero
 *
 * @param allocator     the allocator
 * @param item          the item count
 * @param size          the item size
 *
 * @return              the data address
 */
lx_pointer_t            lx_allocator_nalloc0(lx_allocator_ref_t allocator, lx_size_t item, lx_size_t size);

/*! realloc data
 *
 * @param allocator     the allocator
 * @param data          the data address
 * @param size          the data size
 *
 * @return              the new data address
 */
lx_pointer_t            lx_allocator_ralloc(lx_allocator_ref_t allocator, lx_pointer_t data, lx_size_t size);

/*! free data
 *
 * @param allocator     the allocator
 * @param data          the data address
 *
 */
lx_void_t               lx_allocator_free(lx_allocator_ref_t allocator, lx_pointer_t data);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


