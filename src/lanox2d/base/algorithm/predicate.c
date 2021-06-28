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
 * @file        predicate.c
 */

/* //////////////////////////////////////////////////////////////////////////////////////
 * includes
 */
#include "predicate.h"

/* //////////////////////////////////////////////////////////////////////////////////////
 * implementation
 */
lx_bool_t lx_predicate_eq(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t value) {
    lx_assert(iterator);
    return !lx_iterator_comp(iterator, item, value);
}

lx_bool_t lx_predicate_le(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t value) {
    lx_assert(iterator);
    return lx_iterator_comp(iterator, item, value) < 0;
}

lx_bool_t lx_predicate_be(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t value) {
    lx_assert(iterator);
    return lx_iterator_comp(iterator, item, value) > 0;
}

lx_bool_t lx_predicate_leq(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t value) {
    lx_assert(iterator);
    return lx_iterator_comp(iterator, item, value) <= 0;
}

lx_bool_t lx_predicate_beq(lx_iterator_ref_t iterator, lx_cpointer_t item, lx_cpointer_t value) {
    lx_assert(iterator);
    return lx_iterator_comp(iterator, item, value) >= 0;
}
