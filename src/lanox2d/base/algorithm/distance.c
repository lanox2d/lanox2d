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
 * @file        distance.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "distance.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_size_t lx_distance(lx_iterator_ref_t iterator, lx_size_t head, lx_size_t tail) {
    lx_assert_and_check_return_val(iterator, 0);
    lx_check_return_val(head != tail, 0);

    lx_size_t distance = 0;
    lx_size_t mode = lx_iterator_mode(iterator);
    if (mode & LX_ITERATOR_MODE_RACCESS) {
        distance = tail - head;
    } else if (mode & LX_ITERATOR_MODE_FORWARD) {
        if (lx_iterator_head(iterator) == head && lx_iterator_tail(iterator) == tail) {
            distance = lx_iterator_size(iterator);
        } else {
            lx_size_t itor = head;
            for (; itor != tail; itor = lx_iterator_next(iterator, itor)) distance++;
        }
    } else if (mode & LX_ITERATOR_MODE_REVERSE) {
        if (lx_iterator_head(iterator) == head && lx_iterator_tail(iterator) == tail) {
            distance = lx_iterator_size(iterator);
        } else {
            lx_size_t itor = tail;
            do {
                distance++;
                itor = lx_iterator_prev(iterator, itor);
            } while (itor != head);
        }
    } else {
        lx_assert(0);
    }
    return distance;
}
