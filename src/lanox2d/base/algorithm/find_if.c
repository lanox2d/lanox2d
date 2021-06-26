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
 * @file        find_if.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "find_if.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_size_t lx_find_if(lx_iterator_ref_t iterator, lx_size_t head, lx_size_t tail, lx_predicate_ref_t pred, lx_cpointer_t value) {
    lx_assert_and_check_return_val(pred && iterator && (lx_iterator_mode(iterator) & LX_ITERATOR_MODE_FORWARD), lx_iterator_tail(iterator));
    lx_check_return_val(head != tail, lx_iterator_tail(iterator));

    lx_size_t itor = head;
    lx_bool_t find = lx_false;
    for (; itor != tail; itor = lx_iterator_next(iterator, itor)) {
        if ((find = pred(iterator, lx_iterator_item(iterator, itor), value))) {
            break;
        }
    }
    return find? itor : lx_iterator_tail(iterator);
}

lx_size_t lx_find_all_if(lx_iterator_ref_t iterator, lx_predicate_ref_t pred, lx_cpointer_t value) {
    return lx_find_if(iterator, lx_iterator_head(iterator), lx_iterator_tail(iterator), pred, value);
}

