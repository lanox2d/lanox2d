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
 * @file        foreach.c
 *
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "foreach.h"
#include "for.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_size_t lx_foreach(lx_iterator_ref_t iterator, lx_size_t head, lx_size_t tail, lx_foreach_cb_t callback, lx_cpointer_t udata) {
    lx_assert_and_check_return_val(iterator && (lx_iterator_mode(iterator) & LX_ITERATOR_MODE_FORWARD) && callback, 0);
    lx_check_return_val(head != tail, 0);

    lx_size_t count = 0;
    lx_for (lx_pointer_t, item, head, tail, iterator) {
        if (!callback(iterator, item, udata)) break;
        count++;
    }
    return count;
}
lx_size_t lx_foreach_all(lx_iterator_ref_t iterator, lx_foreach_cb_t callback, lx_cpointer_t udata) {
    lx_assert(iterator && callback);
    return lx_foreach(iterator, lx_iterator_head(iterator), lx_iterator_tail(iterator), callback, udata);
}
