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
 * @file        binary_find.h
 *
 */
#ifndef LX_BASE_ALGORITHM_BINARY_FIND_H
#define LX_BASE_ALGORITHM_BINARY_FIND_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_enter

/* //////////////////////////////////////////////////////////////////////////////////////
 * interfaces
 */

/*! binary find item
 *
 * @param iterator  the iterator
 * @param head      the iterator head
 * @param tail      the iterator tail
 * @param item      the finded item
 *
 * @return          the iterator itor, return lx_iterator_tail(iterator) if not found
 */
lx_size_t           lx_binary_find(lx_iterator_ref_t iterator, lx_size_t head, lx_size_t tail, lx_cpointer_t item);

/*!binary find item for all
 *
 * @param iterator  the iterator
 * @param item      the finded item
 *
 * @return          the iterator itor, return lx_iterator_tail(iterator) if not found
 */
lx_size_t           lx_binary_find_all(lx_iterator_ref_t iterator, lx_cpointer_t item);

/* //////////////////////////////////////////////////////////////////////////////////////
 * extern
 */
lx_extern_c_leave
#endif
