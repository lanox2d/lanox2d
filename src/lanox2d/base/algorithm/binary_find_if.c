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
 * @file        binary_find_if.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "binary_find_if.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_size_t lx_binary_find_if(lx_iterator_ref_t iterator, lx_size_t head, lx_size_t tail, lx_iterator_comp_t comp, lx_cpointer_t udata) {
    lx_assert_and_check_return_val(comp && iterator && (lx_iterator_mode(iterator) & LX_ITERATOR_MODE_RACCESS), lx_iterator_tail(iterator));
    lx_check_return_val(head != tail, lx_iterator_tail(iterator));

    lx_size_t l = head;
    lx_size_t r = tail;
    lx_size_t m = (l + r) >> 1;
    lx_long_t c = -1;
    while (l < r) {
        c = comp(iterator, lx_iterator_item(iterator, m), udata);
        if (c > 0) r = m;
        else if (c < 0) l = m + 1;
        else break;
        m = (l + r) >> 1;
    }
    return !c? m : lx_iterator_tail(iterator);
}

lx_size_t lx_binary_find_all_if(lx_iterator_ref_t iterator, lx_iterator_comp_t comp, lx_cpointer_t udata) {
    return lx_binary_find_if(iterator, lx_iterator_head(iterator), lx_iterator_tail(iterator), comp, udata);
}

