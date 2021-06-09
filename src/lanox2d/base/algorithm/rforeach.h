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
 * @file        rforeach.h
 *
 */
#ifndef LX_BASE_ALGORITHM_RFOREACH_H
#define LX_BASE_ALGORITHM_RFOREACH_H

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

/// the rforeach callback type
typedef lx_bool_t   (*lx_rforeach_cb_t)(lx_iterator_ref_t iterator, lx_pointer_t item, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! rforeach
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param callback  the item callback
 * @param udata     the user data
 *
 * @return          the item count
 */
lx_size_t           lx_rforeach(lx_iterator_ref_t iterator, lx_size_t head, lx_size_t tail, lx_rforeach_cb_t callback, lx_cpointer_t udata);

/*! rforeach all
 *
 * @param container the container
 * @param callback  the item callback
 * @param udata     the user data
 *
 * @return          the item count
 */
lx_size_t           lx_rforeach_all(lx_cpointer_t container, lx_rforeach_cb_t callback, lx_cpointer_t udata);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif
