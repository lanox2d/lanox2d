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
 * @file        for.h
 *
 */
#ifndef LX_BASE_ALGORITHM_FOR_H
#define LX_BASE_ALGORITHM_FOR_H

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "prefix.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * macros
 */

/*! for items using iterator
 *
 * @code
 * lx_for(lx_char_t*, item, lx_iterator_head(iterator), lx_iterator_tail(iterator), iterator) {
 *      lx_trace_d("item: %s", item);
 * }
 *
 * lx_for(lx_size_t, item, lx_iterator_head(iterator), lx_iterator_tail(iterator), iterator) {
 *      lx_trace_d("item: %lu", item);
 * }
 *
 * @endcode
 */
#define lx_for(type, item, head, tail, iterator) \
    lx_assert(!iterator || (lx_iterator_mode(iterator) & (LX_ITERATOR_MODE_FORWARD | LX_ITERATOR_MODE_RACCESS))); \
    type item; \
    lx_size_t item##_itor = head; \
    lx_size_t item##_head = head; \
    lx_size_t item##_tail = tail; \
    if (item##_head != item##_tail) \
        for (   ; \
                item##_itor != item##_tail && ((item = (type)lx_iterator_item(iterator, item##_itor)), 1); \
                item##_itor = lx_iterator_next(iterator, item##_itor))

/*! for all items of container
 *
 * @code
 *
 * lx_for_all(lx_char_t*, item, container) {
 *      lx_trace_d("item: %s", item);
 * }
 *
 * lx_for_all(lx_size_t, item, container) {
 *      lx_trace_d("item: %lu", item);
 * }
 *
 * @endcode
 */
#define lx_for_all(type, item, container) \
            lx_iterator_t item##_iterator_all; \
            lx_iterator_of(&item##_iterator_all, container); \
            lx_for(type, item, lx_iterator_head(&item##_iterator_all), lx_iterator_tail(&item##_iterator_all), &item##_iterator_all)

#endif
