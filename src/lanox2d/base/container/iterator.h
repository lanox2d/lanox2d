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
 * @file        iterator.h
 *
 */
#ifndef LX_BASE_CONTAINER_ITERATOR_H
#define LX_BASE_CONTAINER_ITERATOR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * types
 */

/// the iterator mode type
typedef enum lx_iterator_mode_t_ {
    LX_ITERATOR_MODE_FORWARD        = 1     //!< forward iterator
,   LX_ITERATOR_MODE_REVERSE        = 2     //!< reverse iterator
,   LX_ITERATOR_MODE_RACCESS        = 4     //!< random access iterator
,   LX_ITERATOR_MODE_MUTABLE        = 8     //!< mutable iterator, the item of the same iterator is mutable for removing and moving, .e.g vector, hash, ...
,   LX_ITERATOR_MODE_READONLY       = 16    //!< readonly iterator
}lx_iterator_mode_t;

/// the iterator operation type
struct lx_iterator_t_;
typedef struct lx_iterator_op_t_ {
    lx_size_t               (*head)(struct lx_iterator_t_* iterator);
    lx_size_t               (*tail)(struct lx_iterator_t_* iterator);
    lx_size_t               (*prev)(struct lx_iterator_t_* iterator, lx_size_t itor);
    lx_size_t               (*next)(struct lx_iterator_t_* iterator, lx_size_t itor);
    lx_pointer_t            (*item)(struct lx_iterator_t_* iterator, lx_size_t itor);
}lx_iterator_op_t;

/// the iterator operation ref type
typedef lx_iterator_op_t const* lx_iterator_op_ref_t;

/// the iterator type
typedef struct lx_iterator_t_ {
    lx_size_t               mode;
    lx_size_t               step;
    lx_pointer_t            udata;
    lx_iterator_op_ref_t    op;
}lx_iterator_t, *lx_iterator_ref_t;

/*! the iterator base type in container
 *
 * typedef struct lx_container_t_ {
 *      lx_iterator_base_t base;
 *      // ...
 * }lx_container_t;
 */
typedef struct lx_iterator_base_t_ {
    lx_void_t (*iterator)(lx_iterator_t* iterator);
}lx_iterator_base_t;

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! the iterator mode
 *
 * @param iterator  the iterator
 *
 * @return          the iterator mode
 */
lx_size_t           lx_iterator_mode(lx_iterator_ref_t iterator);

/*! the iterator head
 *
 * @param iterator  the iterator
 *
 * @return          the iterator head
 */
lx_size_t           lx_iterator_head(lx_iterator_ref_t iterator);

/*! the iterator tail
 *
 * @param iterator  the iterator
 *
 * @return          the iterator tail
 */
lx_size_t           lx_iterator_tail(lx_iterator_ref_t iterator);

/*! the iterator prev
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 *
 * @return          the iterator prev
 */
lx_size_t           lx_iterator_prev(lx_iterator_ref_t iterator, lx_size_t itor);

/*! the iterator next
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 *
 * @return          the iterator next
 */
lx_size_t           lx_iterator_next(lx_iterator_ref_t iterator, lx_size_t itor);

/*! the iterator item
 *
 * @param iterator  the iterator
 * @param itor      the item itor
 *
 * @return          the iterator item
 */
lx_pointer_t        lx_iterator_item(lx_iterator_ref_t iterator, lx_size_t itor);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave

#endif


